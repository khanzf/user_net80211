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

static inline unsigned int
ieee80211_get_hdrlen(uint16_t fc)
{
    unsigned int hdrlen = 24;   /* Minimum header: 3 addresses */

    switch (fc & IEEE80211_FC0_TYPE_MASK) {
    case IEEE80211_FC0_TYPE_DATA:
        if (fc & (IEEE80211_FC1_DIR_FROMDS | IEEE80211_FC1_DIR_TODS))
            hdrlen = 30;        /* 4 addresses (WDS / Mesh) */

        if (fc & IEEE80211_FC0_SUBTYPE_QOS) {
            hdrlen += 2;        /* QoS Control field */
        }
        break;

    case IEEE80211_FC0_TYPE_CTL:
        switch (fc & IEEE80211_FC0_SUBTYPE_MASK) {
        case IEEE80211_FC0_SUBTYPE_CTS:
        case IEEE80211_FC0_SUBTYPE_ACK:
            hdrlen = 10;
            break;
        default:
            hdrlen = 16;        /* RTS, BAR, etc. */
            break;
        }
        break;

    case IEEE80211_FC0_TYPE_MGT:
        /* Management frames have 24 bytes header */
        hdrlen = 24;
        break;
    }

    /* Add HT Control field (4 bytes) if present */
    if (fc & IEEE80211_FC1_ORDER)
        hdrlen += 4;

    return hdrlen;
}