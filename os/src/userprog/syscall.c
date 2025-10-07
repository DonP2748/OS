#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

/*DonP sign*/
#include "devices/input.h"
#include "devices/shutdown.h"
#include "threads/vaddr.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "userprog/process.h"
#include "userprog/pagedir.h"

static void sys_halt(void);
static void sys_exit(int status);
static tid_t sys_exe(const char* path);
static int sys_wait(tid_t id);
static bool sys_create(const char* path, unsigned int initial_size);
static bool sys_remove(const char* path);
static int sys_open(const char* path);
static int sys_filesize(int fd);
static int sys_read(int fd, void* buffer, unsigned int size);
static int sys_write(int fd, const void* buffer, unsigned int size);
static void sys_seek(int fd, unsigned int position);
static unsigned int sys_tell(int fd);
static void sys_close(int fd);
//helper func
static void validate_user_ptr(void* addr);
static struct file* get_file_by_fd(int fd);
static int install_fd(struct file* f);
static void remove_file_by_fd(int fd);

static void syscall_handler (struct intr_frame *);


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  /*DonP sign*/
  validate_user_ptr(f->esp);
  //find the syscall number in the first 4 bytes on stack
  int syscall_num = *(int *) f->esp;

  switch (syscall_num){
    case SYS_HALT:
    {
      sys_halt();
      break;
    }
    case SYS_EXIT:
    {
      //must cast int since stack is aligned 4 bytes while void* type + 1 is just addr + 1 (or error)
      validate_user_ptr((int *)f->esp + 1); //check valid before dereference
      int status = *((int *) f->esp + 1); //status return from main
      sys_exit(status);
      break;
    }
    case SYS_EXEC:
    {
      validate_user_ptr((char **)f->esp + 1);
      validate_user_ptr(*((char **)f->esp + 1));
      const char* path = *((char **) f->esp + 1);
      f->eax = sys_exe(path);
      break;
    }
    case SYS_WAIT:
    {  
      validate_user_ptr((int *)f->esp + 1);
      tid_t id = *((int *) f->esp + 1);
      f->eax = sys_wait(id);
      break;
    }
    case SYS_CREATE:
    {
      validate_user_ptr((char **)f->esp + 1); //check the arg's addr 
      validate_user_ptr(*((char **)f->esp + 1)); //check the buffer's addr which arg point to
      validate_user_ptr((unsigned *)f->esp + 2);
      const char* path = *((char **) f->esp + 1);
      unsigned size = *((unsigned *) f->esp + 2);
      f->eax = sys_create(path, size);
      break; 
    }
    case SYS_REMOVE:
    {
      validate_user_ptr((char **)f->esp + 1);
      validate_user_ptr(*((char **)f->esp + 1));
      const char* path = *((char **) f->esp + 1);
      f->eax = sys_remove(path);
      break;
    }
    case SYS_OPEN:
    {
      validate_user_ptr((char **)f->esp + 1);
      validate_user_ptr(*((char **)f->esp + 1));
      const char* path = *((char **) f->esp + 1);
      f->eax = sys_open(path);
      break;
    }
    case SYS_FILESIZE:
    {
      validate_user_ptr((int *)f->esp + 1);
      int fd = *((int *) f->esp + 1);
      f->eax = sys_filesize(fd);
      break;
    }
    case SYS_READ:
    {
      validate_user_ptr((int *)f->esp + 1);
      validate_user_ptr((void **)f->esp + 2);
      validate_user_ptr(*((void **)f->esp + 2));
      validate_user_ptr((unsigned *)f->esp + 3);
      int fd = *((int *) f->esp + 1);
      void *buffer = *((void **) f->esp + 2);
      unsigned size = *((unsigned *) f->esp + 3);
      f->eax = sys_read(fd, buffer, size);      
      break;
    }
    case SYS_WRITE:
    {
      validate_user_ptr((int *)f->esp + 1);
      validate_user_ptr((void **)f->esp + 2);
      validate_user_ptr(*((void **)f->esp + 2));
      validate_user_ptr((unsigned *)f->esp + 3);
      int fd = *((int *) f->esp + 1);
      const void *buffer = *((void **) f->esp + 2);
      unsigned size = *((unsigned *) f->esp + 3);
      f->eax = sys_write(fd, buffer, size);  // return value in eax
      break;
    }
    case SYS_SEEK:
    {
      validate_user_ptr((int *)f->esp + 1);
      validate_user_ptr((unsigned *)f->esp + 2);
      int fd = *((int *) f->esp + 1);
      unsigned size = *((unsigned *) f->esp + 2);
      sys_seek(fd, size);
      break;
    }
    case SYS_TELL:
    {
      validate_user_ptr((int *)f->esp + 1);
      int fd = *((int *) f->esp + 1);
      f->eax = sys_tell(fd);
      break;
    }
    case SYS_CLOSE:
    {
      validate_user_ptr((int *)f->esp + 1);
      int fd = *((int *) f->esp + 1);
      sys_close(fd);
      break;
    }
    case SYS_MMAP:
      break;
    case SYS_MUNMAP:
      break;
    case SYS_CHDIR:
      break;
    case SYS_MKDIR:
      break;
    case SYS_READDIR:
      break;
    case SYS_ISDIR:
      break;
    case SYS_INUMBER:
      break;

    default:
    { 
      sys_exit(-1);
      break;
    }
  }

}

static void sys_halt(void)
{
  shutdown_power_off();
}


static void sys_exit(int status)
{
  struct thread* cur = thread_current();
  printf("%s: exit(%d)\n", thread_name(), status);
  cur->exit_status = status;
  thread_exit();
}

static tid_t sys_exe(const char* path)
{
  return process_execute(path);
}

static int sys_wait(tid_t id)
{
  return process_wait(id);
}

static bool sys_create(const char* path, unsigned int initial_size)
{
  return filesys_create(path, initial_size);
}

static bool sys_remove(const char* path)
{
  return filesys_remove(path);
}

static int sys_open(const char* path)
{

  struct file *f = filesys_open(path); // uses directory/inode

  if(!f){
    return -1;
  } 
  //assign new refer to fd table
  int fd = install_fd(f);
  if (fd < 0) { //out of fd slot
    file_close(f);
    return -1;
  }
  return fd;
}


static int sys_filesize(int fd)
{
  struct file* _file = get_file_by_fd(fd);
  if(_file){
    return file_length(_file);
  }
  return -1;
}


static int sys_read(int fd, void* buffer, unsigned int size)
{
  if(fd == 0){
    // Read from keyboard (stdin)
    uint8_t *buf = (uint8_t*)buffer;
    for (unsigned int i = 0; i < size; i++) {
      buf[i] = input_getc();  //get one character from keyboard
    }
    return size;
  }
  else if(fd == 1 || fd == 2){
    return -1;
  }
  else{
    struct file* _file = get_file_by_fd(fd);
    if(!_file){
      return -1;
    }
    return file_read(_file, buffer, size);
  }
  return -1;
}

static int sys_write(int fd, const void* buffer, unsigned int size)
{ 
  //check fd valid  
  if(fd == 0){
    return -1;
  }
  else if(fd == 1 || fd == 2){ //
    //the data has been aready interpreted from user vprintf
    putbuf(buffer, size);
    return size;
  }
  else{ //file
    struct file* _file = get_file_by_fd(fd);
    if(!_file){
      return -1;
    }
    return file_write(_file, buffer, size);
  }
  return -1;
}

static void sys_seek(int fd, unsigned int position)
{
  struct file* _file = get_file_by_fd(fd);
  if(_file){
    file_seek(_file, position);
  }
}

static unsigned int sys_tell(int fd)
{
  struct file* _file = get_file_by_fd(fd);
  if(_file){
    return file_tell(_file);
  }
  return -1;
}


static void sys_close(int fd)
{
  struct file* _file = get_file_by_fd(fd);
  if(_file){
    file_close(_file);
    remove_file_by_fd(fd); //avoid dangling pointer
  }
  else{
    sys_exit(-1);
  }
}











//helper func

//check user addr valid or not
static void validate_user_ptr(void* addr)
{
  struct thread* cur = thread_current();
  //check NULL, virtual address and if the vaddr is mapped
  if (addr == NULL ){
    sys_exit(-1);
    return;
  }
  //handle sc-boundary-3 test case
  for(unsigned i = 0; i < sizeof(void*); i++){
    if(!is_user_vaddr(addr + i) || pagedir_get_page(cur->pagedir, addr + i) == NULL ){
      sys_exit(-1);
      return;
    } 
  } 
}


static int install_fd(struct file* f)
{
  struct thread* cur = thread_current();
  for(int fd = 3; fd < FD_MAX; fd++){
    if(cur->fd_table[fd] == NULL){
      cur->fd_table[fd] = f;
      return fd;
    }
  }
  return -1;
}

static struct file* get_file_by_fd(int fd)
{
  struct thread *t = thread_current ();
  if (fd < 0 || fd >= FD_MAX)
    return NULL;
  return t->fd_table[fd]; /* may be NULL */
}

static void remove_file_by_fd(int fd)
{
  struct thread *t = thread_current ();
  if (fd < 0 || fd >= FD_MAX){
    return;
  }
  t->fd_table[fd] = NULL; 
}