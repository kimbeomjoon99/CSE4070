#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void sysexit(int s){
	struct thread *cur = thread_current();
	cur->exit_status = s;
	printf("%s: exit(%d)\n", cur->name, s);
	thread_exit();
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int key = *(int*)(f->esp);
  //printf("%d %d %d\n", key, *((int*)(f->esp + (sizeof(void*) * 3))), *(int*)(f->esp + 12));
  //thread_exit();
  //return;
  switch(key){
    case SYS_HALT:
      shutdown_power_off();
      break;
    case SYS_EXIT:
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      else sysexit(*(int*)(f->esp + sizeof(void*)));
      break;
    case SYS_EXEC:
      //printf("hi!\n");
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      else{
        //printf("hi1\n");
        f->eax = process_execute((char*)(*(uint32_t*)(f->esp + sizeof(void*))));
        //printf("hi2\n");	
      }
      break;
    case SYS_WAIT:
      if (!is_user_vaddr(f->esp + sizeof(void*))) sysexit(-1);
      else{
        f->eax = process_wait(*(int*)(f->esp + sizeof(void*)));
      }
      break;
    case SYS_READ:
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      else if (!is_user_vaddr((void*)(f->esp + (sizeof(void*) * 2)))) sysexit(-1);
      else if (!is_user_vaddr((void*)(f->esp + (sizeof(void*) * 3)))) sysexit(-1);
      else{
        if (*(int*)((f->esp + sizeof(void*)))) f->eax = -1;
	else{
          int i;
	  for (i = 0; i < *(int*)(f->esp + (sizeof(void) * 3)); i++)
            if (input_getc() == '\0') break;
	  f->eax = i;
	}
      }
      break;
    case SYS_WRITE:
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      else if (!is_user_vaddr((void*)(f->esp + (sizeof(void*) * 2)))) sysexit(-1);
      else if (!is_user_vaddr((void*)(f->esp + (sizeof(void*) * 3)))) sysexit(-1);
      else{
        if (*(int*)(f->esp + sizeof(void*)) == 1){
          putbuf((void*)*(int*)((f->esp + (sizeof(void*) * 2))), *(int*)((f->esp + (sizeof(void*) * 3))));
	  f->eax = *(int*)((f->esp + (sizeof(void*) * 3)));
	}
	else f->eax = -1;
      }
      break;
    case SYS_FIB:
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      else{
        int num = (int)*(int*)(f->esp + sizeof(void*));
	int ans, prev = 1;
	for (int i = 1; i <= num; i++){
          if (i == 1 || i == 2) ans = 1;
	  else{
            ans += prev;
	    prev = ans - prev;
	  }
	}
        f->eax = ans;
      }
      break;
    case SYS_MAX:
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      else if (!is_user_vaddr((void*)(f->esp + (sizeof(void*) * 2)))) sysexit(-1);
      else if (!is_user_vaddr((void*)(f->esp + (sizeof(void*) * 3)))) sysexit(-1);
      else if (!is_user_vaddr((void*)(f->esp + (sizeof(void*) * 4)))) sysexit(-1);
      else{
        int a = (int)*(int*)(f->esp + sizeof(void*));
	int b = (int)*(int*)(f->esp + (sizeof(void*) * 2));
	int c = (int)*(int*)(f->esp + (sizeof(void*) * 3));
	int d = (int)*(int*)(f->esp + (sizeof(void*) * 4));
	int max = a;
	if (b > max) max = b;
	if (c > max) max = c;
	if (d > max) max = d;
	f->eax = max;
      }
      break;
    default :
      thread_exit();
      break;
  }
  //printf ("system call!\n");
  //thread_exit ();
}
