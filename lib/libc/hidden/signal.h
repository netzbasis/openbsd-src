/*	$OpenBSD: signal.h,v 1.4 2015/09/11 15:38:33 guenther Exp $	*/
/*
 * Copyright (c) 2015 Philip Guenther <guenther@openbsd.org>
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

#ifndef _LIBC_SIGNAL_H
#define _LIBC_SIGNAL_H

#include_next <signal.h>

__BEGIN_HIDDEN_DECLS
extern sigset_t __sigintr;
__END_HIDDEN_DECLS

extern PROTO_NORMAL(sys_siglist);
extern PROTO_NORMAL(sys_signame);

PROTO_NORMAL(signal);

PROTO_NORMAL(kill);             /* wrap to ban SIGTHR? */
PROTO_NORMAL(sigaltstack);
PROTO_NORMAL(sigpending);
PROTO_NORMAL(sigreturn);

#endif	/* !_LIBC_SIGNAL_H */
