/*
 * Copyright (c) 2016 Todd C. Miller <Todd.Miller@courtesan.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <config.h>

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#ifdef HAVE_STRING_H
# include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif /* HAVE_STRINGS_H */

#include "sudoers.h"

/* Trivial reference-counted strings. */
struct rcstr {
    int refcnt;
    char str[1];	/* actually bigger */
};

/*
 * Allocate a reference-counted string and copy src to it.
 * Returns the newly-created string with a refcnt of 1.
 */
char *
rcstr_dup(const char *src)
{
    size_t len = strlen(src);
    char *dst;
    debug_decl(rcstr_dup, SUDOERS_DEBUG_UTIL)

    dst = rcstr_alloc(len);
    memcpy(dst, src, len);
    dst[len] = '\0';
    debug_return_ptr(dst);
}

char *
rcstr_alloc(size_t len)
{
    struct rcstr *rcs;
    debug_decl(rcstr_dup, SUDOERS_DEBUG_UTIL)

    /* Note: sizeof(struct rcstr) includes space for the NUL */
    rcs = malloc(sizeof(struct rcstr) + len);
    if (rcs == NULL)
	return NULL;

    rcs->refcnt = 1;
    rcs->str[0] = '\0';
    debug_return_ptr(rcs->str);
}

char *
rcstr_addref(const char *s)
{
    struct rcstr *rcs;
    debug_decl(rcstr_dup, SUDOERS_DEBUG_UTIL)

    if (s == NULL)
	debug_return_ptr(NULL);

    rcs = __containerof(s, struct rcstr, str);
    rcs->refcnt++;
    debug_return_ptr(rcs->str);
}

void
rcstr_delref(const char *s)
{
    struct rcstr *rcs;
    debug_decl(rcstr_dup, SUDOERS_DEBUG_UTIL)

    if (s != NULL) {
	rcs = __containerof(s, struct rcstr, str);
	if (--rcs->refcnt == 0) {
	    rcs->str[0] = '\0';
	    free(rcs);
	}
    }
    debug_return;
}
