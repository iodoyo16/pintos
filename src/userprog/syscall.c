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
void halt(void){
  shutdown_power_off();
}
void exit(int status){
  struct thread* t=thread_current();
  t->exit_status=status;
  printf("%s: exit(%d)\n",thread_name(),status);
  thread_exit();
}
pid_t exec(const char *cmd_line){
  return process_execute(cmd_line);
}
int wait(pid_t pid){
  return process_wait(pid);
}
int read(int fd, void* buffer, unsigned size){
  unsigned i=0;
  if(fd==0){
    for(i=0;i<size;i++){
      if(((char*)buffer)[i]=='\0')
        break;
    }
  }
  return i;
}
int write(int fd, const void *buffer, unsigned size){
  if(fd==1){
    putbuf(buffer, size);
    return size;
  }
  return -1;
}
static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  //printf("esp: %x\n",(uint32_t *)(f->esp));
  //printf("syscall num : %d\n",*(uint32_t *)(f->esp));
  //hex_dump(f->esp,f->esp,100,1);
  switch(*((uint32_t *)(f->esp))){
    case SYS_HALT:                   /* Halt the operating system. */
      halt();
    break;
    case SYS_EXIT:                   /* Terminate this process. */
      // void exit (int status)
      if(!is_user_vaddr(f->esp+4)){
        exit(-1);
      }
      exit(*(uint32_t *)(f->esp + 4));
    break;
    case SYS_EXEC:                   /* Start another process. */
      // pit_d exec(const char *cmd_line);
      if(!is_user_vaddr(f->esp+4)){
        exit(-1);
      }
      f->eax=exec((const char*)*(uint32_t *)(f->esp +4));
    break;
    case SYS_WAIT:                   /* Wait for a child process to die. */
      // int wait(pit_d pid);
      if(!is_user_vaddr(f->esp+4)){
        exit(-1);
      }
      f->eax=wait((pid_t)*(uint32_t *)(f->esp + 4));
    break;
    case SYS_CREATE:                 /* Create a file. */
    // bool create(const char *file, unsigned initial_size);
    break;
    case SYS_REMOVE:                 /* Delete a file. */
    // bool remove (const char *file);
    break;
    case SYS_OPEN:                   /* Open a file. */
    // int open (const char *file);
    break;
    case SYS_FILESIZE:               /* Obtain a file's size. */
    // int filesize (int fd);
    break;
    case SYS_READ:                   /* Read from a file. */
    // int read(int fd, void *buffer, unsigned size);
      if(!is_user_vaddr(f->esp+4)||!is_user_vaddr(f->esp+8)||!is_user_vaddr(f->esp+12)){
        exit(-1);
      }
      f->eax=read(((int)*(uint32_t *)(f->esp + 4)), (void *)(*((uint32_t *)(f->esp +8))), (unsigned)*((uint32_t *)(f->esp+12)));
    break;
    case SYS_WRITE:                  /* Write to a file. */
    // int write (int fd, const void *buffer, unsigned size);
      if(!is_user_vaddr(f->esp+4)||!is_user_vaddr(f->esp+8)||!is_user_vaddr(f->esp+12)){
          exit(-1);
      }
      f->eax=write(((int)*(uint32_t *)(f->esp + 4)),(void *)(*((uint32_t *)(f->esp + 8))), (unsigned)*((uint32_t *)(f->esp+12)));
    break;
    case SYS_SEEK:                   /* Change position in a file. */
    // void seek(int fd,unsigned position);
    break;
    case SYS_TELL:                   /* Report current position in a file. */
    // unsigned tell(int fd);
    break;
    case SYS_CLOSE:                  /* Close a file. */
    // void close(int fd);
    break;
  }
  //printf ("system call!\n");
}

