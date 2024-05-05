// Physical memory allocator, intended to allocate
// memory for user processes, kernel stacks, page table pages,
// and pipe buffers. Allocates 4096-byte pages.

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "spinlock.h"

void freerange(void *vstart, void *vend);
extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  int use_lock;
  int referenceCounts[PHYSTOP/PGSIZE];
} refCounts;

struct {
  struct spinlock lock;
  int use_lock;
  struct run *freelist;
} kmem;

// Initialization happens in two phases.
// 1. main() calls kinit1() while still using entrypgdir to place just
// the pages mapped by entrypgdir on free list.
// 2. main() calls kinit2() with the rest of the physical pages
// after installing a full page table that maps them on all cores.
void
kinit1(void *vstart, void *vend)
{
  initlock(&kmem.lock, "kmem");
  kmem.use_lock = 0;

  initlock(&refCounts.lock, "refCounts");
  refCounts.use_lock = 0;

  for(int i=0;i<PHYSTOP/PGSIZE; i++){
    refCounts.referenceCounts[i] = 0;
  }

  freerange(vstart, vend);
}

void
kinit2(void *vstart, void *vend)
{
  freerange(vstart, vend);
  kmem.use_lock = 1;
  refCounts.use_lock = 1;
}

void
freerange(void *vstart, void *vend)
{
  char *p;
  p = (char*)PGROUNDUP((uint)vstart);
  for(; p + PGSIZE <= (char*)vend; p += PGSIZE)
    kfree(p);
}
//PAGEBREAK: 21
// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(char *v)
{
  struct run *r;

  if((uint)v % PGSIZE || v < end || V2P(v) >= PHYSTOP)
    panic("kfree");

  if(refCounts.use_lock)
    acquire(&refCounts.lock);
  
  if(refCounts.referenceCounts[V2P(v)/PGSIZE]< 0)
    panic("kfree : refcounts < 0");
  
  if(refCounts.referenceCounts[V2P(v)/PGSIZE]>0){
    
    refCounts.referenceCounts[V2P(v)/PGSIZE] --;

    if(refCounts.referenceCounts[V2P(v)/PGSIZE]>0){
      if(refCounts.use_lock)
      release(&refCounts.lock);
      return;
    }
  }
  
  // Fill with junk to catch dangling refs.
  memset(v, 1, PGSIZE);

  if(kmem.use_lock)
    acquire(&kmem.lock);

  r = (struct run*)v;
  r->next = kmem.freelist;
  kmem.freelist = r;

  if(refCounts.use_lock)
    release(&refCounts.lock);

  if(kmem.use_lock)
    release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
char*
kalloc(void)
{
  struct run *r;

  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
  {
    if(refCounts.use_lock)
      acquire(&refCounts.lock);
    
    refCounts.referenceCounts[V2P(r)/PGSIZE] = 1;

    if(refCounts.use_lock)
      release(&refCounts.lock);

    kmem.freelist = r->next;
  }  
  if(kmem.use_lock)
    release(&kmem.lock);
  return (char*)r;
}

int freePages(void){
  struct run * r;
  int pages = 0;
  if(kmem.use_lock)
    acquire(&kmem.lock);
  r = kmem.freelist;
  while(r){
    pages++;
    r=r->next;
  }
  if(kmem.use_lock)
    release(&kmem.lock);
  return pages;
}

void increment(int pa){

  if((int)(pa)<0 || (int)(pa)>=PHYSTOP)
    panic("increment : pa out of bounds");

  if(refCounts.use_lock)
    acquire(&refCounts.lock);

  refCounts.referenceCounts[pa/PGSIZE] +=1;
  if(refCounts.use_lock)
    release(&refCounts.lock);
}

void decrement(int pa){

  if((int)(pa)<0 || (int)(pa)>=PHYSTOP)
    panic("decrement : pa out of bounds");

  if(refCounts.use_lock)
    acquire(&refCounts.lock);

  refCounts.referenceCounts[pa/PGSIZE] -=1;
  if(refCounts.use_lock)
    release(&refCounts.lock);
}

int getRefCounts(int pa){
  if((int)(pa)<0 || (int)(pa)>=PHYSTOP)
    panic("getRefCounts : pa out of bounds");

  if(refCounts.use_lock)
    acquire(&refCounts.lock);

  int c = refCounts.referenceCounts[pa/PGSIZE];
  if(refCounts.use_lock)
    release(&refCounts.lock);

  return c;
}