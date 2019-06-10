#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>

int     memfd;
void    *mapped_reg_base;
void    *mapped_ddr_base;
void    *mapped_inst_base;

#undef readl
#define readl(addr) \
    ({ unsigned int __v = (*(volatile unsigned int *) (addr)); __v; })

#undef writel
#define writel(addr,b) (void)((*(volatile unsigned int *) (addr)) = (b))



#define REG_BASE_ADDRESS     0x8F000000
#define DDR_BASE_ADDRESS     0x60000000
#define INST_BASE_ADDRESS     0x6D000000


void *memory_map(unsigned int map_size, off_t base_addr) //map_size = n MByte
{
    void *mapped_base;
    mapped_base = mmap(0, map_size*1024*1024, PROT_READ | PROT_WRITE, MAP_SHARED
, memfd, base_addr);
    if (mapped_base == (void *) -1) {
        printf("Can't map memory to user space.\n");
        exit(0);
    }
#ifdef DEBUG
    printf("Memory mapped at address %p.\n", mapped_base);
#endif
    return mapped_base;
}


void memory_unmap(unsigned int map_size, void *mapped_base)
{
    if (munmap(mapped_base, map_size*1024*1024) == -1) {
        printf("Can't unmap memory from user space.\n");
        exit(0);
    }
}


int load_bin(char *path, void* offset, size_t size)
{
    FILE *pb_in=fopen(path,"r");
    printf("3\n");
    unsigned int t=0;
    int addr=0;
    int numread=0;
    printf("3\n");

    if(pb_in == NULL){
        printf("Err: open %s: %s\n",__func__, path);
        return -1;
    }
    printf("3\n");

#ifdef DEBUG
    printf("loading %s @0x%x size %d, expected %d, file size:%d\n", path, (int)offset,
        (int)(size),(int)get_file_size(path) *4/9, (int)get_file_size(path));
#endif
    printf("3\n");

    if (fread(offset, sizeof(char), size, pb_in) != size){
        printf("Err: fread error, actually read: %d\n",numread);
        //mem_show(offset, 64);
        }

    printf("3\n");
    fclose(pb_in);
    printf("3\n");
    return addr;
}

static void timespec_sub(struct timespec *t1, const struct timespec *t2)
{
  assert(t1->tv_nsec >= 0);
  assert(t1->tv_nsec < 1000000000);
  assert(t2->tv_nsec >= 0);
  assert(t2->tv_nsec < 1000000000);
  t1->tv_sec -= t2->tv_sec;
  t1->tv_nsec -= t2->tv_nsec;
  if (t1->tv_nsec >= 1000000000)
  {
    t1->tv_sec++;
    t1->tv_nsec -= 1000000000;
  }
  else if (t1->tv_nsec < 0)
  {
    t1->tv_sec--;
    t1->tv_nsec += 1000000000;
  }
}


int dump(char *path, void* from, size_t size) // legacy
{
    FILE *pb_out;
    unsigned int t=0;
    int addr=0;
    pb_out=fopen(path,"wb");

    if(pb_out==NULL){
        printf("dump_ddr:open file error\n");
            return 1;
        }
    
    fwrite(from, 1, size, pb_out);


    fclose(pb_out);
    return addr;
}


int init_fpga(int type){
    if (type==0){
        printf("2\n");
        //load_bin("./param/conv1_1_weights.bin",     mapped_ddr_base, 32);
        //load_bin("./weight/concat_svd_weight.bin",     mapped_ddr_base, 0x43f40);
        load_bin("./weight/concat_svd_weight.bin",     mapped_ddr_base, 23859120);
        
        printf("2\n");
        //load_bin("./param//instr.bin",     mapped_inst_base , 32);
        //load_bin("./param/instr_99116_tail.bin",     mapped_inst_base , 0x18330);
        load_bin("./weight/concat_svd_instr.bin",     mapped_inst_base , 257644);
    }
    
    if (type==1){
        printf("2\n");
        //load_bin("./param//instr.bin",     mapped_inst_base , 32);
        //load_bin("./param/data_147456_tail.bin",     mapped_ddr_base + (0x6A000000 -  0x60000000 ) + 96264 , 0x24000);
        load_bin("./data/input_1.bin",     mapped_ddr_base + (0x6A000000 -  0x60000000 ) + 0 , 0x24c00);
     
        printf("2\n");
        writel(mapped_reg_base+0x200,0x07070f0f);
        printf("2\n");
        writel(mapped_reg_base+0x004,0x00000000);
        writel(mapped_reg_base+0x004,0x00000001);

        ////// Bank Init /////////
        writel(mapped_reg_base+0x224,0x60000000);
        writel(mapped_reg_base+0x228,0x00000000);
        writel(mapped_reg_base+0x22C,0x6A000000);
        writel(mapped_reg_base+0x230,0x00000000);
        writel(mapped_reg_base+0x234,0x6C000000);
        writel(mapped_reg_base+0x238,0x00000000);
        writel(mapped_reg_base+0x23C,0x6C000000);
        writel(mapped_reg_base+0x240,0x00000000);
        writel(mapped_reg_base+0x244,0x6C200000);
        writel(mapped_reg_base+0x248,0x00000000);
        writel(mapped_reg_base+0x24C,0x6C400000);
        writel(mapped_reg_base+0x250,0x00000000);
        writel(mapped_reg_base+0x254,0x6C600000);
        writel(mapped_reg_base+0x258,0x00000000);
        writel(mapped_reg_base+0x25C,0x6C800000);
        writel(mapped_reg_base+0x260,0x00000000);

        /////// ADDR init /////////
        writel(mapped_reg_base+0x20C,0x0006d000);
        writel(mapped_reg_base+0x210,0x0006f000);
        
        ////// RUN !!!!/////
        writel(mapped_reg_base+0x21C,1);
        writel(mapped_reg_base+0x220,1);

        ////// WAIT //////
        while(! readl(mapped_reg_base+0x608) ) {
            // std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        ////// CLAER /////
        writel(mapped_reg_base+0x21C,0);
        writel(mapped_reg_base+0x220,0);
        writel(mapped_reg_base+0x60c,1);
        writel(mapped_reg_base+0x60c,0);
    }


    if (type==2){
        printf("2\n");
        //load_bin("./param//instr.bin",     mapped_inst_base , 32);
        //load_bin("./param/data_147456_tail.bin",     mapped_ddr_base + (0x6A000000 -  0x60000000 ) + 96264 , 0x24000);
        load_bin("./data/input_2.bin",     mapped_ddr_base + (0x6A000000 -  0x60000000 ) + 0 , 0x24c00);
     
        printf("2\n");
        writel(mapped_reg_base+0x200,0x07070f0f);
        printf("2\n");
        writel(mapped_reg_base+0x004,0x00000000);
        writel(mapped_reg_base+0x004,0x00000001);

        ////// Bank Init /////////
        writel(mapped_reg_base+0x224,0x60000000);
        writel(mapped_reg_base+0x228,0x00000000);
        writel(mapped_reg_base+0x22C,0x6A000000);
        writel(mapped_reg_base+0x230,0x00000000);
        writel(mapped_reg_base+0x234,0x6C000000);
        writel(mapped_reg_base+0x238,0x00000000);
        writel(mapped_reg_base+0x23C,0x6C000000);
        writel(mapped_reg_base+0x240,0x00000000);
        writel(mapped_reg_base+0x244,0x6C200000);
        writel(mapped_reg_base+0x248,0x00000000);
        writel(mapped_reg_base+0x24C,0x6C400000);
        writel(mapped_reg_base+0x250,0x00000000);
        writel(mapped_reg_base+0x254,0x6C600000);
        writel(mapped_reg_base+0x258,0x00000000);
        writel(mapped_reg_base+0x25C,0x6C800000);
        writel(mapped_reg_base+0x260,0x00000000);

        /////// ADDR init /////////
        writel(mapped_reg_base+0x20C,0x0006d000);
        writel(mapped_reg_base+0x210,0x0006f000);
        
        ////// RUN !!!!/////
        writel(mapped_reg_base+0x21C,1);
        writel(mapped_reg_base+0x220,1);

        ////// WAIT //////
        while(! readl(mapped_reg_base+0x608) ) {
            // std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        ////// CLAER /////
        writel(mapped_reg_base+0x21C,0);
        writel(mapped_reg_base+0x220,0);
        writel(mapped_reg_base+0x60c,1);
        writel(mapped_reg_base+0x60c,0);
    }
	return 0;
}

int main(){
  off_t   reg_base = REG_BASE_ADDRESS;
  off_t   ddr_base = DDR_BASE_ADDRESS;
  off_t   inst_base = INST_BASE_ADDRESS;    
    
    
  struct timespec ts_start, ts_end;

  int rc;
  printf("1\n");
  memfd = open("/dev/mem", O_RDWR | O_SYNC);
  printf("1\n");
  mapped_reg_base = memory_map(1,reg_base);
  printf("1\n");
  mapped_ddr_base = memory_map(1024,ddr_base);
  printf("1\n");
  mapped_inst_base = memory_map(1024,inst_base);
  printf("1\n");

  init_fpga(0);
  init_fpga(1);
  dump("./data/out_1.bin",mapped_ddr_base + (0x6A000000 -  0x60000000 ) + 4608 , 4096);
  init_fpga(2);
  dump("./data/out_2.bin",mapped_ddr_base + (0x6A000000 -  0x60000000 ) + 4608 , 4096);

//   rc = clock_gettime(CLOCK_MONOTONIC, &ts_start);
//   while(!readl(mapped_reg_base+0x0)){
//     usleep(1000);
//   }
//   rc = clock_gettime(CLOCK_MONOTONIC, &ts_end);

//   writel(mapped_reg_base+0x120,0x00000002);
//   timespec_sub(&ts_end, &ts_start);
//   printf("CLOCK_MONOTONIC reports %ld.%09ld seconds t\n",
//     ts_end.tv_sec, ts_end.tv_nsec);
  
  //dump("./data/outdata.bin",mapped_ddr_base+0x10940,0x4000);
  //dump("./data/data_output.bin",mapped_ddr_base + (0x6A000000 -  0x60000000 ) + 95616 , 648);
  
  return 0;
}
