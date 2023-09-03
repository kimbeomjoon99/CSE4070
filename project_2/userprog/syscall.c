#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

void sysexit(int s){
	struct thread *cur = thread_current();
	cur->exit_status = s;
	// prj 2
	for (int i = 3; i < 128; i++){
		if (cur->fd[i]){
			file_close(cur->fd[i]);
			cur->fd[i] = NULL;
		}
	}
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
  //lock_init(&flock);
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
      if (!is_user_vaddr((void*)(f->esp + (sizeof(void*) * 2)))) sysexit(-1);
      else if (!is_user_vaddr((void*)(f->esp + (sizeof(void*) * 3)))) sysexit(-1);
      else{
        struct thread *cur = thread_current();
        lock_acquire(&flock);
        if (*(int*)((f->esp + sizeof(void*))) >= 3){ //f->eax = -1;
          if (!cur->fd[((int)*(uint32_t*)(f->esp + sizeof(void*)))]) sysexit(-1);
          f->eax = file_read(cur->fd[(int)*(uint32_t*)(f->esp + sizeof(void*))], (void*)(f->esp + (sizeof(void*) * 2)), (int)*(uint32_t*)(f->esp + (sizeof(void*) * 3)));
        }
	else if (!(*(int*)(f->esp + sizeof(void*)))){ //else
          int i;
	  for (i = 0; i < *(int*)(f->esp + (sizeof(void) * 3)); i++)
            if (input_getc() == '\0') break;
	  f->eax = i;
	}
        else f->eax = -1;
        lock_release(&flock);
      }
      break;
    case SYS_WRITE:
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      if (!is_user_vaddr((void*)(f->esp + (sizeof(void*) * 2)))) sysexit(-1);
      else if (!is_user_vaddr((void*)(f->esp + (sizeof(void*) * 3)))) sysexit(-1);
      else{
        struct thread *cur = thread_current();
        lock_acquire(&flock);
        if (*(int*)(f->esp + sizeof(void*)) == 1){
          putbuf((void*)*(int*)((f->esp + (sizeof(void*) * 2))), *(int*)((f->esp + (sizeof(void*) * 3))));
	  f->eax = *(int*)((f->esp + (sizeof(void*) * 3)));
	}
        else if (*(int*)(f->esp + sizeof(void*)) >= 3){
          //file_deny_write(cur->fd[(int)*(uint32_t*)(f->esp + sizeof(void*))]);
          f->eax = file_write(cur->fd[(int)*(uint32_t*)(f->esp + sizeof(void*))], (void*)(f->esp + (sizeof(void*) * 2)), (int)*(uint32_t*)(f->esp + (sizeof(void*) * 3)));
	  //file_allow_write(cur->fd[(int)*(uint32_t*)(f->esp + sizeof(void*))]);
        }
	else f->eax = -1;
        lock_release(&flock);
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
    // prj 2
    case SYS_CREATE:
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      else if (!is_user_vaddr((void*)(f->esp + (sizeof(void*) * 2)))) sysexit(-1);
      else if (!(char*)(*(uint32_t*)(f->esp + sizeof(void*)))) sysexit(-1);
      else{
        f->eax = filesys_create((char*)(*(uint32_t*)(f->esp + sizeof(void*))), (int)*(int*)(f->esp + (sizeof(void*) * 2)));
      }
      break;
    case SYS_REMOVE:
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      else if (!(char*)(*(uint32_t*)(f->esp + sizeof(void*)))) sysexit(-1);
      else{
        f->eax = filesys_remove((char*)(*(uint32_t*)(f->esp + sizeof(void*))));
      }
      break;
    case SYS_OPEN:  // lock_release loc changed
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      else if (!(char*)*(uint32_t*)(f->esp + sizeof(void*))) sysexit(-1);
      else{
        lock_acquire(&flock);
        struct file *o_file = filesys_open((char*)*(uint32_t*)(f->esp + sizeof(void*)));
        if (!o_file){	
          f->eax = -1;
        }
        else{
          int flag = 0;
          struct thread *cur = thread_current();
          for (int i = 3; i < 128; i++){
            if (!cur->fd[i]){
              cur->fd[i] = o_file;
              f->eax = i;
              flag++;
              break;
            }
          }
          if (!flag){ 
            f->eax = -1;
	  }
        }
        lock_release(&flock);
      }
      break;
    case SYS_CLOSE:
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      else{
        struct thread *cur = thread_current();
        if (!cur->fd[(int)*(uint32_t*)(f->esp + sizeof(void*))]) sysexit(-1);
        else{
          file_close(cur->fd[(int)*(uint32_t*)(f->esp + sizeof(void*))]);
          cur->fd[(int)*(uint32_t*)(f->esp + sizeof(void*))] = NULL;
        }
      }
      break;
    case SYS_FILESIZE:
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      else{
        struct thread *cur = thread_current();
        if (!cur->fd[(int)*(uint32_t*)(f->esp + sizeof(void*))]) sysexit(-1);
        else{
          f->eax = file_length(cur->fd[(int)*(uint32_t*)(f->esp + sizeof(void*))]);
        }
      }
      break;
    case SYS_SEEK:
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      else if (!is_user_vaddr((void*)(f->esp + (sizeof(void) * 2)))) sysexit(-1);
      else{
        struct thread *cur = thread_current();
        if (!cur->fd[(int)*(uint32_t*)(f->esp + sizeof(void*))]) sysexit(-1);
        else{
          file_seek(cur->fd[(int)*(uint32_t*)(f->esp + sizeof(void*))], (int)(*(int*)(f->esp + (sizeof(void*) * 2))));
        }
      }
      break;
    case SYS_TELL:
      if (!is_user_vaddr((void*)(f->esp + sizeof(void*)))) sysexit(-1);
      else{
        struct thread *cur = thread_current();
        if (!cur->fd[(int)*(uint32_t*)(f->esp + sizeof(void*))]) sysexit(-1);
        else{
          f->eax = file_tell(cur->fd[(int)*(uint32_t*)(f->esp + sizeof(void*))]);
        }
      }
      break;
    
    default :
      thread_exit();
      break;
  }
  //printf ("system call!\n");
  //thread_exit ();
}
