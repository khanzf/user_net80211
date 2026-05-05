#ifndef ATHN_H
#define ATHN_H

#include "mycompat.h"
#include "ieee80211.h"

struct athn_usb_rx_data {
	struct athn_usb_softc	*sc;
//	struct usbd_xfer	*xfer;
	uint8_t			*buf;
};

struct athn_usb_rx_stream {
	struct mbuf	*m;
	int		moff;
	int		left;
};

struct ar_stream_hdr {
	uint16_t	len;
	uint16_t	tag;
#define AR_USB_RX_STREAM_TAG	0x4e00
#define AR_USB_TX_STREAM_TAG	0x697e
} __packed __attribute__((aligned(4)));

#define AR_MAX_CHAINS	3

#define AR_USB_DEFAULT_NF	(-95)

/* Rx descriptor. */
struct ar_rx_status {
	uint64_t	rs_tstamp;
	uint16_t	rs_datalen;
	uint8_t		rs_status;
#define AR_RXS_RXERR_CRC               0x01
#define AR_RXS_RXERR_PHY               0x02
#define AR_RXS_RXERR_FIFO              0x04
#define AR_RXS_RXERR_DECRYPT           0x08
#define AR_RXS_RXERR_MIC               0x10
	uint8_t		rs_phyerr;
	int8_t		rs_rssi;
	int8_t		rs_rssi_ctl[AR_MAX_CHAINS];
	int8_t		rs_rssi_ext[AR_MAX_CHAINS];
	uint8_t		rs_keyix;
	uint8_t		rs_rate;
	uint8_t		rs_antenna;
	uint8_t		rs_more;
	uint8_t		rs_isaggr;
	uint8_t		rs_moreaggr;
	uint8_t		rs_num_delims;
	uint8_t		rs_flags;
#define AR_RXS_FLAG_GI		0x04
#define AR_RXS_FLAG_2040	0x08

	uint8_t		rs_dummy;
	uint32_t	rs_evm[AR_MAX_CHAINS];
} __packed __attribute__((aligned(4)));


struct ar_htc_frame_hdr {
	uint8_t		endpoint_id;
	uint8_t		flags;
#define AR_HTC_FLAG_NEED_CREDIT_UPDATE		0x01
#define AR_HTC_FLAG_TRAILER			0x02
#define AR_HTC_FLAG_CREDIT_REDISTRIBUTION	0x03

	uint16_t	payload_len;
	uint8_t		control[4];
} __packed;

struct athn_softc {
	struct ieee80211com *		sc_ic;
};

struct athn_usb_softc {
	struct athn_softc	sc_sc;

	struct athn_usb_rx_stream rx_stream;
};

void athn_usb_rxeof(struct athn_usb_rx_data *data, int actlen);

#endif
