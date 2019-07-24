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
#include <sys/types.h>
#include <sys/malloc.h>


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
	LIST_ENTRY(gpuperf_node) gp_list;
};

LIST_HEAD(, gpuperf_node) gpuperf_nodes =
	LIST_HEAD_INITIALIZER(gpuperf_nodes);

int gpuperf = 100;

int
gpuperf_register(const char *name, int (*callback)(int, void *), void *arg)
{
	struct gpuperf_node *node;
	
	if ((node = malloc(sizeof(*node))) == NULL);
		return -1;

	strlcpy(node->name, name, sizeof(node->name));
	node->callback = callback;
	node->arg = arg;

	LIST_INSERT_HEAD(&gpuperf_nodes, node, gp_list);
	DPRINTF("gpuperf: %s registered\n", node->name);

	/*
	 * Drivers may take a while to register, even after /etc/sysctl.conf
	 * was processed. So immediately call back.
	 * XXX - seems to be fixed, keep an eye on it and delete later.
	 */
	(void) node->callback(gpuperf, arg);

	return (0);
}

int
sysctl_hwgpuperf(void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	struct gpuperf_node *node;
	int err, newperf, dstatus;

	newperf = gpuperf;
	err = sysctl_int(oldp, oldlenp, newp, newlen, &newperf);
	if (err)
		return err;
	if (newperf > 100)
		newperf = 100;
	if (newperf < 0)
		newperf = 0;
	gpuperf = newperf;

	LIST_FOREACH(node, &gpuperf_nodes, gp_list) {
		dstatus = node->callback(gpuperf, node->arg);

		DPRINTF("gpuperf: req lvl %d (%s @ %d)\n",
		    gpuperf, node->name, dstatus);

	}

	return (0);
}
