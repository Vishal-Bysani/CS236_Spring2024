#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int main(void)
{
  printf(1,"getNumFreePages: %d\n", getNumFreePages());
  return 0;
}
