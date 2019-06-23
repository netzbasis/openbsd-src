
#ifndef	SMALL_KERNEL
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/resourcevar.h>
#include <sys/fcntl.h>
#include <sys/unistd.h>
#include <sys/sysctl.h>

#define GPUPERF_DEBUG
#ifdef GPUPERF_DEBUG
#define DPRINTF(x...)	do { printf(x); } while(0)
#else
#define DPRINTF(x...)
#endif /* GPUPERF_DEBUG */

#define GPUPERF_MAX_NODES 4

struct gpuperf_node {
	char name[16];			/* gpu driver name */
	void (*callback)(int, void*);	/* must understand 0 - 100 as % */ 
	void *arg;			/* arg passthrough */
}

struct gpuperf_node gpuperf_registered_nodes[GPUPERF_MAX_NODES];

int gpuperf = 100;
int gpuperf_gpun = 0;

int sysctl_hwgpuperf(void *, size_t *, void *, size_t);
int gpuperf_register(const char*, void (*)(int, void *));

int
sysctl_hwgpuperf(void *oldp, size_t *oldlenp, void *newp, size_t newlen)
{
	int i, err, newperf;

	if (gpuperf_gpun == 0)
		return EOPNOTSUPP;

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
		DPRINTF("gpuperf: requesting level %d from %s\n",
		    gpuperf, gpuperf_registered_nodes[i].name);

		gpuperf_registered_nodes[i].callback(gpuperf,
		    gpuperf_registered_nodes[i].arg);
	}

	return 0;
}

int
gpuperf_register(const char *name, void (*callback)(int))
{
	if (gpuperf_gpun >= GPUPERF_MAX_NODES)
		return (-1);

	strlcpy(gpuperf_registered_nodes[gpuperf_gpun].name, name,
	    sizeof(gpuperf_registered_nodes[gpuperf_gpun].name));
	gpuperf_registered_nodes[gpuperf_gpun].callback = callback;
	gpuperf_gpun += 1;
	DPRINTF("gpuperf: %s registered callback (total nodes %d)\n",
	    name, gpuperf_gpun);

	callback(gpuperf, gpuperf_registered_nodes[i].arg);
}
#endif	/* SMALL_KERNEL */
