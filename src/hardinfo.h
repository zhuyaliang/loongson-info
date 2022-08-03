#ifndef __HARD_INFO__
#define __HARD_INFO__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <glibtop/sysinfo.h>

/* Common unsigned types */
#ifndef DEFINED_U8
#define DEFINED_U8
typedef unsigned char  U8;
#endif

#ifndef DEFINED_U16
#define DEFINED_U16
typedef unsigned short U16;
#endif

#ifndef DEFINED_U32
#define DEFINED_U32
typedef unsigned int   U32;
#endif

#ifndef DEFINED_U64
#define DEFINED_U64
typedef unsigned long long  U64;
#endif

/* Common signed types */
#ifndef DEFINED_S8
#define DEFINED_S8
typedef signed char  S8;
#endif

#ifndef DEFINED_S16
#define DEFINED_S16
typedef signed short S16;
#endif

#ifndef DEFINED_S32
#define DEFINED_S32
typedef signed int   S32;
#endif

#ifndef DEFINED_S32
#define DEFINED_S32
typedef signed int   S32;
#endif

#ifndef DEFINED_S64
#define DEFINED_S64
typedef signed long long  S64;
#endif

#ifndef NULL
#define NULL (void *)0
#endif

/* BOOL type constant values */
#ifndef TRUE
#define TRUE (1 == 1)
#endif

#ifndef FALSE
#define FALSE (!TRUE)
#endif

#define LS3A4000_VERSION                        0x0000303030344133 /* 3A4000 */

#define LS3A5000_VERSION                        0x0000303030354133 /* 3A5000 */
#define LS3A5000LL_VERSION                      0x4C4C303030354133 /* 3A5000LL */
#define LS3A5000M_VERSION                       0x004D303030354133 /* 3A5000M */
#define LS3B5000_VERSION                        0x0000303030354233 /* 3B5000 */
#define LS3C5000L_VERSION                       0x004C303030354333 /* 3C5000L */
#define LS3C5000LL_VERSION                      0x4C4C303030354333 /* 3C5000LL */
#define LS3A5000I_VERSION                       0x0049303030354133 /* 3A5000I */
#define LS3A5000i_VERSION                       0x0069303030354133 /* 3A5000i */
#define LS3A5000BM_VERSION                      0x4D42303030354133 /* 3A5000BM */
#define LS3A5000HV_VERSION                      0x5648303030354133 /* 3A5000HV */

#define LSCPU_ID                                0x1fe00020
#define CPU_TEMP_SAMPLE_BASE                    0x1fe00198  //Temperature sampling register
#define TEMP_SENSOR_VALUE_OFFSET                0x4

typedef struct {
    U64 l;
    U64 h;
} cpuid;

typedef struct _cpu_info_t_{
    char cpu_name[100];
    cpuid id;
    char cpu_ver[100];
    char technics[100];
    char cpu_tdp[8];
    char junctiontemperature[20]; //Junction Temperature
    char cpu_pkg[8];
    char cpu_l_w_h[20];
    char cacheL1d[100];
    char cacheL1i[100];
    char cacheL2[100];
    char cacheL3[100];
    char microarch[100];
}cpu_info_t;

typedef struct _ls_sensors_t_{
    U32 cputemp0;
    U32 cputemp1;
    U32 ls7atemp0;
    U32 ls7atemp1;
    U32 fan0;
    U32 fan1;
}ls_sensors_t;

char       *get_cpu_max_speed          (void);
char       *get_cpu_min_speed          (void);
char       *get_cpu_arch               (void);
int         get_cpu_core_num           (void);
int         get_cpu_thread_num         (void);
int         get_sensors                (ls_sensors_t *sen);
char       *get_bios_version           (void);
char       *get_product_name           (void);
cpu_info_t *get_cpu_info               (void);
char       *get_memory_capacity        (void);
char       *get_memory_frequency       (void);
char       *get_memory_channel         (void);
char       *get_memory_style           (void);
char       *get_memory_verification    (void);

const gchar *hardinfo_get_sysinfo           (const gchar *key);
const gchar *hardinfo_get_cpu_name          (void);
const gchar *hardinfo_get_cpu_current_speed (void);
GHashTable  *get_cpu_caches                 (void);

#define CPUCFG_0  0
#define CPUCFG_1  1
#define CPUCFG_2  2
#define CPUCFG_3  3
#define CPUCFG_4  4
#define CPUCFG_5  5
#define CPUCFG_6  6
#define CPUCFG_7  7
#define CPUCFG_8  8
#define CPUCFG_9  9
#define CPUCFG_10 10
#define CPUCFG_11 11
#define CPUCFG_12 12
#define CPUCFG_13 13
#define CPUCFG_14 14

#define loongarch_get_cpucfg(val, reg)            \
do {                                            \
        U64 __res;                           \
        /* cpucfg val, reg */                   \
        __asm__ __volatile__(                   \
                "cpucfg  %0, %1 \n\t"           \
                :"=r"(__res)                    \
                :"r"(reg)                       \
                :                               \
                );                              \
        val = (U32)__res;                    \
} while(0)

#endif
