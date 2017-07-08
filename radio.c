#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PERIPHERY     0x3F200000 //perifērijas adrešu sākums
#define CLOCK_CONTROL 0x3F101070 //pulksteņa kontroles adrešu sākums
#define BLOCK_SIZE (4*1024)
#define BLOCK_SIZE_CLOCK (4*1024)


int main(){
 int mem_fd;
 volatile unsigned  *gpio; //nomapotā perifērija
 volatile unsigned  *clock_ctl; //nomapotais pulkstenis
 unsigned int divider; //pulksteņa dalītājs. frekvence ir PLLfrekvence(500Mhz)/dalītājs

 divider=100; 
  
 if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
	{
	printf("can't open /dev/mem. root much?\n");
	exit(-1);
   }
   
 gpio = mmap(
  NULL,             //Any adddress in our space will do
  BLOCK_SIZE,       //Map length
  PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
  MAP_SHARED,       //Shared with other processes
  mem_fd,           //File to map
  PERIPHERY         //Offset to GPIO peripheral
  );
 
 clock_ctl = mmap(
  NULL,             //Any adddress in our space will do
  BLOCK_SIZE_CLOCK,  //Map length
  PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
  MAP_SHARED,       //Shared with other processes
  mem_fd,           //File to map
  CLOCK_CONTROL     //Offset to clock control
  );

 
 close(mem_fd);
 printf("%p\n", gpio); 
 printf("%p\n", clock_ctl); 

 gpio[0x0] &= ~(7<<12); //set pin 4 as input
 gpio[0x1C/4] |= (1 << 4);//pin 4 on. Adrese ir (fiziskā adrese/4 dēļ pointeru aritmētikas)
 //gpio[0x0] |= (1 << 12); //set pin 4 as output
 gpio[0x0] |= (1 << 14); //set pin 4 as ALT0 function (clock)

 clock_ctl[0x0] |= (0x5a << 24); //set "clock managers password" for control
 clock_ctl[0x0] |= (6 << 0); //clock source 6 (500 mhz pll)
 clock_ctl[0x1] |= (0x5a << 24); //set "clock managers password" for dividers
 clock_ctl[0x1] |= (divider << 12); 
 clock_ctl[0x0] |= (1 << 4); //enable clock 
 
 

 return 0; //make -Wall happy
}
