#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static struct lock files_sys_lock;               /* lock for syschronization between files */

static void syscall_handler (struct intr_frame *);

void halt_wrapper(void* esp);
void exit_wrapper(void* esp);
void exec_wrapper(void* esp);
void wait_wrapper(void* esp);
void create_wrapper(void* esp);
void remove_wrapper(void* esp);
void open_wrapper(void* esp);
void filesize_wrapper(void* esp);
void read_wrapper(void* esp);
void write_wrapper(void* esp);
void seek_wrapper(void* esp);
void tell_wrapper(void* esp);
void close_wrapper(void* esp);


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");

  lock_init(&files_sys_lock);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  
  void* esp = f->esp;

  switch (get_int(esp))
  {
  case SYS_HALT:
    halt_wrapper(esp);
    break;
  
  case SYS_EXIT:
    exit_wrapper(esp);
    break;

  case SYS_EXEC:
    exec_wrapper(esp);
    break;

  case SYS_WAIT:
    wait_wrapper(esp);
    break;

  case SYS_CREATE:
    create_wrapper(esp);
    break;

  case SYS_REMOVE:
    remove_wrapper(esp);
    break;

  case SYS_OPEN:
    open_wrapper(esp);
    break;

  case SYS_FILESIZE:
    filesize_wrapper(esp);
    break;

  case SYS_READ:
    read_wrapper(esp);
    break;

  case SYS_WRITE:
    write_wrapper(esp);
    break;

  case SYS_SEEK:
    seek_wrapper(esp);
    break;

  case SYS_TELL:
    tell_wrapper(esp);
    break;

  case SYS_CLOSE:
    close_wrapper(esp);
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
halt()
{

}

void
halt_wrapper(void* esp)
{

}

void
exit()
{

}

void
exit_wrapper(void* esp)
{
  
}

void
exec()
{

}

void
exec_wrapper(void* esp)
{
  
}

void
wait()
{

}

void
wait_wrapper(void* esp)
{
  
}

void
create()
{

}

void
create_wrapper(void* esp)
{
  
}

void
remove()
{

}

void
remove_wrapper(void* esp)
{
  
}

void
open()
{

}

void
open_wrapper(void* esp)
{
  
}

void
filesize()
{

}

void
filesize_wrapper(void* esp)
{
  
}

void
read()
{

}

void
read_wrapper(void* esp)
{
  
}

void
write()
{

}

void
write_wrapper(void* esp)
{
  
}

void
seek()
{

}

void
seek_wrapper(void* esp)
{
  
}

void
tell()
{

}

void
tell_wrapper(void* esp)
{
  
}

void
close()
{

}

void
close_wrapper(void* esp)
{
  
}