#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  myproc()->heap_size+= n;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_hello(void)
{
  return hello();
}

int sys_helloYou(void){
  char *n;
  if(argstr(0, &n) < 0)
    return -1;
  return helloYou(n);
}

int sys_getNumProc(void){
  return getNumProc();
}

int sys_getMaxPid(void){
  return getMaxPid();
}

int sys_getProcInfo(void){
  int pid, p;
  if(argint(0, &pid)<0) return -1;      //0 because it is the first argument
  if(argint(1, &p)<0) return -1;        //1 because its the second argument
  return getProcInfo(pid,  (struct processInfo*)p);
}

int sys_welcomeFunction(void){
  int p;
  if(argint(0, &p)<0) return -1;
  myproc()->welc_addr = (char*)p;
  return 0;
}

int sys_welcomeDone(void){
  if(myproc()->ret_addr){
    myproc()->tf->eip = (uint)(myproc()->ret_addr);
    return 0;
  }
  return -1;
}

int sys_setprio(void){
  int p;
  if(argint(0,&p)<0) return -1;
  myproc()->priority = p;
  return 0;
}

int sys_getprio(void){
  return myproc()->priority;
}

int sys_heapSize(void){
  return myproc()->heap_size;
}

int sys_getLevel(void){
  int pid;
  if(argint(0,&pid)<0) return -1;

  return getLevel(pid);
}

int sys_trace(void){
  myproc()->trace_enable = 1;
  return 0;
}