/*
 * Copyright (C) Internet Systems Consortium, Inc. ("ISC")
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* $Id: callbacks.h,v 1.2 2020/02/12 13:05:03 jsg Exp $ */

#ifndef DNS_CALLBACKS_H
#define DNS_CALLBACKS_H 1

/*! \file dns/callbacks.h */

/***
 ***	Imports
 ***/

#include <isc/lang.h>
#include <isc/magic.h>

#include <dns/types.h>

ISC_LANG_BEGINDECLS

/***
 ***	Types
 ***/

#define DNS_CALLBACK_MAGIC	ISC_MAGIC('C','L','L','B')
#define DNS_CALLBACK_VALID(cb)	ISC_MAGIC_VALID(cb, DNS_CALLBACK_MAGIC)

struct dns_rdatacallbacks {
	unsigned int magic;

	/*%
	 * dns_load_master calls this when it has rdatasets to commit.
	 */
	dns_addrdatasetfunc_t add;

	/*%
	 * This is called when reading in a database image from a 'map'
	 * format zone file.
	 */
	dns_deserializefunc_t deserialize;

	/*%
	 * dns_master_load*() call this when loading a raw zonefile,
	 * to pass back information obtained from the file header
	 */
	dns_rawdatafunc_t rawdata;
	dns_zone_t *zone;

	/*%
	 * dns_load_master / dns_rdata_fromtext call this to issue a error.
	 */
	void	(*error)(struct dns_rdatacallbacks *, const char *, ...);
	/*%
	 * dns_load_master / dns_rdata_fromtext call this to issue a warning.
	 */
	void	(*warn)(struct dns_rdatacallbacks *, const char *, ...);
	/*%
	 * Private data handles for use by the above callback functions.
	 */
	void	*add_private;
	void	*deserialize_private;
	void	*error_private;
	void	*warn_private;
};

ISC_LANG_ENDDECLS

#endif /* DNS_CALLBACKS_H */
