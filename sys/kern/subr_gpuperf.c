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

#include <sys/gpuperf.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/sysctl.h>

#define GPUPERF_DEBUG
#ifdef GPUPERF_DEBUG
#define DPRINTF(x...)	do { printf(x); } while(0)
#else
#define DPRINTF(x...)
#endif /* GPUPERF_DEBUG */

struct gpuperf_node {
	char name[16];			/* gpu driver name */
	int (*callback)(int, void *);	/* must understand 0 - 100 as % */
	void *arg;			/* arg passthrough */
};

struct gpuperf_node gpuperf_registered_nodes[GPUPERF_MAX_NODES];

int gpuperf = 100;
int gpuperf_gpun = 0;

int
gpuperf_register(const char *name, int (*callback)(int, void *), void *arg)
{
	if (gpuperf_gpun >= GPUPERF_MAX_NODES)
		return (-1);

	strlcpy(gpuperf_registered_nodes[gpuperf_gpun].name, name,
	    sizeof(gpuperf_registered_nodes[gpuperf_gpun].name));
	gpuperf_registered_nodes[gpuperf_gpun].callback = callback;
	gpuperf_registered_nodes[gpuperf_gpun].arg = arg;

	/*
	 * Drivers may take a while to register, even after /etc/sysctl.conf
	 * was processed. So immediately call back.
	 */
	(void) callback(gpuperf, arg);

	gpuperf_gpun += 1;
	DPRINTF("gpuperf: %s registered (total nodes %d)\n",
	    name, gpuperf_gpun);

	return (0);
}

int
sysctl_hwgpuperf(void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int i, err, newperf, dstatus;

	newperf = gpuperf;
	err = sysctl_int(oldp, oldlenp, newp, newlen, &newperf);
	if (err)
		return err;
	if (newperf > 100)
		newperf = 100;
	if (newperf < 0)
		newperf = 0;
	gpuperf = newperf;

	for (i=0; i < gpuperf_gpun; i++) {
		dstatus = gpuperf_registered_nodes[i].callback(gpuperf,
		    gpuperf_registered_nodes[i].arg);

		DPRINTF("gpuperf: requesting level %d from %s (dstatus %d)\n",
		    gpuperf, gpuperf_registered_nodes[i].name, dstatus);

	}

	return (0);
}
