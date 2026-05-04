#ifndef MBUF_H
#define MBUF_H

#include <stdlib.h>
#include <stdint.h>   /* For uint8_t, uintptr_t, etc. */
#include <stddef.h>   /* For size_t (optional, but very common) */
/*
 * Constants related to network buffer management.
 * MCLBYTES must be no larger than PAGE_SIZE.
 */
#ifndef MSIZE
#define MSIZE       256     /* size of an mbuf */
#endif

#ifndef MCLSHIFT
#define MCLSHIFT    11      /* convert bytes to mbuf clusters */
#endif  /* MCLSHIFT */

#define MCLBYTES    (1 << MCLSHIFT) /* size of an mbuf cluster */


#define M_DONTWAIT      1     /* Non-blocking allocation */
#define M_WAITOK        2     /* Can sleep (not used in userspace) */

#define MT_DATA         1     /* Data mbuf */
#define MT_HEADER       2     /* Header mbuf */

/* Typical FreeBSD sizes - you can tune these */
#define MLEN            256   /* Normal mbuf data size (inline) */
#define MHLEN           (MLEN - sizeof(struct m_pkthdr))  /* Room for pkthdr */

/* ======================  Macros  ====================== */

/* MGETHDR: Allocate mbuf with packet header */
#define MGETHDR(m, how, type) do { \
    (m) = malloc(sizeof(struct mbuf)); \
    if (m != NULL) { \
        memset((m), 0, sizeof(struct mbuf)); \
        (m)->m_flags = M_PKTHDR; \
        (m)->m_type = (type); \
        (m)->m_data = (m)->m_dat.m_databuf; \
        (m)->m_next = (m)->m_nextpkt = NULL; \
        (m)->m_len = 0; \
        (m)->m_pkthdr.len = 0; \
    } \
} while (0)

/* MCLGET: Attach a cluster (larger external buffer) */
#define MCLGET(m, how) do { \
    if ((m) != NULL) { \
        (m)->m_dat.m_ext.ext_buf = malloc(2048); /* Typical cluster size */ \
        if ((m)->m_dat.m_ext.ext_buf != NULL) { \
            (m)->m_flags |= M_EXT; \
            (m)->m_data = (m)->m_dat.m_ext.ext_buf; \
            (m)->m_dat.m_ext.ext_size = 2048; \
        } \
    } \
} while (0)

/* Minimal flags (you can expand this) */
#define M_PKTHDR    0x0001   /* Packet header present */
#define M_EXT       0x0002   /* External buffer (cluster) */
#define M_EOR       0x0004   /* End of record */

/* ======================  Packet Header  ====================== */

struct m_pkthdr {
    int      len;           /* Total packet length (all mbufs in chain) */
    uint32_t rcvif;         /* Receive interface (opaque pointer/index) */

    /* Common fields used by WiFi / 802.11 drivers */
    uint32_t csum_flags;    /* Checksum offload flags */
    uint32_t csum_data;     /* Checksum data / result */

    /* Timestamp (optional but often used) */
    uint64_t rcv_tstamp;    /* Receive timestamp (nanoseconds or ticks) */

    /* Flow / tagging */
    uint32_t flowid;        /* Flow identifier */
    uint8_t  flowtype;

    /* Commonly accessed in ieee80211_input path */
    uint16_t vlan_tag;      /* VLAN tag if present */
    uint16_t ph_flags;      /* Packet header flags */

    /* Room for more fields as needed */
};

/* m_pkthdr.ph_flags bits */
#define M_VLANTAG       0x0001  /* VLAN tag is valid */
#define M_FLOWID        0x0002  /* flowid is valid */

/* ======================  mbuf struct updated  ====================== */

struct mbuf {
    struct mbuf     *m_next;        /* Next mbuf in chain */
    struct mbuf     *m_nextpkt;     /* Next packet */

    uint8_t         *m_data;        /* Current data pointer */
    int              m_len;         /* Length of data in this mbuf */
    int              m_flags;
    int              m_type;

    struct m_pkthdr  m_pkthdr;      /* Packet header (valid if M_PKTHDR set) */

    union {
        uint8_t      m_databuf[MLEN];
        struct {
            uint8_t *ext_buf;
            size_t   ext_size;
        } m_ext;
    } m_dat;
};

#define mtod(m, t)      ((t)((m)->m_data))

void m_free(struct mbuf *m);
void m_freem(struct mbuf *m);
void m_adj(struct mbuf *m, int len);

#endif