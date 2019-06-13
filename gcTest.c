// hw7test.c  Deliberately greate garbage to see if gc can find it.

#include "memorymgr.h"
#include "gc.h"
#include <stdio.h>

int main() {
  char *p;
  initmemory(100);

  printf("malloc 4\n");
  p = (char *)myalloc(4);
  printallocation();

  printf("malloc 6; the malloc 4 is now garbage\n");
  p = (char *)myalloc(6);
  printallocation();

  printf("malloc 22; the malloc 6 is now garbage\n");
  p = (char *)myalloc(22);
  printallocation();

  printf("gc: Should free the first two blocks and coalesce them\n"); 
  gc();
  printallocation();
}
