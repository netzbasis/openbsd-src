/*	$OpenBSD: envelope.c,v 1.45 2019/08/14 21:11:25 gilles Exp $	*/

/*
 * Copyright (c) 2013 Eric Faurot <eric@openbsd.org>
 * Copyright (c) 2011-2013 Gilles Chehade <gilles@poolp.org>
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

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/tree.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <event.h>
#include <fcntl.h>
#include <imsg.h>
#include <inttypes.h>
#include <pwd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "smtpd.h"
#include "log.h"

static int envelope_ascii_load(struct envelope *, struct dict *);
static void envelope_ascii_dump(const struct envelope *, char **, size_t *,
    const char *);

void
envelope_set_errormsg(struct envelope *e, char *fmt, ...)
{
	int ret;
	va_list ap;

	va_start(ap, fmt);
	ret = vsnprintf(e->errorline, sizeof(e->errorline), fmt, ap);
	va_end(ap);

	/* this should not happen */
	if (ret < 0)
		err(1, "vsnprintf");

	if ((size_t)ret >= sizeof(e->errorline))
		(void)strlcpy(e->errorline + (sizeof(e->errorline) - 4),
		    "...", 4);
}

void
envelope_set_esc_class(struct envelope *e, enum enhanced_status_class class)
{
	e->esc_class = class;
}

void
envelope_set_esc_code(struct envelope *e, enum enhanced_status_code code)
{
	e->esc_code = code;
}

static int
envelope_buffer_to_dict(struct dict *d,  const char *ibuf, size_t buflen)
{
	static char	 lbuf[sizeof(struct envelope)];
	size_t		 len;
	char		*buf, *field, *nextline;

	memset(lbuf, 0, sizeof lbuf);
	if (strlcpy(lbuf, ibuf, sizeof lbuf) >= sizeof lbuf)
		goto err;
	buf = lbuf;

	while (buflen > 0) {
		len = strcspn(buf, "\n");
		buf[len] = '\0';
		nextline = buf + len + 1;
		buflen -= (nextline - buf);

		field = buf;
		while (*buf && (isalnum((unsigned char)*buf) || *buf == '-'))
			buf++;
		if (!*buf)
			goto err;

		/* skip whitespaces before separator */
		while (*buf && isspace((unsigned char)*buf))
			*buf++ = 0;

		/* we *want* ':' */
		if (*buf != ':')
			goto err;
		*buf++ = 0;

		/* skip whitespaces after separator */
		while (*buf && isspace((unsigned char)*buf))
			*buf++ = 0;
		dict_set(d, field, buf);
		buf = nextline;
	}

	return (1);

err:
	return (0);
}

int
envelope_load_buffer(struct envelope *ep, const char *ibuf, size_t buflen)
{
	struct dict	 d;
	const char	*val, *errstr;
	long long	 version;
	int		 ret = 0;

	dict_init(&d);
	if (!envelope_buffer_to_dict(&d, ibuf, buflen)) {
		log_debug("debug: cannot parse envelope to dict");
		goto end;
	}

	val = dict_get(&d, "version");
	if (val == NULL) {
		log_debug("debug: envelope version not found");
		goto end;
	}
	version = strtonum(val, 1, 64, &errstr);
	if (errstr) {
		log_debug("debug: cannot parse envelope version: %s", val);
		goto end;
	}

	if (version != SMTPD_ENVELOPE_VERSION) {
		log_debug("debug: bad envelope version %lld", version);
		goto end;
	}

	memset(ep, 0, sizeof *ep);
	ret = envelope_ascii_load(ep, &d);
	if (ret)
		ep->version = SMTPD_ENVELOPE_VERSION;
end:
	while (dict_poproot(&d, NULL))
		;
	return (ret);
}

int
envelope_dump_buffer(const struct envelope *ep, char *dest, size_t len)
{
	char	*p = dest;

	envelope_ascii_dump(ep, &dest, &len, "version");
	envelope_ascii_dump(ep, &dest, &len, "dispatcher");
	envelope_ascii_dump(ep, &dest, &len, "tag");
	envelope_ascii_dump(ep, &dest, &len, "type");
	envelope_ascii_dump(ep, &dest, &len, "smtpname");
	envelope_ascii_dump(ep, &dest, &len, "helo");
	envelope_ascii_dump(ep, &dest, &len, "hostname");
	envelope_ascii_dump(ep, &dest, &len, "errorline");
	envelope_ascii_dump(ep, &dest, &len, "sockaddr");
	envelope_ascii_dump(ep, &dest, &len, "sender");
	envelope_ascii_dump(ep, &dest, &len, "rcpt");
	envelope_ascii_dump(ep, &dest, &len, "dest");
	envelope_ascii_dump(ep, &dest, &len, "ctime");
	envelope_ascii_dump(ep, &dest, &len, "last-try");
	envelope_ascii_dump(ep, &dest, &len, "last-bounce");
	envelope_ascii_dump(ep, &dest, &len, "ttl");
	envelope_ascii_dump(ep, &dest, &len, "retry");
	envelope_ascii_dump(ep, &dest, &len, "flags");
	envelope_ascii_dump(ep, &dest, &len, "dsn-notify");
	envelope_ascii_dump(ep, &dest, &len, "dsn-ret");
	envelope_ascii_dump(ep, &dest, &len, "dsn-envid");
	envelope_ascii_dump(ep, &dest, &len, "dsn-orcpt");
	envelope_ascii_dump(ep, &dest, &len, "esc-class");
	envelope_ascii_dump(ep, &dest, &len, "esc-code");

	switch (ep->type) {
	case D_MDA:
		envelope_ascii_dump(ep, &dest, &len, "mda-exec");
		envelope_ascii_dump(ep, &dest, &len, "mda-subaddress");
		envelope_ascii_dump(ep, &dest, &len, "mda-user");
		break;
	case D_MTA:
		break;
	case D_BOUNCE:
		envelope_ascii_dump(ep, &dest, &len, "bounce-ttl");
		envelope_ascii_dump(ep, &dest, &len, "bounce-delay");
		envelope_ascii_dump(ep, &dest, &len, "bounce-type");
		break;
	default:
		return (0);
	}

	if (dest == NULL)
		return (0);

	return (dest - p);
}

static int
ascii_load_uint8(uint8_t *dest, char *buf)
{
	const char *errstr;

	*dest = strtonum(buf, 0, 0xff, &errstr);
	if (errstr)
		return 0;
	return 1;
}

static int
ascii_load_uint16(uint16_t *dest, char *buf)
{
	const char *errstr;

	*dest = strtonum(buf, 0, 0xffff, &errstr);
	if (errstr)
		return 0;
	return 1;
}

static int
ascii_load_uint32(uint32_t *dest, char *buf)
{
	const char *errstr;

	*dest = strtonum(buf, 0, 0xffffffff, &errstr);
	if (errstr)
		return 0;
	return 1;
}

static int
ascii_load_time(time_t *dest, char *buf)
{
	const char *errstr;

	*dest = strtonum(buf, 0, LLONG_MAX, &errstr);
	if (errstr)
		return 0;
	return 1;
}

static int
ascii_load_type(enum delivery_type *dest, char *buf)
{
	if (strcasecmp(buf, "mda") == 0)
		*dest = D_MDA;
	else if (strcasecmp(buf, "mta") == 0)
		*dest = D_MTA;
	else if (strcasecmp(buf, "bounce") == 0)
		*dest = D_BOUNCE;
	else
		return 0;
	return 1;
}

static int
ascii_load_string(char *dest, char *buf, size_t len)
{
	if (strlcpy(dest, buf, len) >= len)
		return 0;
	return 1;
}

static int
ascii_load_sockaddr(struct sockaddr_storage *ss, char *buf)
{
	struct sockaddr_in6 ssin6;
	struct sockaddr_in  ssin;

	memset(&ssin, 0, sizeof ssin);
	memset(&ssin6, 0, sizeof ssin6);

	if (!strcmp("local", buf)) {
		ss->ss_family = AF_LOCAL;
	}
	else if (strncasecmp("IPv6:", buf, 5) == 0) {
		if (inet_pton(AF_INET6, buf + 5, &ssin6.sin6_addr) != 1)
			return 0;
		ssin6.sin6_family = AF_INET6;
		memcpy(ss, &ssin6, sizeof(ssin6));
		ss->ss_len = sizeof(struct sockaddr_in6);
	}
	else {
		if (inet_pton(AF_INET, buf, &ssin.sin_addr) != 1)
			return 0;
		ssin.sin_family = AF_INET;
		memcpy(ss, &ssin, sizeof(ssin));
		ss->ss_len = sizeof(struct sockaddr_in);
	}
	return 1;
}

static int
ascii_load_mailaddr(struct mailaddr *dest, char *buf)
{
	if (!text_to_mailaddr(dest, buf))
		return 0;
	return 1;
}

static int
ascii_load_flags(enum envelope_flags *dest, char *buf)
{
	char *flag;

	while ((flag = strsep(&buf, " ,|")) != NULL) {
		if (strcasecmp(flag, "authenticated") == 0)
			*dest |= EF_AUTHENTICATED;
		else if (strcasecmp(flag, "enqueued") == 0)
			;
		else if (strcasecmp(flag, "bounce") == 0)
			*dest |= EF_BOUNCE;
		else if (strcasecmp(flag, "internal") == 0)
			*dest |= EF_INTERNAL;
		else
			return 0;
	}
	return 1;
}

static int
ascii_load_bounce_type(enum bounce_type *dest, char *buf)
{
	if (strcasecmp(buf, "error") == 0 || strcasecmp(buf, "failed") == 0)
		*dest = B_FAILED;
	else if (strcasecmp(buf, "warn") == 0 ||
	    strcasecmp(buf, "delayed") == 0)
		*dest = B_DELAYED;
	else if (strcasecmp(buf, "dsn") == 0 ||
	    strcasecmp(buf, "delivered") == 0)
		*dest = B_DELIVERED;
	else
		return 0;
	return 1;
}

static int
ascii_load_dsn_ret(enum dsn_ret *ret, char *buf)
{
	if (strcasecmp(buf, "HDRS") == 0)
		*ret = DSN_RETHDRS;
	else if (strcasecmp(buf, "FULL") == 0)
		*ret = DSN_RETFULL;
	else
		return 0;
	return 1;
}

static int
ascii_load_field(const char *field, struct envelope *ep, char *buf)
{
	if (strcasecmp("dispatcher", field) == 0)
		return ascii_load_string(ep->dispatcher, buf,
		    sizeof ep->dispatcher);

	if (strcasecmp("bounce-delay", field) == 0)
		return ascii_load_time(&ep->agent.bounce.delay, buf);

	if (strcasecmp("bounce-ttl", field) == 0)
		return ascii_load_time(&ep->agent.bounce.ttl, buf);

	if (strcasecmp("bounce-type", field) == 0)
		return ascii_load_bounce_type(&ep->agent.bounce.type, buf);

	if (strcasecmp("ctime", field) == 0)
		return ascii_load_time(&ep->creation, buf);

	if (strcasecmp("dest", field) == 0)
		return ascii_load_mailaddr(&ep->dest, buf);

	if (strcasecmp("errorline", field) == 0)
		return ascii_load_string(ep->errorline, buf,
		    sizeof ep->errorline);

	if (strcasecmp("ttl", field) == 0)
		return ascii_load_time(&ep->ttl, buf);

	if (strcasecmp("flags", field) == 0)
		return ascii_load_flags(&ep->flags, buf);

	if (strcasecmp("helo", field) == 0)
		return ascii_load_string(ep->helo, buf, sizeof ep->helo);

	if (strcasecmp("hostname", field) == 0)
		return ascii_load_string(ep->hostname, buf,
		    sizeof ep->hostname);

	if (strcasecmp("last-bounce", field) == 0)
		return ascii_load_time(&ep->lastbounce, buf);

	if (strcasecmp("last-try", field) == 0)
		return ascii_load_time(&ep->lasttry, buf);

	if (strcasecmp("retry", field) == 0)
		return ascii_load_uint16(&ep->retry, buf);

	if (strcasecmp("rcpt", field) == 0)
		return ascii_load_mailaddr(&ep->rcpt, buf);

	if (strcasecmp("mda-exec", field) == 0)
		return ascii_load_string(ep->mda_exec, buf, sizeof(ep->mda_exec));

	if (strcasecmp("mda-subaddress", field) == 0)
		return ascii_load_string(ep->mda_subaddress, buf, sizeof(ep->mda_subaddress));

	if (strcasecmp("mda-user", field) == 0)
		return ascii_load_string(ep->mda_user, buf, sizeof(ep->mda_user));

	if (strcasecmp("sender", field) == 0)
		return ascii_load_mailaddr(&ep->sender, buf);

	if (strcasecmp("smtpname", field) == 0)
		return ascii_load_string(ep->smtpname, buf,
		    sizeof(ep->smtpname));

	if (strcasecmp("sockaddr", field) == 0)
		return ascii_load_sockaddr(&ep->ss, buf);

	if (strcasecmp("tag", field) == 0)
		return ascii_load_string(ep->tag, buf, sizeof ep->tag);

	if (strcasecmp("type", field) == 0)
		return ascii_load_type(&ep->type, buf);

	if (strcasecmp("version", field) == 0)
		return ascii_load_uint32(&ep->version, buf);

	if (strcasecmp("dsn-notify", field) == 0)
		return ascii_load_uint8(&ep->dsn_notify, buf);

	if (strcasecmp("dsn-orcpt", field) == 0)
		return ascii_load_mailaddr(&ep->dsn_orcpt, buf);

	if (strcasecmp("dsn-ret", field) == 0)
		return ascii_load_dsn_ret(&ep->dsn_ret, buf);

	if (strcasecmp("dsn-envid", field) == 0)
		return ascii_load_string(ep->dsn_envid, buf,
		    sizeof(ep->dsn_envid));

	if (strcasecmp("esc-class", field) == 0)
		return ascii_load_uint8(&ep->esc_class, buf);

	if (strcasecmp("esc-code", field) == 0)
		return ascii_load_uint8(&ep->esc_code, buf);

	return (0);
}

static int
envelope_ascii_load(struct envelope *ep, struct dict *d)
{
	const char	       *field;
	char		       *value;
	void		       *hdl;

	hdl = NULL;
	while (dict_iter(d, &hdl, &field, (void **)&value))
		if (!ascii_load_field(field, ep, value))
			goto err;

	return (1);

err:
	log_warnx("envelope: invalid field \"%s\"", field);
	return (0);
}


static int
ascii_dump_uint8(uint8_t src, char *dest, size_t len)
{
	return bsnprintf(dest, len, "%d", src);
}

static int
ascii_dump_uint16(uint16_t src, char *dest, size_t len)
{
	return bsnprintf(dest, len, "%d", src);
}

static int
ascii_dump_uint32(uint32_t src, char *dest, size_t len)
{
	return bsnprintf(dest, len, "%d", src);
}

static int
ascii_dump_time(time_t src, char *dest, size_t len)
{
	return bsnprintf(dest, len, "%lld", (long long) src);
}

static int
ascii_dump_string(const char *src, char *dest, size_t len)
{
	return bsnprintf(dest, len, "%s", src);
}

static int
ascii_dump_type(enum delivery_type type, char *dest, size_t len)
{
	char *p = NULL;

	switch (type) {
	case D_MDA:
		p = "mda";
		break;
	case D_MTA:
		p = "mta";
		break;
	case D_BOUNCE:
		p = "bounce";
		break;
	default:
		return 0;
	}

	return bsnprintf(dest, len, "%s", p);
}

static int
ascii_dump_mailaddr(const struct mailaddr *addr, char *dest, size_t len)
{
	return bsnprintf(dest, len, "%s@%s",
	    addr->user, addr->domain);
}

static int
ascii_dump_flags(enum envelope_flags flags, char *buf, size_t len)
{
	size_t cpylen = 0;

	buf[0] = '\0';
	if (flags) {
		if (flags & EF_AUTHENTICATED)
			cpylen = strlcat(buf, "authenticated", len);
		if (flags & EF_BOUNCE) {
			if (buf[0] != '\0')
				(void)strlcat(buf, " ", len);
			cpylen = strlcat(buf, "bounce", len);
		}
		if (flags & EF_INTERNAL) {
			if (buf[0] != '\0')
				(void)strlcat(buf, " ", len);
			cpylen = strlcat(buf, "internal", len);
		}
	}

	return cpylen < len ? 1 : 0;
}

static int
ascii_dump_bounce_type(enum bounce_type type, char *dest, size_t len)
{
	char *p = NULL;

	switch (type) {
	case B_FAILED:
		p = "failed";
		break;
	case B_DELAYED:
		p = "delayed";
		break;
	case B_DELIVERED:
		p = "delivered";
		break;
	default:
		return 0;
	}
	return bsnprintf(dest, len, "%s", p);
}


static int
ascii_dump_dsn_ret(enum dsn_ret flag, char *dest, size_t len)
{
	size_t cpylen = 0;

	dest[0] = '\0';
	if (flag == DSN_RETFULL)
		cpylen = strlcat(dest, "FULL", len);
	else if (flag == DSN_RETHDRS)
		cpylen = strlcat(dest, "HDRS", len);

	return cpylen < len ? 1 : 0;
}

static int
ascii_dump_field(const char *field, const struct envelope *ep,
    char *buf, size_t len)
{
	if (strcasecmp(field, "dispatcher") == 0)
		return ascii_dump_string(ep->dispatcher, buf, len);

	if (strcasecmp(field, "bounce-delay") == 0) {
		if (ep->agent.bounce.type != B_DELAYED)
			return (1);
		return ascii_dump_time(ep->agent.bounce.delay, buf, len);
	}

	if (strcasecmp(field, "bounce-ttl") == 0) {
		if (ep->agent.bounce.type != B_DELAYED)
			return (1);
		return ascii_dump_time(ep->agent.bounce.ttl, buf, len);
	}

	if (strcasecmp(field, "bounce-type") == 0)
		return ascii_dump_bounce_type(ep->agent.bounce.type, buf, len);

	if (strcasecmp(field, "ctime") == 0)
		return ascii_dump_time(ep->creation, buf, len);

	if (strcasecmp(field, "dest") == 0)
		return ascii_dump_mailaddr(&ep->dest, buf, len);

	if (strcasecmp(field, "errorline") == 0)
		return ascii_dump_string(ep->errorline, buf, len);

	if (strcasecmp(field, "ttl") == 0)
		return ascii_dump_time(ep->ttl, buf, len);

	if (strcasecmp(field, "flags") == 0)
		return ascii_dump_flags(ep->flags, buf, len);

	if (strcasecmp(field, "helo") == 0)
		return ascii_dump_string(ep->helo, buf, len);

	if (strcasecmp(field, "hostname") == 0)
		return ascii_dump_string(ep->hostname, buf, len);

	if (strcasecmp(field, "last-bounce") == 0)
		return ascii_dump_time(ep->lastbounce, buf, len);

	if (strcasecmp(field, "last-try") == 0)
		return ascii_dump_time(ep->lasttry, buf, len);

	if (strcasecmp(field, "retry") == 0)
		return ascii_dump_uint16(ep->retry, buf, len);

	if (strcasecmp(field, "rcpt") == 0)
		return ascii_dump_mailaddr(&ep->rcpt, buf, len);

	if (strcasecmp(field, "mda-exec") == 0) {
		if (ep->mda_exec[0])
			return ascii_dump_string(ep->mda_exec, buf, len);
		return 1;
	}

	if (strcasecmp(field, "mda-subaddress") == 0) {
		if (ep->mda_subaddress[0])
			return ascii_dump_string(ep->mda_subaddress, buf, len);
		return 1;
	}

	if (strcasecmp(field, "mda-user") == 0) {
		if (ep->mda_user[0])
			return ascii_dump_string(ep->mda_user, buf, len);
		return 1;
	}

	if (strcasecmp(field, "sender") == 0)
		return ascii_dump_mailaddr(&ep->sender, buf, len);

	if (strcasecmp(field, "smtpname") == 0)
		return ascii_dump_string(ep->smtpname, buf, len);

	if (strcasecmp(field, "sockaddr") == 0)
		return ascii_dump_string(ss_to_text(&ep->ss), buf, len);

	if (strcasecmp(field, "tag") == 0)
		return ascii_dump_string(ep->tag, buf, len);

	if (strcasecmp(field, "type") == 0)
		return ascii_dump_type(ep->type, buf, len);

	if (strcasecmp(field, "version") == 0)
		return ascii_dump_uint32(SMTPD_ENVELOPE_VERSION, buf, len);

	if (strcasecmp(field, "dsn-notify") == 0)
		return ascii_dump_uint8(ep->dsn_notify, buf, len);

	if (strcasecmp(field, "dsn-ret") == 0)
		return ascii_dump_dsn_ret(ep->dsn_ret, buf, len);

	if (strcasecmp(field, "dsn-orcpt") == 0) {
		if (ep->dsn_orcpt.user[0] && ep->dsn_orcpt.domain[0])
			return ascii_dump_mailaddr(&ep->dsn_orcpt, buf, len);
		return 1;
	}

	if (strcasecmp(field, "dsn-envid") == 0)
		return ascii_dump_string(ep->dsn_envid, buf, len);

	if (strcasecmp(field, "esc-class") == 0) {
		if (ep->esc_class)
			return ascii_dump_uint8(ep->esc_class, buf, len);
		return 1;
	}

	if (strcasecmp(field, "esc-code") == 0) {
		/* this is not a pasto, we dump esc_code if esc_class is !0 */
		if (ep->esc_class)
			return ascii_dump_uint8(ep->esc_code, buf, len);
		return 1;
	}

	return (0);
}

static void
envelope_ascii_dump(const struct envelope *ep, char **dest, size_t *len,
    const char *field)
{
	char	buf[8192];
	int	l;

	if (*dest == NULL)
		return;

	memset(buf, 0, sizeof buf);
	if (!ascii_dump_field(field, ep, buf, sizeof buf))
		goto err;
	if (buf[0] == '\0')
		return;

	l = snprintf(*dest, *len, "%s: %s\n", field, buf);
	if (l < 0 || (size_t) l >= *len)
		goto err;
	*dest += l;
	*len -= l;

	return;
err:
	*dest = NULL;
}
