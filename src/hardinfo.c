#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <errno.h>
#include <sys/mman.h>
#include "hardinfo.h"

cpu_info_t cpu_info [] ={
    {"3A5000",      {LS3A5000_VERSION,      0}, "3A5000",   "14nm",  "35 W",  "0 ~ 70 °C", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3A5000LL",    {LS3A5000LL_VERSION,    0}, "3A5000LL", "14nm",  "30 W",  "0 ~ 70 °C", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3A5000M",     {LS3A5000M_VERSION,     0}, "3A5000M",  "14nm",  "18 W",  "0 ~ 70 °C", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3B5000",      {LS3B5000_VERSION,      0}, "3B5000",   "14nm",  "25 W",  "0 ~ 70 °C", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3C5000L",     {LS3C5000L_VERSION,     0}, "3C5000L",  "14nm",  "200 W", "0 ~ 70 °C", "LGA","70x44x4 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3C5000L",     {LS3C5000LL_VERSION,    0}, "3C5000LL", "14nm",  "160 W", "0 ~ 85 °C", "LGA","70x44x4 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3C5000I",     {LS3A5000I_VERSION,     0}, "3A5000I",  "14nm",  "18 W",  "0 ~ 85 °C", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3C5000i",     {LS3A5000i_VERSION,     0}, "3A5000i",  "14nm",  "18 W" , "0 ~ 85 °C", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3A5000BM",    {LS3A5000BM_VERSION,    0}, "3A5000BM", "14nm",  "18 W",  "0 ~ 70 °C", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3A5000HV",    {LS3A5000HV_VERSION,    0}, "3A5000HV", "14nm",  "35 W",  "0 ~ 70 °C", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "LA464"},
    {"3A4000",      {LS3A4000_VERSION,      0}, "3A4000",   "28nm",  "35 W",  "0 ~ 70 °C", "LGA","37x37x3 mm", "64K", "64K", "256K", "16384K", "GS464V"},
    {"UNKNOW",      {0,                     0}, "UNKNOW",   "0nm",   "0 W",   "0 ~ 0 °C",  "LGA","0x0x0 mm", "0K",  "0K",  "0K",   "0K",     "UNKNOW"}
};

static glibtop_sysinfo *sysinfo = NULL;

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
    void  *p;
    int    fd;
    struct stat statbuf;
    U64    mmoffset;
    void  *mmp;

    if ((fd = open (devmem, O_RDONLY|O_SYNC)) == -1)
    {
        perror (devmem);
        return NULL;
    }

    if ((p = malloc (len)) == NULL)
    {
        perror ("malloc");
        goto out;
    }

    if (fstat (fd, &statbuf) == -1)
    {
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
        perror ("munmap");
    }
    goto out;

try_read:
    if (lseek (fd, base, SEEK_SET) == -1)
    {
        perror ("lseek");
        goto err_free;
    }
    if (read_buffer (fd, p, len, devmem) == 0)
        goto out;

err_free:
    free (p);
    p = NULL;

out:
    if (close (fd) == -1)
        perror (devmem);

    return p;
}

static void read_mm (U64 pbase, unsigned char * datas, int read_cnt)
{
    void * vaddr = NULL;

    vaddr = ls_mem_chunk (pbase, read_cnt, "/dev/mem");
    if (vaddr != NULL)
    {
        memcpy (datas,vaddr,read_cnt);
        free (vaddr);
    }
}

static void read_mm_byte (U64 pbase, U8 *datas)
{
    unsigned char buf[8] ={0};

    read_mm (pbase, buf, 1);
    *datas = (buf[0] << 0);
}

static void read_mm_hword (U64 pbase, U16 *datas)
{
    unsigned char buf[8] ={0};

    read_mm (pbase, buf, 2);
    *datas = (buf[1] << 8) + (buf[0] << 0);
}
static void read_mm_word (U64 pbase, U32 *datas)
{
    unsigned char buf[8] ={0};

    read_mm (pbase, buf, 4);
    *datas = (buf[3] << 24) + (buf[2] << 16) + (buf[1] << 8) + (buf[0] << 0);
}

static void read_mm_dword (U64 pbase, U64 *datas)
{
    u_char buf[8] = { 0 };
    U32    data_h,data_l;

    read_mm (pbase, buf, 8);
    data_l  = ((buf[0] & 0xff) << 0 );
    data_l |= ((buf[1] & 0xff) << 8);
    data_l |= ((buf[2] & 0xff) << 16);
    data_l |= ((buf[3] & 0xff) << 24);
    data_h  = ((buf[4] & 0xff) << 0);
    data_h |= ((buf[5] & 0xff) << 8);
    data_h |= ((buf[6] & 0xff) << 16);
    data_h |= ((buf[7] & 0xff) << 24);

    *datas = data_h;
    *datas = ((*datas & 0xffffffff) << 32);
    *datas |= data_l;
}

static void app_mm_read (U64 pbase, U64 *datas, int read_cnt)
{
    switch (read_cnt)
    {
        case 1:
            read_mm_byte (pbase, (U8 *)datas);
            break;
        case 2:
            read_mm_hword (pbase, (U16 *)datas);
            break;
        case 4:
            read_mm_word (pbase, (U32 *)datas);
            break;
        case 8:
            read_mm_dword (pbase, (U64 *)datas);
            break;
        default:
            printf ("mm %d is no suport!!!\n", read_cnt);
    }
}

static U8 lscpu_tempdetect (U32 *temp0,U32 *temp1)
{
    U32 Data = 0;
    U16 TempSensorDigital0,TempSensorDigital1; //UINT16
    S8  TempSensorAnalog0,TempSensorAnalog1; //INT8 (-40 - 125)

    read_mm_word (CPU_TEMP_SAMPLE_BASE + TEMP_SENSOR_VALUE_OFFSET, &Data);
    TempSensorDigital0 = Data & 0xffff;
    TempSensorDigital1 = (Data & (0xffff << 16)) >> 16; //48-32 16
    //Calculate Temp
    TempSensorAnalog0 = TempSensorDigital0 * 731 / 0x4000 - 273 ;
    TempSensorAnalog1 = TempSensorDigital1 * 731 / 0x4000 - 273 ;

    *temp0 = TempSensorAnalog0;
    *temp1 = TempSensorAnalog1;

    return 0;
}

/**
 * get_cpu_arch:
 *
 * get the hardware identifier
 *
 * Return value: a newly-allocated string or NULL.
 **/
gchar *get_cpu_arch (void)
{
    struct utsname un;
    uname (&un);
    return g_strdup (un.machine);
}

cpu_info_t *get_cpu_info (void)
{
    S32 i;
    U64 cpuid_l=0;
    U64 cpuid_h=0;

    app_mm_read (LSCPU_ID, (U64 *)&cpuid_l, 8);
    app_mm_read (LSCPU_ID + 8, (U64 *)&cpuid_h, 8);

    for ( i = 0; ; i++)
    {
        if (cpu_info[i].id.l == cpuid_l&&  \
            cpu_info[i].id.h == cpuid_h&&  \
            strcmp (cpu_info[i].cpu_name, "UNKNOW")!=0)
        {
            return &cpu_info[i];
        }

        if (strcmp (cpu_info[i].cpu_name, "UNKNOW") == 0 || cpu_info[i].id.l == 0)
            break;
    }

    return &cpu_info[i];
}

int get_sensors (ls_sensors_t *sen)
{
    lscpu_tempdetect (&sen->cputemp0, &sen->cputemp1);
    return 0;
}

const gchar *hardinfo_get_sysinfo (const gchar *key)
{
    if (sysinfo == NULL)
        sysinfo = glibtop_get_sysinfo ();
    return g_hash_table_lookup (sysinfo->cpuinfo[0].values, key);
}

const gchar *hardinfo_get_cpu_name (void)
{
    gchar *cpu_name = NULL;
    guint i;

    const char * const keys[] = {"model name", "Model Name"};

    for (i = 0; cpu_name == NULL && i < G_N_ELEMENTS (keys); i++)
    {
        cpu_name = hardinfo_get_sysinfo (keys[i]);
    }

    return cpu_name;
}

const gchar *hardinfo_get_cpu_current_speed (void)
{
    gchar *current_speed = NULL;
    guint i;
    const char * const keys[] = {"CPU MHz", "cpu MHz"};

    for (i = 0; current_speed == NULL && i < G_N_ELEMENTS (keys); i++)
    {
        current_speed = hardinfo_get_sysinfo (keys[i]);
    }

    return current_speed;
}

#define PATH_SYS_CPU    "/sys/devices/system/cpu"
#define CPUBITS_SIZE 4096
#define CPUBIT_SET(BITS, BIT) ((BITS)[(BIT)/32] |= (1 << (BIT)%32))
#define CPUBIT_GET(BITS, BIT) (((BITS)[(BIT)/32] & (1 << (BIT)%32)) >> (BIT)%32)

static int
path_exist (int idx, int ncaches)
{
    g_autofree gchar *file_name;

    file_name = g_strdup_printf (PATH_SYS_CPU "/cpu%d/cache/index%d", idx, ncaches);

    return access (file_name, F_OK) == 0;
}

static gboolean
read_cache_data (char *result, size_t len, int idx, int cache, const char *file)
{
    FILE *fd;
    g_autofree gchar *file_name = NULL;

    file_name = g_strdup_printf (PATH_SYS_CPU "/cpu%d/cache/index%d/%s", idx, cache, file);

    fd = fopen (file_name, "r");
    if (fd == NULL)
        return FALSE;

    if (!fgets (result, len, fd))
    {
        fclose(fd);
        return FALSE;
    }
    len = strlen (result);
    if (result[len - 1] == '\n')
        result[len - 1] = '\0';

    return TRUE;
}

static gboolean
read_cache (GHashTable *ht, int idx)
{
    int      ncaches = 0;
    int      i = 0;
    gboolean state;
    char     buf[256] = { 0 };

    while (path_exist (idx, ncaches))
        ncaches++;

    for (i = 0; i < ncaches; i++)
    {
        int      type;
        gboolean ret;
        char    *name;

        state = FALSE;

        /* cache type */
        ret = read_cache_data (buf, sizeof(buf), idx, i, "type");
        if (ret == FALSE)
            break;

        if (!strcmp (buf, "Data"))
            type = 'd';
        else if (!strcmp (buf, "Instruction"))
            type = 'i';
        else
            type = 0;

        /* cache level */
        ret = read_cache_data (buf, sizeof(buf), idx, i, "level");
        if (ret == FALSE)
            break;

        if (type)
            name = g_strdup_printf ("L%s%c", buf, type);
        else
            name = g_strdup_printf ("L%s", buf);

        /* cache size */
        ret = read_cache_data (buf, sizeof(buf), idx, i, "size");
        if (ret == FALSE)
            break;

        g_hash_table_insert (ht, name, g_strdup (buf));
        state = TRUE;
    }

    return state;
}

GHashTable *get_cpu_caches (void)
{
    GHashTable *ht;
    guint64     ncpu;
    gboolean    ret;
    guint64     i = 0;

    ncpu = glibtop_get_sysinfo ()->ncpu;
    ht = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_free);

    for (i = 0; i < ncpu; i++)
    {
        ret = read_cache (ht, i);
        if (ret == TRUE)
            break;
    }

    return ht;
}

static int
get_cpu_socket (int idx)
{
    FILE *fd;
    int   result;
    g_autofree gchar *file_name = NULL;

    file_name = g_strdup_printf (PATH_SYS_CPU "/cpu%d/topology/physical_package_id", idx);

    fd = fopen (file_name, "r");
    if (fd == NULL)
        return -1;

    if (fscanf(fd, "%d", &result) <= 0)
    {
        fclose(fd);
        return -1;
    }

    return result;
}

static int
get_cpu_socket_core (int idx)
{
    FILE *fd;
    int   result;
    g_autofree gchar *file_name = NULL;

    file_name = g_strdup_printf (PATH_SYS_CPU "/cpu%d/topology/core_id", idx);

    fd = fopen (file_name, "r");
    if (fd == NULL)
        return -1;

    if (fscanf(fd, "%d", &result) <= 0)
    {
        fclose(fd);
        return -1;
    }

    return result;
}

int get_cpu_core_num (void)
{
    guint64 ncpu;
    guint64 i = 0;
    guint   socket = 0;
    guint   socket_core = 0;
    char    socket_buf[256] = { 0 };
    char    socket_core_buf[256] = { 0 };

    ncpu = glibtop_get_sysinfo ()->ncpu;

    for (i = 0; i < ncpu; i++)
    {
        socket = get_cpu_socket (i);
        socket_buf[socket] = 1;

        socket_core = get_cpu_socket_core (i);
        socket_core_buf[socket_core] = 1;
    }
    socket = 0;
    socket_core = 0;

    for (i = 0; i < sizeof (socket_buf); i++)
    {
        if (socket_buf[i] == 1)
            socket++;
        if (socket_core_buf[i] == 1)
            socket_core++;
    }

    return socket * socket_core;
}

static uint *
cpubits_from_str (char *str)
{
    char *v, *nv, *hy;
    int   r0, r1;

    uint *newbits = malloc (CPUBITS_SIZE);
    if (newbits)
    {
        memset (newbits, 0, CPUBITS_SIZE);
        if (str != NULL)
        {
            v = (char*)str;
            while ( *v != 0 )
            {
                nv = strchr (v, ',');
                if (nv == NULL) nv = strchr (v, 0);
                hy = strchr (v, '-');
                if (hy && hy < nv)
                {
                    r0 = strtol (v, NULL, 0);
                    r1 = strtol (hy + 1, NULL, 0);
                }
                else
                    r0 = r1 = strtol (v, NULL, 0);

                for (; r0 <= r1; r0++)
                    CPUBIT_SET (newbits, r0);

                v = (*nv == ',') ? nv + 1 : nv;
            }
        }
    }
    return newbits;
}

static uint
cpubits_count (uint *b)
{
    static const uint max = CPUBITS_SIZE * 8;
    uint count = 0, i = 0;

    while (i < max)
    {
        count += CPUBIT_GET (b, i);
        i++;
    }
    return count;
}

int get_cpu_thread_num (void)
{
    char *tmp;
    uint *threads;
    int   thread;

    g_file_get_contents("/sys/devices/system/cpu/present", &tmp, NULL, NULL);
    if (tmp == NULL)
        return -1;

    threads = cpubits_from_str (tmp);
    thread = cpubits_count (threads);

    g_free (threads);

    return thread;
}
