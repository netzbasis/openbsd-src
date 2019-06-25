/*	$OpenBSD$	*/

/*
 * Copyright (c) 2019 Benjamin Baier <ben@netzbasis.de>
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

#ifndef _SYS_GPUPERF_H_
#define _SYS_GPUPERF_H_

#ifndef _SIZE_T_DEFINED_
#define _SIZE_T_DEFINED_
#include <sys/_types.h>
typedef __size_t        size_t;
#endif /* _SIZE_T_DEFINED_ */

#define GPUPERF_MAX_NODES 4

int sysctl_hwgpuperf(void *, size_t *, void *, size_t);
int gpuperf_register(const char *, int (*)(int, void *), void *);

#endif /* _SYS_GPUPERF_H_ */
