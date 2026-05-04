#include "mbuf.h"
#include <stdlib.h>
#include <string.h>

/*
 * Free a single mbuf (and its external cluster if present).
 * Does NOT free the rest of the chain.
 */
void m_free(struct mbuf *m)
{
    if (m == NULL)
        return;

    /* Free external cluster/buffer if present */
    if (m->m_flags & M_EXT) {
        if (m->m_dat.m_ext.ext_buf) {
            free(m->m_dat.m_ext.ext_buf);
        }
    }

    /* Zero before freeing to catch use-after-free bugs in tests */
    memset(m, 0xCC, sizeof(struct mbuf));  /* Optional but helpful for debugging */
    free(m);
}

/*
 * Free an entire mbuf chain (most commonly used).
 * This is the equivalent of the kernel's m_freem().
 */
void m_freem(struct mbuf *m)
{
    struct mbuf *next;

    while (m != NULL) {
        next = m->m_next;
        m_free(m);
        m = next;
    }
}

void m_adj(struct mbuf *m, int len)
{
    struct mbuf *n;
    int remaining;

    if (m == NULL || len == 0)
        return;

    /* === Trim from head === */
    if (len > 0) {
        remaining = len;

        while (m != NULL && remaining > 0) {
            if (remaining >= m->m_len) {
                /* Drop entire mbuf */
                remaining -= m->m_len;
                m->m_len = 0;
                m->m_data = (m->m_flags & M_EXT) ?
                            m->m_dat.m_ext.ext_buf : m->m_dat.m_databuf;
                m = m->m_next;
            } else {
                /* Partial trim */
                m->m_data += remaining;
                m->m_len -= remaining;
                remaining = 0;
            }
        }

        /* Update total packet length in pkthdr */
        if (m != NULL && (m->m_flags & M_PKTHDR)) {
            m->m_pkthdr.len -= len;
            if (m->m_pkthdr.len < 0)
                m->m_pkthdr.len = 0;
        }
    }
    /* === Trim from tail === */
    else {
        len = -len;           /* make positive */
        if (len <= 0)
            return;

        /* Calculate total length */
        int total = 0;
        for (n = m; n; n = n->m_next)
            total += n->m_len;

        if (len >= total) {
            /* Drop everything */
            for (n = m; n; n = n->m_next) {
                n->m_len = 0;
            }
            if (m->m_flags & M_PKTHDR)
                m->m_pkthdr.len = 0;
            return;
        }

        remaining = len;

        /* Trim from the end */
        for (n = m; n; n = n->m_next) {
            if (remaining >= n->m_len) {
                remaining -= n->m_len;
                n->m_len = 0;
            } else {
                n->m_len -= remaining;
                remaining = 0;
                break;
            }
        }

        if (m->m_flags & M_PKTHDR)
            m->m_pkthdr.len -= len;
    }
}