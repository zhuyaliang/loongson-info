#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>
#include "hardinfo.h"
cpu_info_t cpu_info[] ={
    {"3A5000",      {LS3A5000_VERSION,      0}, "3A5000",   "14nm",  "35 W",  "0 ~ 70", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3A5000LL",    {LS3A5000LL_VERSION,    0}, "3A5000LL", "14nm",  "30 W",  "0 ~ 70", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3A5000M",     {LS3A5000M_VERSION,     0}, "3A5000M",  "14nm",  "18 W",  "0 ~ 70", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3B5000",      {LS3B5000_VERSION,      0}, "3B5000",   "14nm",  "25 W",  "0 ~ 70", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3C5000L",     {LS3C5000L_VERSION,     0}, "3C5000L",  "14nm",  "200 W", "0 ~ 70", "LGA","70x44x4 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3C5000L",     {LS3C5000LL_VERSION,    0}, "3C5000LL", "14nm",  "160 W", "0 ~ 85", "LGA","70x44x4 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3C5000I",     {LS3A5000I_VERSION,     0}, "3A5000I",  "14nm",  "18 W",  "0 ~ 85", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3C5000i",     {LS3A5000i_VERSION,     0}, "3A5000i",  "14nm",  "18 W" , "0 ~ 85", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3A5000BM",    {LS3A5000BM_VERSION,    0}, "3A5000BM", "14nm",  "18 W",  "0 ~ 70", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3A5000HV",    {LS3A5000HV_VERSION,    0}, "3A5000HV", "14nm",  "35 W",  "0 ~ 70", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3A4000",      {LS3A4000_VERSION,      0}, "3A4000",   "28nm",  "35 W",  "0 ~ 70", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "GS464V"},
    {"UNKNOW",      {0,                     0}, "UNKNOW",   "0nm",   "0 W",   "0 ~ 0",  "LGA","0x0x0 mm", "0K",  "0K",  "0K",   "0K",     "UNKNOW"}
};
static char buffer[100];

static void app_free (void* pData)
{
    free(pData);
}

static char* app_system(const char *cmd)
{
    char result[102400] = {0};
    char buf[1024] = {0};
    FILE *fp = NULL;
    char *data=NULL;
    int count=0;

    if( (fp = popen (cmd, "r")) == NULL ) {
        printf ("popen error!\n");
        return NULL;
    }

    while (fgets (buf, sizeof(buf), fp)) {
        strcat (result, buf);
        count++;
        if(count*1024==102400)
        {
            printf ("result buffer is overflow!!!\n");
            break;
        }
    }
    pclose (fp);
    // printf("result: %s\n", result);
    // printf("resultlen: %d\n", strlen(result));
    if(strlen (result)!=0)
    {
        data=malloc(strlen (result));
        if(data==NULL)
            printf ("app_molloc failed!!!\n");
        strcpy (data,result);
    }
    return data;
}

static int read_buffer (int fd, U8 *buf, U32 count, const char *prefix)
{
    int r = 1;
    U32 r2 = 0;

    while (r2 != count && r != 0)
    {
        r = read (fd, buf + r2, count - r2);
        if (r == -1)
        {
            if (errno != EINTR)
            {
                perror(prefix);
                return -1;
            }
        }
        else
            r2 += r;
    }

    if (r2 != count)
    {
        fprintf (stderr, "%s: Unexpected end of file\n", prefix);
        return -1;
    }
    return 0;
}
/*
 * Copy a physical memory chunk into a memory buffer.
 * This function allocates memory.
 */
static void *ls_mem_chunk (U64 base, U32 len, const char *devmem)
{
    void *p;
    int fd;
    struct stat statbuf;
    U64 mmoffset;
    void *mmp;

    if ((fd = open (devmem, O_RDONLY|O_SYNC)) == -1)
    {
        perror(devmem);
        return NULL;
    }

    if ((p = malloc (len)) == NULL)
    {
        perror("malloc");
        goto out;
    }

    if (fstat (fd, &statbuf) == -1)
    {
        fprintf (stderr, "%s: ", devmem);
        perror ("stat");
        goto err_free;
    }

    /*
     * mmap() will fail with SIGBUS if trying to map beyond the end of
     * the file.
     */
    if (S_ISREG (statbuf.st_mode) && base + (U64)(len > statbuf.st_size))
    {
        fprintf (stderr, "mmap: Can't map beyond end of file %s\n",devmem);
        goto err_free;
    }

    mmoffset = base % getpagesize ();
    /*
     * Please note that we don't use mmap() for performance reasons here,
     * but to workaround problems many people encountered when trying
     * to read from /dev/mem using regular read() calls.
     */
    mmp = mmap (NULL, mmoffset + len, PROT_READ, MAP_SHARED, fd, base - mmoffset);
    if (mmp == MAP_FAILED)
        goto try_read;

    memcpy (p, (U8 *)mmp + mmoffset, len);

    if (munmap (mmp, mmoffset + len) == -1)
    {
        fprintf (stderr, "%s: ", devmem);
        perror ("munmap");
    }
    goto out;

try_read:
    if (lseek (fd, base, SEEK_SET) == -1)
    {
        fprintf (stderr, "%s: ", devmem);
        perror ("lseek");
        goto err_free;
    }
    if (read_buffer (fd, p, len, devmem) == 0)
        goto out;

err_free:
    free (p);
    p = NULL;

out:
    if (close(fd) == -1)
        perror(devmem);

    return p;
}
static void read_mm (U64 pbase, unsigned char * datas, int read_cnt) 
{
    void * vaddr = NULL;
    vaddr=ls_mem_chunk (pbase, read_cnt, "/dev/mem");
    if(vaddr!=NULL)
    {
        memcpy (datas,vaddr,read_cnt);
        free (vaddr);
    }
}
static void read_mm_byte (U64 pbase, U8 *datas) 
{
    unsigned char buf[8] ={0};
    read_mm (pbase, buf, 1);
    *datas = (buf[0]<<0);
}
static void read_mm_hword (U64 pbase, U16 *datas) 
{
    unsigned char buf[8] ={0};
    read_mm (pbase, buf, 2);
    *datas = (buf[1]<<8)+(buf[0]<<0);
}
static void read_mm_word (U64 pbase, U32 *datas) 
{
    unsigned char buf[8] ={0};
    read_mm (pbase, buf, 4);
    *datas = (buf[3]<<24)+(buf[2]<<16)+(buf[1]<<8)+(buf[0]<<0);
}

static void read_mm_dword (U64 pbase, U64 *datas) 
{
    unsigned char buf[8] ={0};
    U32 data_h,data_l;
    read_mm (pbase, buf, 8);
    // app_print_data(buf,8);
    data_l = ((buf[0]&0xff)<<0);
    data_l |= ((buf[1]&0xff)<<8);
    data_l |= ((buf[2]&0xff)<<16);
    data_l |= ((buf[3]&0xff)<<24);
    data_h = ((buf[4]&0xff)<<0);
    data_h |= ((buf[5]&0xff)<<8);
    data_h |= ((buf[6]&0xff)<<16);
    data_h |= ((buf[7]&0xff)<<24);
    *datas=data_h;
    *datas=((*datas&0xffffffff)<<32);
    *datas|=data_l;
}
static void app_mm_read (U64 pbase, U64 *datas, int read_cnt) 
{
    switch(read_cnt)
    {
        case 1:
            read_mm_byte (pbase,(U8 *)datas);
            break;
        case 2:
            read_mm_hword (pbase,(U16 *)datas);
            break;
        case 4:
            read_mm_word (pbase,(U32 *)datas);
            break;
        case 8:
            read_mm_dword (pbase,(U64 *)datas);
            break;
        default:
        printf("mm %d is no suport!!!\n",read_cnt);
    }
}

static U8 lscpu_tempdetect (U32 *temp0,U32 *temp1)
{
    U32 Data = 0;
    U16 TempSensorDigital0,TempSensorDigital1; //UINT16
    S8   TempSensorAnalog0,TempSensorAnalog1;       //INT8 (-40 - 125)

    read_mm_word (CPU_TEMP_SAMPLE_BASE +TEMP_SENSOR_VALUE_OFFSET, &Data);
    TempSensorDigital0 = Data & 0xffff;
    TempSensorDigital1 = (Data & (0xffff << 16)) >> 16; //48-32 16
    //Calculate Temp
    TempSensorAnalog0 = TempSensorDigital0 * 731 / 0x4000 - 273 ;
    TempSensorAnalog1 = TempSensorDigital1 * 731 / 0x4000 - 273 ;
    // *(INT8 *)Temperature = (TempSensorAnalog0 + TempSensorAnalog1) / 2;
    *temp0=TempSensorAnalog0;
    *temp1=TempSensorAnalog1;

    return 0;
}
char *get_cpu_name (void)
{
    char *data=NULL;
    char *tmp=NULL;

    data=app_system ("cat /proc/cpuinfo | grep 'model name'");
    // printf("***%s***\n",data);
    if(data==NULL)
    {
        sprintf (buffer,"%s","unknow");
        goto done;
    }
    sprintf (buffer,"%s",strstr (data,":")+1+strspn (strstr(data,":")+1," "));
    // printf("***%s***\n",buffer);
    if((tmp = strstr (buffer, "\n")))
        *tmp = '\0';
done:
    if(data!=NULL)
        app_free (data);
    return buffer;
}
char *get_cpu_version (void)
{
    char *data=NULL;
    char *tmp=NULL;

    data=app_system ("dmidecode -t processor | grep 'Version:'");
    if(data==NULL)
    {
        sprintf (buffer,"%s","unknow");
        goto done;
    }
    sprintf (buffer,"%s",strstr(data,":")+1+strspn(strstr(data,":")+1," "));
    if((tmp = strstr (buffer, "\n")))
        *tmp = '\0';
done:
    if(data!=NULL)
        app_free (data);
    return buffer;
}
char *get_cpu_current_speed (void)
{
    char *data=NULL;
    char *tmp=NULL;

    data=app_system ("cat /proc/cpuinfo | grep 'cpu MHz'");
    if(data==NULL)
    {
        data=app_system ("cat /proc/cpuinfo | grep 'CPU MHz'");
        if(data==NULL)
        {
            sprintf(buffer,"%s","unknow");
            goto done;
        }
    }
    sprintf (buffer,"%s",strstr (data,":")+1+strspn (strstr(data,":")+1," "));
    if((tmp = strstr(buffer, "\n")))
        *tmp = '\0';
done:
    if(data!=NULL)
        app_free (data);
    return buffer;
}
char *get_cpu_max_speed (void)
{
    char *data=NULL;
    char *tmp=NULL;

    data=app_system ("dmidecode -t 4 | grep 'Max Speed:'");
    if(data==NULL)
    {
        sprintf (buffer,"%s","unknow");
        goto done;
    }
    sprintf (buffer,"%s",strstr (data,":")+1+strspn (strstr(data,":")+1," "));
    if((tmp = strstr(buffer, "\n")))
        *tmp = '\0';
done:
    if(data!=NULL)
        app_free (data);
    return buffer;
}
char *get_cpu_min_speed (void)
{
    char *data=NULL;
    char *tmp=NULL;

    data=app_system ("lscpu | grep 'CPU min MHz:'");
    if(data==NULL)
    {
        sprintf (buffer,"%s","unknow");
        goto done;
    }
    sprintf (buffer,"%s",strstr (data,":")+1+strspn (strstr(data,":")+1," "));
    if((tmp = strstr (buffer, "\n")))
        *tmp = '\0';
done:
    if(data!=NULL)
        app_free (data);
    return buffer;
}
char *get_cpu_arch (void)
{
    char *data=NULL;
    char *tmp=NULL;

    data=app_system ("lscpu | grep 'Architecture:'");
    if(data==NULL)
  {
        data=app_system ("lscpu | grep '架构：'");
        if(data==NULL)
        {
            sprintf (buffer,"%s","unknow");
            goto done;
        }
        else
            sprintf (buffer,"%s",strstr (data,"：")+3+strspn (strstr(data,"：")+3," "));
    }
    else
        sprintf (buffer,"%s",strstr (data,":")+1+strspn (strstr (data,":")+1," "));
    if((tmp = strstr (buffer, "\n")))
        *tmp = '\0';
done:
    if(data!=NULL)
        app_free(data);
    return buffer;
}

char *get_cpu_core_num (void)
{
    char *data=NULL;
    char *tmp=NULL;

    data=app_system ("lscpu | grep 'Core(s) per socket:'");
    if(data==NULL)
    {
        data=app_system ("lscpu | grep '每个座的核数：'");
        if(data==NULL)
        {
            sprintf (buffer,"%s","unknow");
            goto done;
        }
        else
            sprintf (buffer,"%s",strstr (data,"：")+3+strspn (strstr (data,"：")+3," "));
    }
    else
        sprintf (buffer,"%s",strstr (data,":")+1+strspn(strstr (data,":")+1," "));
    if((tmp = strstr (buffer, "\n")))
        *tmp = '\0';
done:
    if(data!=NULL)
        app_free (data);
    return buffer;
}
char *get_cpu_thread_num (void)
{
    char *data=NULL;
    char *tmp=NULL;

    data=app_system("lscpu | grep 'Thread(s) per core:'");
    if(data==NULL)
    {
        data=app_system("lscpu | grep '每个核的线程数：'");
        if(data==NULL)
        {
            sprintf(buffer,"%s","unknow");
            goto done;
        }
        else
            sprintf(buffer,"%s",strstr(data,"：")+3+strspn(strstr(data,"：")+3," "));
    }
    else
        sprintf(buffer,"%s",strstr(data,":")+1+strspn(strstr(data,":")+1," "));
    if((tmp = strstr(buffer, "\n")))
        *tmp = '\0';
done:
    if(data!=NULL)
        app_free(data);
    return buffer;
}
char *get_cpu_cacheL1d (void)
{
    char *data=NULL;
    char *tmp=NULL;
    data = app_system("lscpu | grep 'L1d cache:'");
    if(data==NULL)
    {
        data = app_system("lscpu | grep 'L1d 缓存：'");
        if(data==NULL)
        {
            sprintf(buffer,"%s","unknow");
            goto done;
        }
        else
            sprintf(buffer,"%s",strstr(data,"：")+3+strspn(strstr(data,"：")+3," "));
    }
    else
        sprintf(buffer,"%s",strstr(data,":")+1+strspn(strstr(data,":")+1," "));
    if((tmp = strstr(buffer, "\n")))
        *tmp = '\0';
done:
    if(data!=NULL)
        app_free(data);
    return buffer;
}
char *get_cpu_cacheL1i (void)
{
    char *data=NULL;
    char *tmp=NULL;

    data=app_system("lscpu | grep 'L1i cache:'");
    if(data==NULL)
    {
        data = app_system("lscpu | grep 'L1i 缓存：'");
        if(data==NULL)
        {
            sprintf(buffer,"%s","unknow");
            goto done;
        }
        else
            sprintf(buffer,"%s",strstr(data,"：")+3+strspn(strstr(data,"：")+3," "));
    }
    else
        sprintf(buffer,"%s",strstr(data,":")+1+strspn(strstr(data,":")+1," "));
    if((tmp = strstr(buffer, "\n")))
        *tmp = '\0';
done:
    if(data!=NULL)
        app_free(data);
    return buffer;
}
char *get_cpu_cacheL2 (void)
{
    char *data=NULL;
    char *tmp=NULL;

    data=app_system("lscpu | grep 'L2 cache:'");
    if(data==NULL)
    {
        data = app_system("lscpu | grep 'L2 缓存：'");
        if(data==NULL)
        {
            sprintf(buffer,"%s","unknow");
            goto done;
        }
        else
            sprintf(buffer,"%s",strstr(data,"：")+3+strspn(strstr(data,"：")+3," "));
    }
    else
        sprintf(buffer,"%s",strstr(data,":")+1+strspn(strstr(data,":")+1," "));
    if((tmp = strstr(buffer, "\n")))
        *tmp = '\0';
done:
    if(data!=NULL)
        app_free(data);
    return buffer;
}
char *get_cpu_cacheL3 (void)
{
    char *data=NULL;
    char *tmp=NULL;

    data=app_system("lscpu | grep 'L3 cache:'");
    if(data==NULL)
    {
        data = app_system("lscpu | grep 'L3 缓存：'");
        if(data==NULL)
        {
            sprintf(buffer,"%s","unknow");
            goto done;
        }
        else
            sprintf(buffer,"%s",strstr(data,"：")+3+strspn(strstr(data,"：")+3," "));
    }
    else
        sprintf(buffer,"%s",strstr(data,":")+1+strspn(strstr(data,":")+1," "));
    if((tmp = strstr(buffer, "\n")))
        *tmp = '\0';
done:
    if(data!=NULL)
        app_free(data);
    return buffer;
}

cpu_info_t *get_cpu_info (void)
{
    S32 i;
    U64 cpuid_l=0;
    U64 cpuid_h=0;

    app_mm_read(LSCPU_ID, (U64 *)&cpuid_l, 8);
    app_mm_read(LSCPU_ID+8, (U64 *)&cpuid_h, 8);

    // printf("cpu_id_l 0x%llx,cpu_id_h 0x%llx\n",cpuid_l,cpuid_h);
    for(i=0;;i++)
    {
        // printf("cpu_name %s\n",cpu_info[i].cpu_name);
        if(cpu_info[i].id.l==cpuid_l&&  \
            cpu_info[i].id.h==cpuid_h&&  \
            strcmp(cpu_info[i].cpu_name,"UNKNOW")!=0)
        {
            return &cpu_info[i];
        }
        if(strcmp(cpu_info[i].cpu_name,"UNKNOW")==0 || cpu_info[i].id.l==0)
            break;
    }

    sprintf(cpu_info[i].cpu_name,"%s",get_cpu_name());
    sprintf(cpu_info[i].cpu_ver,"%s",get_cpu_version());
    sprintf(cpu_info[i].cacheL1d,"%s",get_cpu_cacheL1d());
    sprintf(cpu_info[i].cacheL1i,"%s",get_cpu_cacheL1i());
    sprintf(cpu_info[i].cacheL2,"%s",get_cpu_cacheL2());
    sprintf(cpu_info[i].cacheL3,"%s",get_cpu_cacheL3());
    return &cpu_info[i];
}

char *get_memory_capacity(void)
{
  char *data=NULL;
  char *tmp=NULL;
   
  data=app_system("dmidecode -t 19 | grep Size");
  if(data==NULL)
  {
    sprintf (buffer,"%s","unknow");
    return buffer;
  }
  sprintf(buffer,"%s",strstr(data,":")+1+strspn(strstr(data,":")+1," "));
  if((tmp = strstr(buffer, "\n")))
    *tmp = '\0';

  if(data!=NULL)
    app_free(data);
  return buffer;
}

char *get_memory_frequency(void)
{
  char *data=NULL;
  char *tmp=NULL;
    
  data=app_system("dmidecode -t 17 | grep Speed");
  if(data==NULL)
  {
    sprintf (buffer,"%s","unknow");
    return buffer;
  }
  sprintf(buffer,"%s",strstr(data,":")+1+strspn(strstr(data,":")+1," "));
  if((tmp = strstr(buffer, "\n")))
    *tmp = '\0';
    
  if(data!=NULL)
    app_free(data);
  return buffer;
}

char *get_memory_channel(void)
{
  char *data=NULL;
  char *tmp=NULL;
    
  data=app_system("dmidecode -t 17 | grep Locator");
  if(data==NULL)
  {
    sprintf (buffer,"%s","unknow");
    return buffer;
  }
  sprintf(buffer,"%s",strstr(data,":")+1+strspn(strstr(data,":")+1," "));
  if((tmp = strstr(buffer, "\n")))
    *tmp = '\0';
    
  if(data!=NULL)
    app_free(data);
  return buffer;
}

char *get_memory_style(void)
{
  char *data=NULL;
  char *tmp=NULL;
    
  data=app_system("dmidecode -t 17 | grep 'Type:'");
  if(data==NULL)
  {
    sprintf (buffer,"%s","unknow");
    return buffer;
  }
  sprintf(buffer,"%s",strstr(data,":")+1+strspn(strstr(data,":")+1," "));
  if((tmp = strstr(buffer, "\n")))
    *tmp = '\0';
    
  if(data!=NULL)
    app_free(data);
  return buffer;
}

char *get_memory_verification(void)
{
  char *data=NULL;
  char *tmp=NULL;
    
  data=app_system("dmidecode -t 16 | grep 'Error Correction Type:'");
  if(data==NULL)
  {
    sprintf (buffer,"%s","unknow");
    return buffer;
  }
  sprintf(buffer,"%s",strstr(data,":")+1+strspn(strstr(data,":")+1," "));
  if((tmp = strstr(buffer, "\n")))
    *tmp = '\0';
    
  if(data!=NULL)
    app_free(data);
  return buffer;
}

char *get_product_name (void)
{
    char *data=NULL;

    data=app_system("dmidecode -t 1 | grep 'Product Name:'");
    sscanf(data, "%*[^ ] %*[^ ] %s", buffer);

    if(data!=NULL)
        app_free(data);
    return buffer;
}
char *get_bios_version (void)
{
    char *data=NULL;

    data=app_system("dmidecode -t bios | grep Version:");
    sscanf(data, "%*[^ ] %s",buffer);

    if(data!=NULL)
        app_free(data);
    return buffer;
}
int get_sensors (ls_sensors_t *sen)
{
    lscpu_tempdetect (&sen->cputemp0,&sen->cputemp1);
    // printf("cpu temp0 %d ,temp1 %d\n", sen->cputemp0,sen->cputemp1);
  return 0;
}
