#include "mbuf.h"
#include <stdlib.h>

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