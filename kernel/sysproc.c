#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
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

// reports which pages have been accessed.
int
sys_pgaccess(void)
{
  //it takes the starting virtual address of the first user page to check.
 uint64  va; 
 if(argaddr(0,&va)<0){
  return -1;
 }

 //it takes the number of pages to check.
 int numpages;
if(argint(1,&numpages)<0){
  return -1;
}
if(numpages>32||numpages<0){
  return -1;
}
 
 //it takes a user address to a buffer to store the results into a bitmask
 uint64 user_buffer;
 if(argaddr(2,&user_buffer)<0){
  return -1;
 }

//Initilize bitmask;
uint64 bitmask=0;

for(int i=0;i<numpages;i++){
  uint64 addr=va+i*PGSIZE;
  if(addr>MAXVA){
    return -1;
  }
  pte_t *pte=walk(myproc()->pagetable,addr,0);
//detect and clear the PTE_A,and add it in bitmask
  if(*pte&PTE_V){
    if(*pte&PTE_A){
      bitmask|=(1<<i);
      *pte&=(~PTE_A);
    }
  }
} 
//bitmask 前面需要加&，不然会触发kernel trap.
  if(copyout(myproc()->pagetable,user_buffer,(char *)&bitmask,sizeof(bitmask))){
    return -1;
  }


  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


