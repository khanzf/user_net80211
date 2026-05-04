#include <string.h>
#include <string.h>
#include "ieee80211.h"

void init_ieee80211com(struct ieee80211com *ic, void *softc)
{
    memset(ic, 0, sizeof(*ic));
    ic->ic_softc = softc;
    //snprintf(ic->ic_name, sizeof(ic->ic_name), "athn0");

    ic->ic_opmode = IEEE80211_M_STA;
    ic->ic_caps = IEEE80211_C_AES | IEEE80211_C_PMGT;

    /* Set a dummy current channel (e.g. channel 1, 2.412 GHz) */
    static struct ieee80211_channel dummy_chan = {
        .ic_freq = 2412,
        .ic_ieee = 1,
        .ic_flags = 0
    };
    ic->ic_curchan = &dummy_chan;
}

unsigned int
ieee80211_hdrsize(const void *data)
{
    const struct ieee80211_frame *wh = (const struct ieee80211_frame *)data;
    uint16_t fc = le16toh(*(const uint16_t *)wh->i_fc);
    unsigned int hdrlen;

    switch (fc & IEEE80211_FC0_TYPE_MASK) {
    case IEEE80211_FC0_TYPE_DATA:
        if ((fc & (IEEE80211_FC1_DIR_FROMDS | IEEE80211_FC1_DIR_TODS)) ==
            (IEEE80211_FC1_DIR_FROMDS | IEEE80211_FC1_DIR_TODS))
            hdrlen = 30;                    /* 4 address fields */
        else
            hdrlen = 24;                    /* 3 address fields */

        if (fc & IEEE80211_FC0_SUBTYPE_QOS)
            hdrlen += 2;                    /* QoS Control */
        break;

    case IEEE80211_FC0_TYPE_CTL:
        switch (fc & IEEE80211_FC0_SUBTYPE_MASK) {
        case IEEE80211_FC0_SUBTYPE_CTS:
        case IEEE80211_FC0_SUBTYPE_ACK:
            hdrlen = 10;
            break;
        default:
            hdrlen = 16;
            break;
        }
        break;

    case IEEE80211_FC0_TYPE_MGT:
    default:
        hdrlen = 24;
        break;
    }

    if (fc & IEEE80211_FC1_ORDER)           /* HT Control field */
        hdrlen += 4;

    return hdrlen;
}