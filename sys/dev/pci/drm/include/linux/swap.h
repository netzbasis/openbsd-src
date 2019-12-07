/* Public domain. */

#ifndef _LINUX_SWAP_H
#define _LINUX_SWAP_H

#include <uvm/uvm_extern.h>

static inline long
get_nr_swap_pages(void)
{ 
	return uvmexp.swpages - uvmexp.swpginuse;
}

#define current_is_kswapd()	0

#endif
