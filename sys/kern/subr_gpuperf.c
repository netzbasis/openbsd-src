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
	int (*callback)(int, void *);	/* called with GP_LOW, GP_AUTO, GP_HIGH */
	void *arg;			/* arg passthrough */
	LIST_ENTRY(gpuperf_node) gp_list;
};

LIST_HEAD(, gpuperf_node) gpuperf_nodes =
	LIST_HEAD_INITIALIZER(gpuperf_nodes);

int
gpuperf_register(const char *name, int (*callback)(int, void *), void *arg)
{
	struct gpuperf_node *node;

	if ((node = malloc(sizeof(*node), M_DEVBUF, M_NOWAIT)) == NULL)
		return -1;

	strlcpy(node->name, name, sizeof(node->name));
	node->callback = callback;
	node->arg = arg;

	LIST_INSERT_HEAD(&gpuperf_nodes, node, gp_list);
	DPRINTF("gpuperf: %s registered\n", node->name);

	node->callback(GP_AUTO, node->arg);

	return (0);
}

int
gpuperf_set(int level)
{
	struct gpuperf_node *node;
	int status = 0;

	if ((level < GP_LOW) || (level > GP_HIGH))
		return -1;

	LIST_FOREACH(node, &gpuperf_nodes, gp_list) {
		status = node->callback(level, node->arg);

		DPRINTF("gpuperf: req lvl %d (%s ret %d)\n",
		    level, node->name, status);

	}

	return (0);
}
