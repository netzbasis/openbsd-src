/*	$OpenBSD: asm.h,v 1.10 2016/05/27 16:32:39 deraadt Exp $	*/
/*	$NetBSD: asm.h,v 1.15 2000/08/02 22:24:39 eeh Exp $ */

/*
 * Copyright (c) 1994 Allen Briggs
 * All rights reserved.
 *
 * Gleaned from locore.s and sun3 asm.h which had the following copyrights:
 * locore.s:
 * Copyright (c) 1988 University of Utah.
 * Copyright (c) 1982, 1990 The Regents of the University of California.
 * sun3/include/asm.h:
 * Copyright (c) 1993 Adam Glass
 * Copyright (c) 1990 The Regents of the University of California.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _MACHINE_ASM_H_
#define _MACHINE_ASM_H_

#ifndef _LOCORE
#define _LOCORE
#endif
#include <machine/frame.h>

/* Pull in CCFSZ, CC64FSZ, and BIAS from frame.h */
#ifndef _LOCORE
#define _LOCORE
#endif
#include <machine/frame.h>

#define	_C_LABEL(name)		name
#define	_ASM_LABEL(name)	name

#ifdef __PIC__
/*
 * PIC_PROLOGUE() is akin to the compiler generated function prologue for
 * PIC code. It leaves the address of the Global Offset Table in DEST,
 * clobbering register TMP in the process. Using the temporary enables us
 * to work without a stack frame (doing so requires saving %o7) .
 */
#define PIC_PROLOGUE(dest,tmp) \
	sethi %hi(_GLOBAL_OFFSET_TABLE_-4),dest; \
	rd %pc, tmp; \
	or dest,%lo(_GLOBAL_OFFSET_TABLE_+4),dest; \
	add dest,tmp,dest

/*
 * PICCY_SET() does the equivalent of a `set var, %dest' instruction in
 * a PIC-like way, but without involving the Global Offset Table. This
 * only works for VARs defined in the same file *and* in the text segment.
 */
#define PICCY_SET(var,dest,tmp) \
	3: rd %pc, tmp; add tmp,(var-3b),dest
#else
#define PIC_PROLOGUE(dest,tmp)
#define PICCY_OFFSET(var,dest,tmp)
#endif

#define FTYPE(x)		.type x,@function
#define OTYPE(x)		.type x,@object

#define	_ENTRY(name) \
	.align 4; .globl name; .proc 1; FTYPE(name); name:

#ifdef GPROF
#define _PROF_PROLOGUE \
	.data; .align 8; 1: .uaword 0; .uaword 0; \
	.text; save %sp,-CC64FSZ,%sp; sethi %hi(1b),%o0; call _mcount; \
	or %o0,%lo(1b),%o0; restore
#else
#define _PROF_PROLOGUE
#endif

#define ENTRY(name)		_ENTRY(_C_LABEL(name)); _PROF_PROLOGUE
#define NENTRY(name)		_ENTRY(_C_LABEL(name))
#define	ASENTRY(name)		_ENTRY(_ASM_LABEL(name)); _PROF_PROLOGUE
#define	FUNC(name)		ASENTRY(name)
#define	END(y)			.size y, . - y
#define RODATA(name)		.align 4; .text; .globl _C_LABEL(name); \
				OTYPE(_C_LABEL(name)); _C_LABEL(name):


#define ASMSTR			.asciz

#define RCSID(name)		.asciz name

#define	STRONG_ALIAS(alias,sym)						\
	.global alias;							\
	alias = sym
#define	WEAK_ALIAS(alias,sym)						\
	.weak alias;							\
	alias = sym

/*
 * WARN_REFERENCES: create a warning if the specified symbol is referenced.
 */
#ifdef __STDC__
#define	WARN_REFERENCES(_sym,_msg)				\
	.section .gnu.warning. ## _sym ; .ascii _msg ; .text
#else
#define	WARN_REFERENCES(_sym,_msg)				\
	.section .gnu.warning./**/_sym ; .ascii _msg ; .text
#endif /* __STDC__ */

#endif /* _MACHINE_ASM_H_ */
