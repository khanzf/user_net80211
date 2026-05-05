#ifndef IEEE80211COM_H
#define IEEE80211COM_H

#include <string.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include "mbuf.h"        /* Your mbuf emulation */

#define IEEE80211_FC0_VERSION_MASK      0x03
#define IEEE80211_FC0_VERSION_SHIFT     0
#define IEEE80211_FC0_VERSION_0         0x00
#define IEEE80211_FC0_TYPE_MASK         0x0c
#define IEEE80211_FC0_TYPE_SHIFT        2
#define IEEE80211_FC0_TYPE_MGT          0x00
#define IEEE80211_FC0_TYPE_CTL          0x04
#define IEEE80211_FC0_TYPE_DATA         0x08

#define IEEE80211_FC0_SUBTYPE_MASK      0xf0
#define IEEE80211_FC0_SUBTYPE_SHIFT     4
/* for TYPE_MGT */
#define IEEE80211_FC0_SUBTYPE_ASSOC_REQ     0x00
#define IEEE80211_FC0_SUBTYPE_ASSOC_RESP    0x10
#define IEEE80211_FC0_SUBTYPE_REASSOC_REQ   0x20
#define IEEE80211_FC0_SUBTYPE_REASSOC_RESP  0x30
#define IEEE80211_FC0_SUBTYPE_PROBE_REQ     0x40
#define IEEE80211_FC0_SUBTYPE_PROBE_RESP    0x50
#define IEEE80211_FC0_SUBTYPE_TIMING_ADV    0x60
#define IEEE80211_FC0_SUBTYPE_BEACON        0x80
#define IEEE80211_FC0_SUBTYPE_ATIM      0x90
#define IEEE80211_FC0_SUBTYPE_DISASSOC      0xa0
#define IEEE80211_FC0_SUBTYPE_AUTH      0xb0
#define IEEE80211_FC0_SUBTYPE_DEAUTH        0xc0
#define IEEE80211_FC0_SUBTYPE_ACTION        0xd0
#define IEEE80211_FC0_SUBTYPE_ACTION_NOACK  0xe0
/* for TYPE_CTL */
#define IEEE80211_FC0_SUBTYPE_CONTROL_WRAP  0x70
#define IEEE80211_FC0_SUBTYPE_BAR       0x80
#define IEEE80211_FC0_SUBTYPE_BA        0x90
#define IEEE80211_FC0_SUBTYPE_PS_POLL       0xa0
#define IEEE80211_FC0_SUBTYPE_RTS       0xb0
#define IEEE80211_FC0_SUBTYPE_CTS       0xc0
#define IEEE80211_FC0_SUBTYPE_ACK       0xd0
#define IEEE80211_FC0_SUBTYPE_CF_END        0xe0
#define IEEE80211_FC0_SUBTYPE_CF_END_ACK    0xf0
/* for TYPE_DATA (bit combination) */
#define IEEE80211_FC0_SUBTYPE_DATA      0x00
#define IEEE80211_FC0_SUBTYPE_CF_ACK        0x10
#define IEEE80211_FC0_SUBTYPE_CF_POLL       0x20
#define IEEE80211_FC0_SUBTYPE_CF_ACPL       0x30
#define IEEE80211_FC0_SUBTYPE_NODATA        0x40
#define IEEE80211_FC0_SUBTYPE_CFACK     0x50
#define IEEE80211_FC0_SUBTYPE_CFPOLL        0x60
#define IEEE80211_FC0_SUBTYPE_CF_ACK_CF_ACK 0x70
#define IEEE80211_FC0_SUBTYPE_QOS       0x80
#define IEEE80211_FC0_SUBTYPE_QOS_CFACK     0x90
#define IEEE80211_FC0_SUBTYPE_QOS_CFPOLL    0xa0
#define IEEE80211_FC0_SUBTYPE_QOS_CFACKPOLL 0xb0
#define IEEE80211_FC0_SUBTYPE_QOS_NULL      0xc0

#define IEEE80211_FC1_DIR_MASK          0x03
#define IEEE80211_FC1_DIR_NODS          0x00
#define IEEE80211_FC1_DIR_TODS          0x01
#define IEEE80211_FC1_DIR_FROMDS        0x02
#define IEEE80211_FC1_DIR_DSTODS        0x03

#define IEEE80211_FC1_ORDER             0x80   /* HT Control field present */




#define IEEE80211_IS_MGMT(wh)                   \
    (!! (((wh)->i_fc[0] & IEEE80211_FC0_TYPE_MASK)      \
        == IEEE80211_FC0_TYPE_MGT))
#define IEEE80211_IS_CTL(wh)                    \
    (!! (((wh)->i_fc[0] & IEEE80211_FC0_TYPE_MASK)      \
        == IEEE80211_FC0_TYPE_CTL))
#define IEEE80211_IS_DATA(wh)                   \
    (!! (((wh)->i_fc[0] & IEEE80211_FC0_TYPE_MASK)      \
        == IEEE80211_FC0_TYPE_DATA))


#define	IEEE80211_CRC_LEN			4

typedef char *      caddr_t;    /* core address */

#include <stdint.h>

/*
 * Userspace equivalent of FreeBSD's ieee80211_rx_stats.
 * Contains per-packet receive metadata passed from driver to net80211.
 */
struct ieee80211_rx_stats {
    uint32_t r_flags;        /* IEEE80211_RX_F_* flags */

    /* Channel information */
    uint16_t c_freq;         /* Frequency in MHz */
    uint8_t  c_ieee;         /* IEEE channel number */
    uint8_t  c_band;         /* IEEE80211_CHAN_2GHZ / 5GHZ / etc. */

    /* PHY / modulation info */
    uint8_t  c_phytype;      /* IEEE80211_RX_FP_* (11a/b/g/n/ac/...) */
    uint8_t  c_rate;         /* Rate index or MCS */
    uint16_t c_rxflags;      /* Additional RX flags */

    /* Signal quality */
    int8_t   rssi;           /* Received Signal Strength */
    int8_t   nf;             /* Noise Floor */

    /* MIMO / antenna info */
    uint8_t  r_nantenna;     /* Number of antennas used */
    int8_t   r_antenna[4];   /* Per-antenna RSSI (if supported) */

    /* Other common fields */
    uint32_t r_timestamp;    /* Hardware timestamp (optional) */
    uint32_t r_pktlen;       /* Total packet length (including FCS) */

    /* Add more fields here as your driver code (athn_rxeof) requires them */
};

/* Common flag bits (expand as needed) */
#define IEEE80211_RX_F_11N      0x00000001
#define IEEE80211_RX_F_11AC     0x00000002
#define IEEE80211_RX_F_SHORTPRE 0x00000100
#define IEEE80211_RX_F_CCK      0x00000200
#define IEEE80211_RX_F_OFDM     0x00000400
#define IEEE80211_RX_F_HT       0x00001000
#define IEEE80211_RX_F_VHT      0x00002000

/* PHY types (approximate) */
#define IEEE80211_RX_FP_11A     0
#define IEEE80211_RX_FP_11B     1
#define IEEE80211_RX_FP_11G     2
#define IEEE80211_RX_FP_11N     3
#define IEEE80211_RX_FP_11AC    4


/* Minimal channel structure */
struct ieee80211_channel {
    uint32_t ic_freq;        /* Frequency in MHz */
    uint32_t ic_flags;       /* Channel flags */
    uint8_t  ic_ieee;        /* IEEE channel number */
    uint8_t  ic_maxregpower; /* Regulatory power */
    uint8_t  ic_maxpower;    /* Max power */
};

/* Very minimal ieee80211com for userspace testing */
struct ieee80211com {
    /* Back pointer to driver softc (if needed) */
    void *ic_softc;

    /* Interface */
    char     ic_name[16];    /* e.g. "athn0" */

    /* Capabilities */
    uint32_t ic_caps;        /* Device capabilities (IEEE80211_C_*) */

    /* Current state */
    uint32_t ic_opmode;      /* Operating mode: STA, AP, etc. */
    uint32_t ic_flags;
    uint32_t ic_flags_ext;

    /* Current channel */
    struct ieee80211_channel *ic_curchan;

    /* Supported channels */
    struct ieee80211_channel *ic_channels;
    int                       ic_nchans;

    /* MAC address */
    uint8_t ic_macaddr[6];

    /* Input function - where received frames go */
    void (*ic_input)(struct ieee80211com *ic,
                     struct mbuf *m,
                     int rssi, int noise);

    /* Other commonly used fields */
    uint32_t ic_rt;          /* Current rate table index or similar */
    uint32_t ic_curmode;

    /* You can add more fields as your test code needs them */
};

/* Common capability bits (you can expand) */
#define IEEE80211_C_WEP         0x00000001
#define IEEE80211_C_TKIP        0x00000002
#define IEEE80211_C_AES         0x00000004
#define IEEE80211_C_MONITOR     0x00010000
#define IEEE80211_C_SHPREAMBLE  0x00020000
#define IEEE80211_C_PMGT        0x00100000

/* Operating modes */
#define IEEE80211_M_STA         1
#define IEEE80211_M_IBSS        2
#define IEEE80211_M_HOSTAP      3
#define IEEE80211_M_MONITOR     4

#define IEEE80211_ADDR_LEN  6       /* size of 802.11 address */

struct ieee80211_frame {
    uint8_t     i_fc[2];
    uint8_t     i_dur[2];
    uint8_t     i_addr1[IEEE80211_ADDR_LEN];
    uint8_t     i_addr2[IEEE80211_ADDR_LEN];
    uint8_t     i_addr3[IEEE80211_ADDR_LEN];
    uint8_t     i_seq[2];
    /* possibly followed by addr4[IEEE80211_ADDR_LEN]; */
    /* see below */
} __packed;



#define ieee80211_find_rxnode(ic, wh) NULL;
unsigned int ieee80211_hdrsize(const void *data);
void ieee80211_input_all(struct mbuf *, int rssi, int nf);

#endif
