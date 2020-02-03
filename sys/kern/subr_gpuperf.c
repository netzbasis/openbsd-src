/*	$OpenBSD$	*/

/*
 * Copyright (c) 2019, 2020 Benjamin Baier <ben@netzbasis.de>
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
#include <sys/malloc.h>

#ifdef GPUPERF_DEBUG
#define DPRINTF(x...) do { printf(x); } while(0)
#else
#define DPRINTF(x...)
#endif /* GPUPERF_DEBUG */

struct gpuperf_dev {
	char name[16];				/* device name */
	void *arg;				/* arg passthrough */
	int (*callback)(gpuperf_level, void *);
	LIST_ENTRY(gpuperf_dev) next;
};

LIST_HEAD(, gpuperf_dev) gpuperf_list =
	LIST_HEAD_INITIALIZER(gpuperf_list);

int
gpuperf_register(const char *name, int (*callback)(gpuperf_level, void *),
    void *arg)
{
	struct gpuperf_dev *dev;
	int status = 0;

	if ((dev = malloc(sizeof(*dev), M_DEVBUF, M_NOWAIT)) == NULL)
		return -1;

	strlcpy(dev->name, name, sizeof(dev->name));
	dev->callback = callback;
	dev->arg = arg;

	LIST_INSERT_HEAD(&gpuperf_list, dev, next);
	status = dev->callback(GPU_AUTO, dev->arg);

	DPRINTF("gpuperf: %s registered, status %d\n", dev->name, status);

	return status;
}

int
gpuperf_set(gpuperf_level level)
{
	struct gpuperf_dev *dev;
	int status = 0;

	if ((level < GPU_AUTO) || (level > GPU_HIGH))
		return -1;

	LIST_FOREACH(dev, &gpuperf_list, next) {
		status += dev->callback(level, dev->arg);

		DPRINTF("gpuperf: requesting %d from %s, status %d\n",
		    level, dev->name, status);
	}

	return status;
}
