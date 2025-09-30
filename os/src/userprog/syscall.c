#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

/*DonP sign*/
static int sys_write(int fd, const void* buffer, unsigned int size); 


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  // printf ("system call!\n");
  // thread_exit ();

  /*DonP sign*/
  struct thread* cur = thread_current();
  //find the syscall number in the first 4 bytes on stack
  int syscall_num = *(int *) f->esp;

  switch (syscall_num){
    case SYS_WRITE:
      int fd = *((int *) f->esp + 1);
      const void *buffer = *((void **) f->esp + 2);
      unsigned size = *((unsigned *) f->esp + 3);
      f->eax = sys_write(fd, buffer, size);  // return value in eax
      break;
    case SYS_EXIT:
      int status = *((int *) f->esp + 1); //status return from main
      printf("%s: exit(%d)\n", thread_name(), status);
      cur->exit_status = status;
      thread_exit();
      break;
    default:
      break;
  }

}

static int sys_write(int fd, const void* buffer, unsigned int size)
{
  if(fd == 1){
    //the data has been aready interpreted from user vprintf
    putbuf(buffer, size);
    return size;
  }
  return 0;
}


