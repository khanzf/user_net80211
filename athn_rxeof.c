#include <stdio.h>
#include <string.h>
#include "athn.h"
#include "mycompat.h"

void
athn_usb_rx_frame(struct athn_usb_softc *usc, struct mbuf *m)
//    struct mbuf_list *ml)
{
	struct athn_softc *sc = &usc->sc_sc;
	//struct ieee80211com *ic = &sc->sc_ic;
	//struct ifnet *ifp = &ic->ic_if;
	struct ieee80211_frame *wh;
	struct ieee80211_node *ni;
	struct ieee80211_rx_stats rxi;
	struct ar_htc_frame_hdr *htc;
	struct ar_rx_status *rs;
	uint16_t datalen;

	if (__predict_false(m->m_len < sizeof(*htc)))
		goto skip;
	htc = mtod(m, struct ar_htc_frame_hdr *);
	if (__predict_false(htc->endpoint_id == 0)) {
		printf("bad endpoint %d\n", htc->endpoint_id);
		goto skip;
	}
	if (htc->flags & AR_HTC_FLAG_TRAILER) {
		if (m->m_len < htc->control[0])
			goto skip;
		m_adj(m, -(int)htc->control[0]);
	}
	m_adj(m, sizeof(*htc));	/* Strip HTC header. */

	if (__predict_false(m->m_len < sizeof(*rs)))
		goto skip;
	rs = mtod(m, struct ar_rx_status *);

	/* Make sure that payload fits. */
	datalen = be16toh(rs->rs_datalen);
	if (__predict_false(m->m_len < sizeof(*rs) + datalen))
		goto skip;

	if (__predict_false(datalen < sizeof(*wh) + IEEE80211_CRC_LEN))
		goto skip;

	if (rs->rs_status != 0) {
//		if (rs->rs_status & AR_RXS_RXERR_DECRYPT)
//			ic->ic_stats.is_ccmp_dec_errs++;
		//ifp->if_ierrors++;
		goto skip;
	}
	m_adj(m, sizeof(*rs));	/* Strip Rx status. */

	/* Grab a reference to the source node. */
	wh = mtod(m, struct ieee80211_frame *);
	ni = ieee80211_find_rxnode(ic, wh);

	/* Remove any HW padding after the 802.11 header. */
	if (!(wh->i_fc[0] & IEEE80211_FC0_TYPE_CTL)) {
		uint32_t hdrlen = ieee80211_hdrsize(wh);
		if (hdrlen & 3) {
			memmove((caddr_t)wh + 2, wh, hdrlen);
			m_adj(m, 2);
		}
		wh = mtod(m, struct ieee80211_frame *);
	}
#if NBPFILTER > 0
	if (__predict_false(sc->sc_drvbpf != NULL))
		athn_usb_rx_radiotap(sc, m, rs);
#endif
	/* Trim 802.11 FCS after radiotap. */
	m_adj(m, -IEEE80211_CRC_LEN);

	/* Send the frame to the 802.11 layer. */
	memset(&rxi, 0, sizeof(rxi));
#if 0
	rxi.rxi_rssi = rs->rs_rssi + AR_USB_DEFAULT_NF;
	rxi.rxi_tstamp = betoh64(rs->rs_tstamp);
	if (!(wh->i_fc[0] & IEEE80211_FC0_TYPE_CTL) &&
	    (wh->i_fc[1] & IEEE80211_FC1_PROTECTED) &&
	    (ic->ic_flags & IEEE80211_F_RSNON) &&
	    (ni->ni_flags & IEEE80211_NODE_RXPROT) &&
	    (ni->ni_rsncipher == IEEE80211_CIPHER_CCMP ||
	    (IEEE80211_IS_MULTICAST(wh->i_addr1) &&
	    ni->ni_rsngroupcipher == IEEE80211_CIPHER_CCMP))) {
		if (ar5008_ccmp_decap(sc, m, ni) != 0) {
			//ifp->if_ierrors++;
			ieee80211_release_node(ic, ni);
			goto skip;
		}
		rxi.rxi_flags |= IEEE80211_RXI_HWDEC;
	}
#endif
//	ieee80211_inputm(ifp, m, ni, &rxi, ml);

	/* Node is no longer needed. */
	//ieee80211_release_node(ic, ni);
	return;
 skip:
	m_freem(m);
}

void
athn_usb_rxeof(struct usbd_xfer *xfer, void *priv)
//,
//    usbd_status status)
{
//	struct mbuf_list ml = MBUF_LIST_INITIALIZER();
	struct athn_usb_rx_data *data = priv;
	struct athn_usb_softc *usc = data->sc;
	//struct athn_softc *sc = &usc->sc_sc;
	//struct ifnet *ifp = &sc->sc_ic.ic_if;
	struct athn_usb_rx_stream *stream = &usc->rx_stream;
	uint8_t *buf = data->buf;
	struct ar_stream_hdr *hdr;
	struct mbuf *m;
	uint16_t pktlen;
	int off, len;

#if 0
	if (__predict_false(status != USBD_NORMAL_COMPLETION)) {
		DPRINTF(("RX status=%d\n", status));
		if (status == USBD_STALLED)
			usbd_clear_endpoint_stall_async(usc->rx_data_pipe);
		if (status != USBD_CANCELLED)
			goto resubmit;
		return;
	}
	usbd_get_xfer_status(xfer, NULL, NULL, &len, NULL);
#endif

	if (stream->left > 0) {
		if (len >= stream->left) {
			/* We have all our pktlen bytes now. */
			if (__predict_true(stream->m != NULL)) {
				memcpy(mtod(stream->m, uint8_t *) +
				    stream->moff, buf, stream->left);
				athn_usb_rx_frame(usc, stream->m);
				stream->m = NULL;
			}
			/* Next header is 32-bit aligned. */
			off = (stream->left + 3) & ~3;
			buf += off;
			len -= off;
			stream->left = 0;
		} else {
			/* Still need more bytes, save what we have. */
			if (__predict_true(stream->m != NULL)) {
				memcpy(mtod(stream->m, uint8_t *) +
				    stream->moff, buf, len);
				stream->moff += len;
			}
			stream->left -= len;
			goto resubmit;
		}
	}
	KASSERT(stream->left == 0);
	while (len >= sizeof(*hdr)) {
		hdr = (struct ar_stream_hdr *)buf;
		if (hdr->tag != htole16(AR_USB_RX_STREAM_TAG)) {
			
			printf("invalid tag 0x%x\n", hdr->tag);
			break;
		}
		pktlen = le16toh(hdr->len);
		buf += sizeof(*hdr);
		len -= sizeof(*hdr);

		if (__predict_true(pktlen <= MCLBYTES)) {
			/* Allocate an mbuf to store the next pktlen bytes. */
			MGETHDR(m, M_DONTWAIT, MT_DATA);
			if (__predict_true(m != NULL)) {
				m->m_pkthdr.len = m->m_len = pktlen;
				if (pktlen > MHLEN) {
					MCLGET(m, M_DONTWAIT);
					if (!(m->m_flags & M_EXT)) {
						m_free(m);
						m = NULL;
					}
				}
			}
		} else	/* Drop frames larger than MCLBYTES. */
			m = NULL;

		//if (m == NULL)
		//	ifp->if_ierrors++;

		/*
		 * NB: m can be NULL, in which case the next pktlen bytes
		 * will be discarded from the Rx stream.
		 */
		if (pktlen > len) {
			/* Need more bytes, save what we have. */
			stream->m = m;	/* NB: m can be NULL. */
			if (__predict_true(stream->m != NULL)) {
				memcpy(mtod(stream->m, uint8_t *), buf, len);
				stream->moff = len;
			}
			stream->left = pktlen - len;
			goto resubmit;
		}
		if (__predict_true(m != NULL)) {
			/* We have all the pktlen bytes in this xfer. */
			memcpy(mtod(m, uint8_t *), buf, pktlen);
			athn_usb_rx_frame(usc, m);
		}

		/* Next header is 32-bit aligned. */
		off = (pktlen + 3) & ~3;
		buf += off;
		len -= off;
	}
//	if_input(ifp, &ml);

 resubmit:
	/* Setup a new transfer. */
#if 0
	usbd_setup_xfer(xfer, usc->rx_data_pipe, data, data->buf,
	    ATHN_USB_RXBUFSZ, USBD_SHORT_XFER_OK | USBD_NO_COPY,
	    USBD_NO_TIMEOUT, athn_usb_rxeof);
	(void)usbd_transfer(xfer);
#endif
}

