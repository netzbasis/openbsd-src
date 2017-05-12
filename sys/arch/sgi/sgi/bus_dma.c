/*	$OpenBSD: bus_dma.c,v 1.41 2017/05/11 15:47:45 visa Exp $ */

/*
 * Copyright (c) 2003-2004 Opsycon AB  (www.opsycon.se / www.opsycon.com)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */
/*-
 * Copyright (c) 1996, 1997, 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>

#include <uvm/uvm_extern.h>

#include <mips64/cache.h>
#include <machine/cpu.h>

#include <machine/bus.h>

#if defined(TGT_INDY) || defined(TGT_INDIGO2)
#include <sgi/sgi/ip22.h>
#endif

/*
 * Common function for DMA map creation.  May be called by bus-specific
 * DMA map creation functions.
 */
int
_dmamap_create(bus_dma_tag_t t, bus_size_t size, int nsegments,
    bus_size_t maxsegsz, bus_size_t boundary, int flags, bus_dmamap_t *dmamp)
{
	struct machine_bus_dmamap *map;
	void *mapstore;
	size_t mapsize;

	/*
	 * Allocate and initialize the DMA map.  The end of the map
	 * is a variable-sized array of segments, so we allocate enough
	 * room for them in one shot.
	 *
	 * Note we don't preserve the WAITOK or NOWAIT flags.  Preservation
	 * of ALLOCNOW notifies others that we've reserved these resources,
	 * and they are not to be freed.
	 *
	 * The bus_dmamap_t includes one bus_dma_segment_t, hence
	 * the (nsegments - 1).
	 */
	mapsize = sizeof(struct machine_bus_dmamap) +
	    (sizeof(bus_dma_segment_t) * (nsegments - 1));
	if ((mapstore = malloc(mapsize, M_DEVBUF, (flags & BUS_DMA_NOWAIT) ?
	    (M_NOWAIT | M_ZERO) : (M_WAITOK | M_ZERO))) == NULL)
		return (ENOMEM);

	map = (struct machine_bus_dmamap *)mapstore;
	map->_dm_size = size;
	map->_dm_segcnt = nsegments;
	map->_dm_maxsegsz = maxsegsz;
	map->_dm_boundary = boundary;
	map->_dm_flags = flags & ~(BUS_DMA_WAITOK|BUS_DMA_NOWAIT);

	*dmamp = map;
	return (0);
}

/*
 * Common function for DMA map destruction.  May be called by bus-specific
 * DMA map destruction functions.
 */
void
_dmamap_destroy(bus_dma_tag_t t, bus_dmamap_t map)
{
	free(map, M_DEVBUF, 0);
}

/*
 * Common function for loading a DMA map with a linear buffer.  May
 * be called by bus-specific DMA map load functions.
 */
int
_dmamap_load(bus_dma_tag_t t, bus_dmamap_t map, void *buf, bus_size_t buflen,
    struct proc *p, int flags)
{
	paddr_t lastaddr;
	int seg, error;

	/*
	 * Make sure that on error condition we return "no valid mappings".
	 */
	map->dm_nsegs = 0;
	map->dm_mapsize = 0;

	if (buflen > map->_dm_size)
		return (EINVAL);

	seg = 0;
	error = (*t->_dmamap_load_buffer)(t, map, buf, buflen, p, flags,
	    &lastaddr, &seg, 1);
	if (error == 0) {
		map->dm_nsegs = seg + 1;
		map->dm_mapsize = buflen;
	}

	return (error);
}

/*
 * Like _bus_dmamap_load(), but for mbufs.
 */
int
_dmamap_load_mbuf(bus_dma_tag_t t, bus_dmamap_t map, struct mbuf *m0, int flags)
{
	paddr_t lastaddr;
	int seg, error, first;
	struct mbuf *m;

	/*
	 * Make sure that on error condition we return "no valid mappings".
	 */
	map->dm_nsegs = 0;
	map->dm_mapsize = 0;

#ifdef DIAGNOSTIC
	if ((m0->m_flags & M_PKTHDR) == 0)
		panic("_dmamap_load_mbuf: no packet header");
#endif

	if (m0->m_pkthdr.len > map->_dm_size)
		return (EINVAL);

	first = 1;
	seg = 0;
	error = 0;
	for (m = m0; m != NULL && error == 0; m = m->m_next) {
		if (m->m_len == 0)
			continue;
		error = (*t->_dmamap_load_buffer)(t, map, m->m_data, m->m_len,
		    NULL, flags, &lastaddr, &seg, first);
		first = 0;
	}
	if (error == 0) {
		map->dm_nsegs = seg + 1;
		map->dm_mapsize = m0->m_pkthdr.len;
	}

	return (error);
}

/*
 * Like _dmamap_load(), but for uios.
 */
int
_dmamap_load_uio(bus_dma_tag_t t, bus_dmamap_t map, struct uio *uio, int flags)
{
	paddr_t lastaddr;
	int seg, i, error, first;
	bus_size_t minlen, resid;
	struct proc *p = NULL;
	struct iovec *iov;
	void *addr;

	/*
	 * Make sure that on error condition we return "no valid mappings".
	 */
	map->dm_nsegs = 0;
	map->dm_mapsize = 0;

	resid = uio->uio_resid;
	iov = uio->uio_iov;

	if (uio->uio_segflg == UIO_USERSPACE) {
		p = uio->uio_procp;
#ifdef DIAGNOSTIC
		if (p == NULL)
			panic("_dmamap_load_uio: USERSPACE but no proc");
#endif
	}

	first = 1;
	seg = 0;
	error = 0;
	for (i = 0; i < uio->uio_iovcnt && resid != 0 && error == 0; i++) {
		/*
		 * Now at the first iovec to load.  Load each iovec
		 * until we have exhausted the residual count.
		 */
		minlen = resid < iov[i].iov_len ? resid : iov[i].iov_len;
		addr = (void *)iov[i].iov_base;

		error = (*t->_dmamap_load_buffer)(t, map, addr, minlen,
		    p, flags, &lastaddr, &seg, first);
		first = 0;

		resid -= minlen;
	}
	if (error == 0) {
		map->dm_nsegs = seg + 1;
		map->dm_mapsize = uio->uio_resid;
	}

	return (error);
}

/*
 * Like _dmamap_load(), but for raw memory allocated with
 * bus_dmamem_alloc().
 */
int
_dmamap_load_raw(bus_dma_tag_t t, bus_dmamap_t map, bus_dma_segment_t *segs,
    int nsegs, bus_size_t size, int flags)
{
	if (nsegs > map->_dm_segcnt || size > map->_dm_size)
		return (EINVAL);

	/*
	 * Make sure we don't cross any boundaries.
	 */
	if (map->_dm_boundary) {
		bus_addr_t bmask = ~(map->_dm_boundary - 1);
		int i;

		if (t->_dma_mask != 0)
			bmask &= t->_dma_mask;
		for (i = 0; i < nsegs; i++) {
			if (segs[i].ds_len > map->_dm_maxsegsz)
				return (EINVAL);
			if ((segs[i].ds_addr & bmask) !=
			    ((segs[i].ds_addr + segs[i].ds_len - 1) & bmask))
				return (EINVAL);
		}
	}

	bcopy(segs, map->dm_segs, nsegs * sizeof(*segs));
	map->dm_nsegs = nsegs;
	map->dm_mapsize = size;
	return (0);
}

/*
 * Common function for unloading a DMA map.  May be called by
 * bus-specific DMA map unload functions.
 */
void
_dmamap_unload(bus_dma_tag_t t, bus_dmamap_t map)
{
	/*
	 * No resources to free; just mark the mappings as
	 * invalid.
	 */
	map->dm_nsegs = 0;
	map->dm_mapsize = 0;
}

/*
 * Common function for DMA map synchronization.  May be called
 * by bus-specific DMA map synchronization functions.
 */
void
_dmamap_sync(bus_dma_tag_t t, bus_dmamap_t map, bus_addr_t addr,
    bus_size_t size, int op)
{
	int nsegs;
	int curseg;
	int how;
	struct cpu_info *ci;

#ifdef TGT_COHERENT
	/* we only need to writeback here */
	if ((op & BUS_DMASYNC_PREWRITE) == 0)
		return;
	else
		how = CACHE_SYNC_W;
#else
	/*
	 * If only PREWRITE is requested, writeback.
	 * PREWRITE with PREREAD writebacks and invalidates (since noncoherent)
	 * *all* cache levels.
	 * Otherwise, just invalidate (since noncoherent).
	 */
	if (op & BUS_DMASYNC_PREWRITE) {
		if (op & BUS_DMASYNC_PREREAD)
			how = CACHE_SYNC_X;
		else
			how = CACHE_SYNC_W;
	} else {
		if (op & (BUS_DMASYNC_PREREAD | BUS_DMASYNC_POSTREAD))
			how = CACHE_SYNC_R;
		else
			return;
	}
#endif

	ci = curcpu();
	nsegs = map->dm_nsegs;
	curseg = 0;

	while (size && nsegs) {
		paddr_t paddr;
		vaddr_t vaddr;
		bus_size_t ssize;

		ssize = map->dm_segs[curseg].ds_len;
		paddr = map->dm_segs[curseg]._ds_paddr;
		vaddr = map->dm_segs[curseg]._ds_vaddr;

		if (addr != 0) {
			if (addr >= ssize) {
				addr -= ssize;
				ssize = 0;
			} else {
				vaddr += addr;
				paddr += addr;
				ssize -= addr;
				addr = 0;
			}
		}
		if (ssize > size)
			ssize = size;

#ifndef TGT_COHERENT
		if (IS_XKPHYS(vaddr) && XKPHYS_TO_CCA(vaddr) == CCA_NC) {
			size -= ssize;
			ssize = 0;
		}
#endif

		if (ssize != 0) {
			Mips_IOSyncDCache(ci, vaddr, ssize, how);
#if defined(TGT_INDY) || defined(TGT_INDIGO2)
			/*
			 * Also flush external L2 if available - this could
			 * (and used to) be done in Mips_IOSyncDCache, but
			 * as the external L2 is physically addressed, this
			 * would require the physical address to be
			 * recomputed, although we know it here.
			 */
			if (ip22_extsync != NULL)
				(*ip22_extsync)(ci, paddr, ssize, how);
#endif
			size -= ssize;
		}
		curseg++;
		nsegs--;
	}

#ifdef DIAGNOSTIC
	if (size != 0)
		panic("_dmamap_sync: ran off map!");
#endif
}

/*
 * Common function for DMA-safe memory allocation.  May be called
 * by bus-specific DMA memory allocation functions.
 */
int
_dmamem_alloc(bus_dma_tag_t t, bus_size_t size, bus_size_t alignment,
    bus_size_t boundary, bus_dma_segment_t *segs, int nsegs, int *rsegs,
    int flags)
{
	return _dmamem_alloc_range(t, size, alignment, boundary,
	    segs, nsegs, rsegs, flags,
	    dma_constraint.ucr_low, dma_constraint.ucr_high);
}

/*
 * Common function for freeing DMA-safe memory.  May be called by
 * bus-specific DMA memory free functions.
 */
void
_dmamem_free(bus_dma_tag_t t, bus_dma_segment_t *segs, int nsegs)
{
	vm_page_t m;
	paddr_t pa;
	struct pglist mlist;
	int curseg;

	/*
	 * Build a list of pages to free back to the VM system.
	 */
	TAILQ_INIT(&mlist);
	for (curseg = 0; curseg < nsegs; curseg++) {
		for (pa = segs[curseg]._ds_paddr;
		    pa < (segs[curseg]._ds_paddr + segs[curseg].ds_len);
		    pa += PAGE_SIZE) {
			m = PHYS_TO_VM_PAGE(pa);
			TAILQ_INSERT_TAIL(&mlist, m, pageq);
		}
	}

	uvm_pglistfree(&mlist);
}

/*
 * Common function for mapping DMA-safe memory.  May be called by
 * bus-specific DMA memory map functions.
 */
int
_dmamem_map(bus_dma_tag_t t, bus_dma_segment_t *segs, int nsegs, size_t size,
    caddr_t *kvap, int flags)
{
	vaddr_t va, sva;
	size_t ssize;
	paddr_t pa;
	int curseg, error, pmap_flags;
	const struct kmem_dyn_mode *kd;

#if defined(TGT_INDIGO2)
	/*
	 * On ECC MC systems, which do not allow uncached writes to memory
	 * during regular operation, fail requests for uncached (coherent)
	 * memory.
	 */
	if ((flags & (BUS_DMA_COHERENT | BUS_DMA_NOCACHE)) && ip22_ecc)
		return EINVAL;
#endif

#ifdef TGT_COHERENT
	/* coherent mappings do not need to be uncached on these platforms */
	flags &= ~BUS_DMA_COHERENT;
#endif

	if (nsegs == 1) {
		pa = segs[0]._ds_paddr;
		if (flags & (BUS_DMA_COHERENT | BUS_DMA_NOCACHE))
			*kvap = (caddr_t)PHYS_TO_XKPHYS(pa, CCA_NC);
		else
			*kvap = (caddr_t)PHYS_TO_XKPHYS(pa, CCA_CACHED);
		return (0);
	}

	size = round_page(size);
	kd = flags & BUS_DMA_NOWAIT ? &kd_trylock : &kd_waitok;
	va = (vaddr_t)km_alloc(size, &kv_any, &kp_none, kd);
	if (va == 0)
		return (ENOMEM);

	*kvap = (caddr_t)va;

	sva = va;
	ssize = size;
	pmap_flags = PMAP_WIRED | PMAP_CANFAIL;
	if (flags & (BUS_DMA_COHERENT | BUS_DMA_NOCACHE))
		pmap_flags |= PMAP_NOCACHE;
	for (curseg = 0; curseg < nsegs; curseg++) {
		for (pa = segs[curseg]._ds_paddr;
		    pa < (segs[curseg]._ds_paddr + segs[curseg].ds_len);
		    pa += PAGE_SIZE, va += PAGE_SIZE, size -= PAGE_SIZE) {
#ifdef DIAGNOSTIC
			if (size == 0)
				panic("_dmamem_map: size botch");
#endif
			error = pmap_enter(pmap_kernel(), va, pa,
			    PROT_READ | PROT_WRITE,
			    PROT_READ | PROT_WRITE | pmap_flags);
			if (error) {
				pmap_update(pmap_kernel());
				km_free((void *)sva, ssize, &kv_any, &kp_none);
				return (error);
			}

			/*
			 * This is redundant with what pmap_enter() did
			 * above, but will take care of forcing other
			 * mappings of the same page (if any) to be
			 * uncached.
			 * If there are no multiple mappings of that
			 * page, this amounts to a noop.
			 */
			if (flags & (BUS_DMA_COHERENT | BUS_DMA_NOCACHE))
				pmap_page_cache(PHYS_TO_VM_PAGE(pa),
				    PGF_UNCACHED);
		}
		pmap_update(pmap_kernel());
	}

	return (0);
}

/*
 * Common function for unmapping DMA-safe memory.  May be called by
 * bus-specific DMA memory unmapping functions.
 */
void
_dmamem_unmap(bus_dma_tag_t t, caddr_t kva, size_t size)
{
	if (IS_XKPHYS((vaddr_t)kva))
		return;

	km_free(kva, round_page(size), &kv_any, &kp_none);
}

/*
 * Common function for mmap(2)'ing DMA-safe memory.  May be called by
 * bus-specific DMA mmap(2)'ing functions.
 */
paddr_t
_dmamem_mmap(bus_dma_tag_t t, bus_dma_segment_t *segs, int nsegs, off_t off,
    int prot, int flags)
{
	int i;

	for (i = 0; i < nsegs; i++) {
#ifdef DIAGNOSTIC
		if (off & PAGE_MASK)
			panic("_dmamem_mmap: offset unaligned");
		if (segs[i].ds_addr & PAGE_MASK)
			panic("_dmamem_mmap: segment unaligned");
		if (segs[i].ds_len & PAGE_MASK)
			panic("_dmamem_mmap: segment size not multiple"
			    " of page size");
#endif
		if (off >= segs[i].ds_len) {
			off -= segs[i].ds_len;
			continue;
		}

		return segs[i]._ds_paddr + off;
	}

	/* Page not found. */
	return (-1);
}

/**********************************************************************
 * DMA utility functions
 **********************************************************************/

/*
 * Utility function to load a linear buffer.  lastaddrp holds state
 * between invocations (for multiple-buffer loads).  segp contains
 * the starting segment on entrance, and the ending segment on exit.
 * first indicates if this is the first invocation of this function.
 */
int
_dmamap_load_buffer(bus_dma_tag_t t, bus_dmamap_t map, void *buf,
    bus_size_t buflen, struct proc *p, int flags, paddr_t *lastaddrp,
    int *segp, int first)
{
	bus_size_t sgsize;
	bus_addr_t lastaddr, baddr, bmask;
	paddr_t curaddr;
	vaddr_t vaddr = (vaddr_t)buf;
	int seg;
	pmap_t pmap;

	if (p != NULL)
		pmap = p->p_vmspace->vm_map.pmap;
	else
		pmap = pmap_kernel();

	lastaddr = *lastaddrp;
	bmask  = ~(map->_dm_boundary - 1);
	if (t->_dma_mask != 0)
		bmask &= t->_dma_mask;

	for (seg = *segp; buflen > 0; ) {
		/*
		 * Get the physical address for this segment.
		 */
		if (pmap_extract(pmap, vaddr, &curaddr) == FALSE)
			panic("_dmapmap_load_buffer: pmap_extract(%p, %p) failed!",
			    pmap, (void *)vaddr);

#ifdef DIAGNOSTIC
		if (curaddr > dma_constraint.ucr_high ||
		    curaddr < dma_constraint.ucr_low)
			panic("Non DMA-reachable buffer at addr %p (raw)",
			    (void *)curaddr);
#endif

		/*
		 * Compute the segment size, and adjust counts.
		 */
		sgsize = NBPG - ((u_long)vaddr & PGOFSET);
		if (buflen < sgsize)
			sgsize = buflen;

		/*
		 * Make sure we don't cross any boundaries.
		 */
		if (map->_dm_boundary > 0) {
			baddr = ((bus_addr_t)curaddr + map->_dm_boundary) &
			    bmask;
			if (sgsize > (baddr - (bus_addr_t)curaddr))
				sgsize = (baddr - (bus_addr_t)curaddr);
		}

		/*
		 * Insert chunk into a segment, coalescing with
		 * previous segment if possible.
		 */
		if (first) {
			map->dm_segs[seg].ds_addr =
			    (*t->_pa_to_device)(curaddr, map->_dm_flags);
			map->dm_segs[seg].ds_len = sgsize;
			map->dm_segs[seg]._ds_paddr = curaddr;
			map->dm_segs[seg]._ds_vaddr = vaddr;
			first = 0;
		} else {
			if ((bus_addr_t)curaddr == lastaddr + 1 &&
			    (map->dm_segs[seg].ds_len + sgsize) <=
			     map->_dm_maxsegsz &&
			     (map->_dm_boundary == 0 ||
			     (map->dm_segs[seg].ds_addr & bmask) ==
			     ((bus_addr_t)curaddr & bmask)))
				map->dm_segs[seg].ds_len += sgsize;
			else {
				if (++seg >= map->_dm_segcnt)
					break;
				map->dm_segs[seg].ds_addr =
				    (*t->_pa_to_device)(curaddr,
				        map->_dm_flags);
				map->dm_segs[seg].ds_len = sgsize;
				map->dm_segs[seg]._ds_paddr = curaddr;
				map->dm_segs[seg]._ds_vaddr = vaddr;
			}
		}

		lastaddr = (bus_addr_t)curaddr + sgsize - 1;
		vaddr += sgsize;
		buflen -= sgsize;
	}

	*segp = seg;
	*lastaddrp = lastaddr;

	/*
	 * Did we fit?
	 */
	if (buflen != 0)
		return (EFBIG);		/* XXX better return value here? */

	return (0);
}

/*
 * Allocate physical memory from the given physical address range.
 * Called by DMA-safe memory allocation methods.
 */
int
_dmamem_alloc_range(bus_dma_tag_t t, bus_size_t size, bus_size_t alignment,
    bus_size_t boundary, bus_dma_segment_t *segs, int nsegs, int *rsegs,
    int flags, paddr_t low, paddr_t high)
{
	paddr_t curaddr, lastaddr;
	vm_page_t m;
	struct pglist mlist;
	int curseg, error, plaflag;

	/* Always round the size. */
	size = round_page(size);

	/*
	 * Allocate pages from the VM system.
	 */
	plaflag = flags & BUS_DMA_NOWAIT ? UVM_PLA_NOWAIT : UVM_PLA_WAITOK;
	if (flags & BUS_DMA_ZERO)
		plaflag |= UVM_PLA_ZERO;

	TAILQ_INIT(&mlist);
	error = uvm_pglistalloc(size, low, high, alignment, boundary,
	    &mlist, nsegs, plaflag);
	if (error)
		return (error);

	/*
	 * Compute the location, size, and number of segments actually
	 * returned by the VM code.
	 */
	m = TAILQ_FIRST(&mlist);
	curseg = 0;
	segs[curseg]._ds_paddr = VM_PAGE_TO_PHYS(m);
	lastaddr = segs[curseg].ds_addr =
	    (*t->_pa_to_device)(segs[curseg]._ds_paddr, flags);
	segs[curseg].ds_len = PAGE_SIZE;
	m = TAILQ_NEXT(m, pageq);

	for (; m != NULL; m = TAILQ_NEXT(m, pageq)) {
		paddr_t pa = VM_PAGE_TO_PHYS(m);
#ifdef DIAGNOSTIC
		if (pa < low || pa >= high) {
			printf("vm_page_alloc_memory returned non-sensical"
			    " address 0x%lx\n", pa);
			panic("_dmamem_alloc_range");
		}
#endif
		curaddr = (*t->_pa_to_device)(pa, flags);
		if (curaddr == (lastaddr + PAGE_SIZE))
			segs[curseg].ds_len += PAGE_SIZE;
		else {
			curseg++;
			segs[curseg]._ds_paddr = pa;
			segs[curseg].ds_addr = curaddr;
			segs[curseg].ds_len = PAGE_SIZE;
		}
		lastaddr = curaddr;
	}

	*rsegs = curseg + 1;

	return (0);
}
