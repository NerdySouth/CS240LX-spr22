// weak test of malloc/free: checks that subsequent allocations are
// <sizeof (void *)> bytes from each other.

#ifndef __CKALLOC_H__
#define __CKALLOC_H__

#include "src-loc.h"
#include "kr-malloc.h"



typedef enum {  ALLOCED = 11, FREED } state_t;

// pull the remainder into the second redzone.
typedef struct ck_hdr {
    struct ck_hdr *next;
    uint32_t nbytes_alloc;  // how much the user requested to allocate.
    uint32_t state;          // state of the block: { ALLOCED, FREED }

    src_loc_t alloc_loc;    // location they called ckalloc() at.

    // used for gc: i didn't cksum these.
    uint32_t refs_start;    // number of pointers to the start of the block.
    uint32_t refs_middle;   // number of pointers to the middle of the block.

    uint16_t mark;          // 0 initialize.
	uint16_t leaked;
} hdr_t;

static hdr_t *alloc_list;
static hdr_t *last_node = NULL;

// returns pointer to the first header block.
hdr_t *ck_first_hdr(void);

// returns pointer to next hdr or 0 if none.
hdr_t *ck_next_hdr(hdr_t *p);

// get the number of bytes for hdr <h>
static inline unsigned ck_nbytes(hdr_t *h) {
    return h->nbytes_alloc;
}
// get the start of allocated data for <h>
void *ck_hdr_start(hdr_t *h);
// get the end of allocated data for <h>
void *ck_hdr_end(hdr_t *h);

// is <ptr> in block <h>?
unsigned 
ck_ptr_in_block(hdr_t *h, void *ptr);

// is pointer <p> on the allocated list?
hdr_t* ck_ptr_is_alloced(void *ptr);

#define ckalloc(_n) (ckalloc)(_n, SRC_LOC_MK())
#define ckfree(_ptr) (ckfree)(_ptr, SRC_LOC_MK())
void *(ckalloc)(uint32_t nbytes, src_loc_t loc);
void (ckfree)(void *addr, src_loc_t loc);


// integrity check the allocated / freed blocks in the heap
//
// returns number of errors in the heap.   stops checking
// if heap is in an unrecoverable state.
//
// TODO (extensions): 
//  - probably should have returned an error log, so that the 
//    caller could report / fix / etc.
//  - give an option to fix errors so that you can keep going.
int ck_heap_errors(void);


// public function: call to flag leaks (part 1 of lab).
//  - <warn_no_start_ref_p>: if true, will warn about blocks that only have 
//    internal references.
//
//  - returns number of bytes leaked (where bytes = amount of bytes the
//    user explicitly allocated: does not include redzones, header, etc).
unsigned ck_find_leaks(int warn_no_start_ref_p);

// mark and sweep: works similarly to ck_find_leaks, resets
// state of unrefernced blocks.
// 
// Invariant:
//  - it should always be the case that after calling ck_gc(), 
//    immediately calling ck_find_leaks() should return 0 bytes 
//    found.
unsigned ck_gc(void);

// These two routines are just used for testing:

// Expects no leaks.
void check_no_leak(void);
// Expects leaks.
unsigned check_should_leak(void);

// info about the heap useful for checking.
struct heap_info {
    // original start of the heap.
    void *heap_start;
    // end of active heap (the next byte we would allocate)
    void *heap_end;

    // ckfree increments this on each free.
    unsigned nbytes_freed;
    // ckmalloc increments this on each free.
    unsigned nbytes_alloced;
};

struct heap_info heap_info(void);


// can use this to do debugging that you can turn off an on.
#if 1
#define ck_debug(args...) debug("CK_DEBUG:" args)
#else
#define ck_debug(args...) do { } while(0)
#endif

// just emits an error.
#define ck_error(_h, args...) do {      \
        trace("ERROR:");\
        printk(args);                    \
        hdr_print(_h);                  \
} while(0)

// emit error, then panic.
#define ck_panic(_h, args...) do {      \
        trace("ERROR:");\
        printk(args);                    \
        hdr_print(_h);                  \
        panic(args);                    \
} while(0)

// shouldn't be in the header.
static void inline hdr_print(hdr_t *h) {
    trace("\tnbytes=%d\n", h->nbytes_alloc);
    src_loc_t *l = &h->alloc_loc;
    if(l->file)
        trace("\tBlock allocated at: %s:%s:%d\n", l->file, l->func, l->lineno);

#if 0
    // next lab
    l = &h->free_loc;
    if(h->state == FREED && l->file)
        trace("\tBlock freed at: %s:%s:%d\n", l->file, l->func, l->lineno);
#endif
}


#endif
