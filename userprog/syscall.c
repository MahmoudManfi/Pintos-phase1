#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static struct lock files_sys_lock;               /* lock for syschronization between files */

static void syscall_handler (struct intr_frame *);

void halt_wrapper();
void exit_wrapper();
void exec_wrapper();
void wait_wrapper();
void create_wrapper();
void remove_wrapper();
void open_wrapper();
void filesize_wrapper();
void read_wrapper();
void write_wrapper();
void seek_wrapper();
void tell_wrapper();
void close_wrapper();


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  
  void *esp = f->esp;

  switch (get_int(esp))
  {
  case SYS_HALT:
    halt_wrapper();
    break;
  
  case SYS_EXIT:
    exit_wrapper();
    break;

  case SYS_EXEC:
    exec_wrapper();
    break;

  case SYS_WAIT:
    wait_wrapper();
    break;

  case SYS_CREATE:
    create_wrapper();
    break;

  case SYS_REMOVE:
    remove_wrapper();
    break;

  case SYS_OPEN:
    open_wrapper();
    break;

  case SYS_FILESIZE:
    filesize_wrapper();
    break;

  case SYS_READ:
    read_wrapper();
    break;

  case SYS_WRITE:
    write_wrapper();
    break;

  case SYS_SEEK:
    seek_wrapper();
    break;

  case SYS_TELL:
    tell_wrapper();
    break;

  case SYS_CLOSE:
    close_wrapper();
    break;

  default:
    break;
  }

  thread_exit ();
}

int 
get_int(int** esp)
{

}

char* 
get_char_ptr(char*** esp) 
{

}

void* 
get_void_ptr(void*** esp)
{

}

void 
validate_void_ptr(const void* pt)
{

}

void
halt_wrapper()
{

}

void
exit_wrapper()
{
  
}

void
exec_wrapper()
{
  
}

void
wait_wrapper()
{
  
}

void
create_wrapper()
{
  
}

void
remove_wrapper()
{
  
}

void
open_wrapper()
{
  
}

void
filesize_wrapper()
{
  
}

void
read_wrapper()
{
  
}

void
write_wrapper()
{
  
}

void
seek_wrapper()
{
  
}

void
tell_wrapper()
{
  
}

void
close_wrapper()
{
  
}