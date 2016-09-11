/*	$OpenBSD: util.c,v 1.5 2016/03/24 07:03:30 mpi Exp $ */

/*
 * Copyright (c) 2015 Martin Pieuchot
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

#include "srp_compat.h"

#include <sys/socket.h>
#include <sys/domain.h>
#include <sys/queue.h>
#include <sys/srp.h>

#include <net/rtable.h>
#include <net/route.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <assert.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

struct sockaddr *rt_plen2mask(struct rtentry *, struct sockaddr_in6 *);

struct domain inetdomain = {
	AF_INET, "inet", NULL, NULL, NULL, NULL, NULL,
	sizeof(struct sockaddr_in), offsetof(struct sockaddr_in, sin_addr),
	32,
};

struct domain inet6domain = {
	AF_INET6, "inet6", NULL, NULL, NULL, NULL, NULL,
	sizeof(struct sockaddr_in6), offsetof(struct sockaddr_in6, sin6_addr),
	128,
};

struct domain *domains[] = { &inetdomain, &inet6domain, NULL };

/*
 * Insert a route from a string containing a destination: "192.168.1/24"
 */
void
route_insert(unsigned int rid, sa_family_t af, char *string)
{
	struct sockaddr_storage	 ss, ms;
	struct sockaddr		*ndst, *dst = (struct sockaddr *)&ss;
	struct sockaddr		*mask = (struct sockaddr *)&ms;
	struct rtentry		*rt, *nrt;
	char			 ip[INET6_ADDRSTRLEN];
	int			 plen, error;

	rt = calloc(1, sizeof(*rt));
	if (rt == NULL)
		errx(1, "out of memory");

	plen = inet_net_ptosa(af, string, dst, mask);
	if (plen == -1)
		err(1, "wrong line: %s", string);

	/* Normalize sockaddr a la rtrequest1(9) */
	ndst = malloc(dst->sa_len);
	if (ndst == NULL)
		errx(1, "out of memory");
	rt_maskedcopy(dst, ndst, mask);

	if ((error = rtable_insert(rid, ndst, mask, NULL, 0, rt)) != 0) {
		inet_net_satop(af, rt_key(rt), plen, ip, sizeof(ip));
		errx(1, "can't add route: %s, %s\n", ip, strerror(error));
	}
	nrt = rtable_lookup(rid, dst, mask, NULL, RTP_ANY);
	if (nrt != rt) {
		inet_net_satop(af, rt_key(rt), plen, ip, sizeof(ip));
		errx(1, "added route not found: %s\n", ip);
	}
}

/*
 * Delete a route from a string containing a destination: "192.168.1/24"
 */
void
route_delete(unsigned int rid, sa_family_t af, char *string)
{
	struct sockaddr_storage	 ss, ms;
	struct sockaddr		*dst = (struct sockaddr *)&ss;
	struct sockaddr		*mask = (struct sockaddr *)&ms;
	struct rtentry		*rt, *nrt;
	char			 ip[INET6_ADDRSTRLEN];
	int			 plen, error;

	plen = inet_net_ptosa(af, string, dst, mask);
	if (plen == -1)
		err(1, "wrong line: %s", string);

	rt = rtable_lookup(0, dst, mask, NULL, RTP_ANY);
	if (rt == NULL) {
		inet_net_satop(af, dst, plen, ip, sizeof(ip));
		errx(1, "can't find route: %s\n", ip);
	}

	assert(memcmp(rt_key(rt), dst, dst->sa_len) == 0);
	assert(rt_plen(rt) == rtable_satoplen(af, mask));

	if ((error = rtable_delete(0, dst, mask, rt)) != 0) {
		inet_net_satop(af, dst, plen, ip, sizeof(ip));
		errx(1, "can't rm route: %s, %s\n", ip, strerror(error));
	}

	nrt = rtable_lookup(0, dst, mask, NULL, RTP_ANY);
	if (nrt != NULL) {
		char ip0[INET6_ADDRSTRLEN];
		inet_net_satop(af, rt_key(nrt), plen, ip, sizeof(ip));
		inet_net_satop(af, rt_key(rt), plen, ip0, sizeof(ip0));
		errx(1, "found: %s after deleting: %s", ip, ip0);
	}

	free(rt_key(rt));
	free(rt);
}

/*
 * Lookup a route from a string containing a destination: "192.168.1/24"
 */
void
route_lookup(unsigned int rid, sa_family_t af, char *string)
{
	struct sockaddr_storage	 ss, ms;
	struct sockaddr		*dst = (struct sockaddr *)&ss;
	struct sockaddr		*mask = (struct sockaddr *)&ms;
	struct rtentry		*rt;
	char			 ip[INET6_ADDRSTRLEN];
	int			 plen;

	plen = inet_net_ptosa(af, string, dst, mask);
	if (plen == -1)
		err(1, "wrong line: %s", string);

	rt = rtable_lookup(0, dst, mask, NULL, RTP_ANY);
	if (rt == NULL) {
		inet_net_satop(af, dst, plen, ip, sizeof(ip));
		errx(1, "%s not found\n", ip);
	}
	assert(memcmp(rt_key(rt), dst, dst->sa_len) == 0);
	assert(rt_plen(rt) == rtable_satoplen(af, mask));
}

int
do_from_file(unsigned int rid, sa_family_t af, char *filename,
    void (*func)(unsigned int, sa_family_t, char *))
{
	FILE			*fp;
	char			*buf;
	size_t			 len;
	int			 lines = 0;

	if ((fp = fopen(filename, "r")) == NULL)
		errx(1, "No such file: %s\n", filename);

	while ((buf = fgetln(fp, &len)) != NULL) {
		if (buf[len - 1] == '\n')
			buf[len - 1] = '\0';

		(*func)(rid, af, buf);
		lines++;
	}
	fclose(fp);

	return (lines);
}

int
rtentry_dump(struct rtentry *rt, void *w, unsigned int rid)
{
	char			 dest[INET6_ADDRSTRLEN];
	sa_family_t		 af = rt_key(rt)->sa_family;

	inet_net_satop(af, rt_key(rt), rt_plen(rt), dest, sizeof(dest));
	printf("%s\n", dest);

	return (0);
}

int
rtentry_delete(struct rtentry *rt, void *w, unsigned int rid)
{
	char			 dest[INET6_ADDRSTRLEN];
	sa_family_t		 af = rt_key(rt)->sa_family;
	struct sockaddr_in6	 sa_mask;
	struct sockaddr		*mask = rt_plen2mask(rt, &sa_mask);
	int			 error;

	assert(rt_plen(rt) == rtable_satoplen(af, mask));

	if ((error = rtable_delete(0, rt_key(rt), mask, rt)) != 0) {
		inet_net_satop(af, rt_key(rt), rt_plen(rt), dest, sizeof(dest));
		errx(1, "can't rm route: %s, %s\n", dest, strerror(error));
	}

	return (0);
}

void
rt_maskedcopy(struct sockaddr *src, struct sockaddr *dst,
    struct sockaddr *netmask)
{
	uint8_t	*cp1 = (uint8_t *)src;
	uint8_t	*cp2 = (uint8_t *)dst;
	uint8_t	*cp3 = (uint8_t *)netmask;
	uint8_t	*cplim = cp2 + *cp3;
	uint8_t	*cplim2 = cp2 + *cp1;

	*cp2++ = *cp1++; *cp2++ = *cp1++; /* copies sa_len & sa_family */
	cp3 += 2;
	if (cplim > cplim2)
		cplim = cplim2;
	while (cp2 < cplim)
		*cp2++ = *cp1++ & *cp3++;
	if (cp2 < cplim2)
		memset(cp2, 0, (unsigned int)(cplim2 - cp2));
}

void
in_prefixlen2mask(struct in_addr *maskp, int plen)
{
	if (plen == 0)
		maskp->s_addr = 0;
	else
		maskp->s_addr = htonl(0xffffffff << (32 - plen));
}

void
in6_prefixlen2mask(struct in6_addr *maskp, int len)
{
	uint8_t maskarray[8] = {0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff};
	int bytelen, bitlen, i;

	assert(0 <= len && len <= 128);

	memset(maskp, 0, sizeof(*maskp));
	bytelen = len / 8;
	bitlen = len % 8;
	for (i = 0; i < bytelen; i++)
		maskp->s6_addr[i] = 0xff;
	/* len == 128 is ok because bitlen == 0 then */
	if (bitlen)
		maskp->s6_addr[bytelen] = maskarray[bitlen - 1];
}

struct sockaddr *
rt_plentosa(sa_family_t af, int plen, struct sockaddr_in6 *sa_mask)
{
	struct sockaddr_in	*sin = (struct sockaddr_in *)sa_mask;
	struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *)sa_mask;

	assert(plen >= 0 || plen == -1);

	if (plen == -1)
		return (NULL);

	memset(sa_mask, 0, sizeof(*sa_mask));

	switch (af) {
	case AF_INET:
		sin->sin_family = AF_INET;
		sin->sin_len = sizeof(struct sockaddr_in);
		in_prefixlen2mask(&sin->sin_addr, plen);
		break;
	case AF_INET6:
		sin6->sin6_family = AF_INET6;
		sin6->sin6_len = sizeof(struct sockaddr_in6);
		in6_prefixlen2mask(&sin6->sin6_addr, plen);
		break;
	default:
		return (NULL);
	}

	return ((struct sockaddr *)sa_mask);
}

struct sockaddr *
rt_plen2mask(struct rtentry *rt, struct sockaddr_in6 *sa_mask)
{
#ifndef ART
	return (rt_mask(rt));
#else
	return (rt_plentosa(rt_key(rt)->sa_family, rt_plen(rt), sa_mask));
#endif /* ART */
}


int
inet_net_ptosa(sa_family_t af, const char *buf, struct sockaddr *sa,
    struct sockaddr *ma)
{
	struct sockaddr_in *sin = (struct sockaddr_in *)sa;
	struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
	int i, plen;

	switch (af) {
	case AF_INET:
		memset(sin, 0, sizeof(*sin));
		sin->sin_family = af;
		sin->sin_len = sizeof(*sin);
		plen = inet_net_pton(af, buf, &sin->sin_addr,
		    sizeof(sin->sin_addr));
		if (plen == -1 || ma == NULL)
			break;

		sin = (struct sockaddr_in *)ma;
		memset(sin, 0, sizeof(*sin));
		sin->sin_len = sizeof(*sin);
		sin->sin_family = 0;
		in_prefixlen2mask(&sin->sin_addr, plen);
		break;
	case AF_INET6:
		memset(sin6, 0, sizeof(*sin6));
		sin6->sin6_family = af;
		sin6->sin6_len = sizeof(*sin6);
		plen = inet_net_pton(af, buf, &sin6->sin6_addr,
		    sizeof(sin6->sin6_addr));
		if (plen == -1 || ma == NULL)
			break;

		sin6 = (struct sockaddr_in6 *)ma;
		memset(sin6, 0, sizeof(*sin6));
		sin6->sin6_len = sizeof(*sin6);
		sin6->sin6_family = 0;
		for (i = 0; i < plen / 8; i++)
			sin6->sin6_addr.s6_addr[i] = 0xff;
		i = plen % 8;
		if (i)
			sin6->sin6_addr.s6_addr[plen / 8] = 0xff00 >> i;
		break;
	default:
		plen = -1;
	}

	return (plen);
}

/*
 * Only compare the address fields, we cannot use memcmp(3) because
 * the radix tree abuses the first fields of the mask sockaddr for
 * a different purpose.
 */
int
maskcmp(sa_family_t af, struct sockaddr *sa1, struct sockaddr *sa2)
{
	struct sockaddr_in *sin1, *sin2;
	struct sockaddr_in6 *sin61, *sin62;
	int len;

	switch (af) {
	case AF_INET:
		sin1 = (struct sockaddr_in *)sa1;
		sin2 = (struct sockaddr_in *)sa2;
		len = sizeof(sin1->sin_addr);
		return memcmp(&sin1->sin_addr, &sin2->sin_addr, len);
	case AF_INET6:
		sin61 = (struct sockaddr_in6 *)sa1;
		sin62 = (struct sockaddr_in6 *)sa2;
		len = sizeof(sin61->sin6_addr);
		return memcmp(&sin61->sin6_addr, &sin62->sin6_addr, len);
	default:
		return (-1);
	}
}

char *
inet_net_satop(sa_family_t af, struct sockaddr *sa, int plen, char *buf,
    size_t len)
{
	struct sockaddr_in *sin = (struct sockaddr_in *)sa;
	struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;

	switch (af) {
	case AF_INET:
		return inet_net_ntop(af, &sin->sin_addr, plen, buf, len);
	case AF_INET6:
		return inet_net_ntop(af, &sin6->sin6_addr, plen, buf, len);
	default:
		return (NULL);
	}
}
