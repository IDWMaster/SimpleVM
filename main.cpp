/*
 Copyright 2016 Brian Bosak -- All Rights Reserved
 This program is intended for educational purposes only. It does not offer any meaningful security features;
 nor is it a robust alternative to hardware virtualization. If you're a student, interested in learning about system architecture;
 take a look at the boot.s file.
 
 */


#include <stdio.h>
#include <signal.h>
#include <sys/ucontext.h>
#include <string.h>
#include <malloc.h>
#include <sys/mman.h>
#include <stdlib.h>
static void* ram;
static void* mapping;
static size_t maplen;
extern "C" {
greg_t jmpdest;
void* faultaddr;
}



void doublefault_handler(int signum, siginfo_t* si, void* arg) {
  switch((size_t)faultaddr) {
    case 0xa00000:
    {
      printf("%s",*((const char**)faultaddr));
    }
  }
  mprotect(mapping,maplen,PROT_NONE);
  
  ucontext* context = (ucontext*)arg;
  context->uc_mcontext.gregs[REG_EFL] = 0; //Clear flags register (TODO: Should only clear the EFL bit)
}



void handler(int signum, siginfo_t* si, void* arg) {
  
  faultaddr = si->si_addr;
  ucontext* context = (ucontext*)arg;
  jmpdest = context->uc_mcontext.gregs[REG_RIP];
 
  switch((size_t)faultaddr) {
    case 0xa00010:
    {
      mprotect(mapping,maplen,PROT_READ | PROT_WRITE);
      memcpy((void*)0xa00010,&ram,sizeof(size_t));
      context->uc_mcontext.gregs[REG_EFL] |= 1 << 8; //Set trap flag
      
    }
    break;
    case 0xa00000:
    {
      
      mprotect(mapping,maplen,PROT_READ | PROT_WRITE);
      context->uc_mcontext.gregs[REG_EFL] |= 1 << 8; //Set trap flag
      
    }
    break;
    case 0xa00008:
    {
      exit(0);
    }
    break;
    default:
      printf("VM fault. Illegal memory access at address %p (performing VM core dump).\n",faultaddr);
      abort();
  }
  
 // printf("Passing control back to jmpdest\n");
  
  //((void(*)())jmpdest)();
  //printf("ERROR -- We should not have returned. VM will be halted with segmentation fault.\n");
  //asm("jmp");
}

int main(int argc, char** argv) {
ram = malloc(1024*1024*1024);
  printf("VM startup -- Firmware initialization\n");
printf("VM information\nText out (DMA) at address 0xa00000\nPower off pin at address 0xa00008\nRAM at addres 0xa00010\n");
maplen = 4096;
mapping = mmap((void*)(1024*1024*10),4096,PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED,0,0);
mprotect(mapping,maplen,PROT_NONE);
printf("VM -- Mapping created at %p\n",mapping);
printf("Physical RAM at address %p\n",ram);
struct sigaction act;
memset(&act,0,sizeof(act));
sigemptyset(&act.sa_mask);
act.sa_sigaction = handler;
act.sa_flags = SA_SIGINFO;

sigaction(SIGSEGV,&act,0);

struct sigaction act2;
memset(&act2,0,sizeof(act2));
sigemptyset(&act2.sa_mask);
act2.sa_sigaction = doublefault_handler;
act2.sa_flags = SA_SIGINFO;

sigaction(SIGTRAP,&act2,0);


int* warranty = 0;
//*warranty = 5;
printf("Startup disk found. Passing control to bootloader....\n");

__asm("jmp _boot");
}
