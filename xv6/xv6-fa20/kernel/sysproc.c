#include "types.h"
#include "x86.h"
#include "defs.h"
#include "param.h"
#include "mmu.h"
#include "proc.h"
#include "sysfunc.h"
#include "spinlock.h"

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
  return proc->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = proc->sz;
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
    if(proc->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since boot.
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
sys_setpri(void)
{
  struct proc *p;
  int pid;
  int pri;

  struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

  if(argint(0, &pid) < 0 || argint(1, &pri) < 0)
    return -1;

  if(pri!=0&&pri!=1&&pri!=2&&pri!=3) {
    return -1;
  }

  initlock(&ptable.lock, "ptable");

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid==pid) {
      p->priority=pri;
      release(&ptable.lock);
      //need to update the status table after this returns.
      return 1;
    }
  }
  release(&ptable.lock);
  return -1;
}

int
sys_getpri(void)
{
  struct proc *p;
  int pri;
  int pid;

  struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

  if(argint(0, &pid) < 0)
    return -1;

  initlock(&ptable.lock, "ptable");

  acquire(&ptable.lock);
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    if(p->pid==pid) {
      pri = p->priority;
      release(&ptable.lock);
      
      return pri;
    }
  }
  release(&ptable.lock);
  return -1;
}

int
sys_getpinfo(void)
{
  struct pstat* status;

    // if(argint(0, &status) < 0))
      return -1;

  //not implemented.
  struct proc *p;
  
  struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;


initlock(&ptable.lock, "ptable");

  acquire(&ptable.lock);
  int i=0;
  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
    status->inuse[i]=1;
    }
  release(&ptable.lock);
  return -1;
}