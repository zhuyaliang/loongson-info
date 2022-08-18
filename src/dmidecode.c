/*************************************************************************
 File Name: dmidecode.c
 Author: zhuyaliang

 Copyright (C) 2022  zhuyaliang https://github.com/zhuyaliang/
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.

 Created Time: 2022年08月16日 星期二 20时31分33秒
************************************************************************/
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "dmidecode.h"

typedef struct {
    guint l;
    guint h;
} u64;

struct _LoongsonDmidecode
{
    GObject   parent;
    char     *dmi_data;
    char     *dmi_table_data;
    gint64    dmi_base;
    guint     dmi_len;
    guint16   dmi_num;
    guint     dmi_flags;
    gboolean  found;
};

#define FLAG_NO_FILE_OFFSET     (1 << 0)
#define FLAG_STOP_AT_EOT        (1 << 1)

#define SYS_FIRMWARE_DIR        "/sys/firmware/dmi/tables"
#define SYS_ENTRY_FILE SYS_FIRMWARE_DIR "/smbios_entry_point"
#define SYS_TABLE_FILE SYS_FIRMWARE_DIR "/DMI"
#define DWORD(x)                (char)(*(const char *)(x))
#define QWORD(x)                (*(const u64 *)(x))
#define WORD(x)                 (guint16)(*(const guint16 *)(x))

G_DEFINE_TYPE (LoongsonDmidecode, loongson_dmidecode, G_TYPE_OBJECT)

static int myread (int fd, char *buf, size_t count, const char *prefix)
{
    ssize_t r = 1;
    size_t r2 = 0;

    while (r2 != count && r != 0)
    {
        r = read (fd, buf + r2, count - r2);
        if (r == -1)
            return -1;
        else
            r2 += r;
    }

    if (r2 != count)
        return -1;

    return 0;
}

static char *dmi_read_file (size_t *max_len, const char *filename)
{
    struct stat statbuf;
    int    fd;
    char  *p;

    if ((fd = open (filename, O_RDONLY)) == -1)
        return NULL;

    if (fstat (fd, &statbuf) == 0)
    {
        if (statbuf.st_size <= 0)
        {
            close (fd);
            return NULL;
        }
        *max_len = statbuf.st_size;
    }

    if ((p = malloc (*max_len)) == NULL)
        goto out;

    if (lseek (fd, 0, SEEK_SET) == -1)
        goto err_free;

    if (myread (fd, p, *max_len, filename) == 0)
        goto out;

err_free:
    free(p);
    p = NULL;

out:
    close (fd);
    return p;
}

static int checksum (const char *data, size_t len)
{
    char   sum = 0;
    size_t a;

    for (a = 0; a < len; a++)
        sum += data[a];

    return (sum == 0);
}

static const char *dmi_print_memory_size (u64 code, int shift)
{
    unsigned long capacity;
    guint16  split[7];
    static const char *unit[8] = {
        "bytes", "kB", "MB", "GB", "TB", "PB", "EB", "ZB"
    };
    int i;

    split[0] = code.l & 0x3FFUL;
    split[1] = (code.l >> 10) & 0x3FFUL;
    split[2] = (code.l >> 20) & 0x3FFUL;
    split[3] = ((code.h << 2) & 0x3FCUL) | (code.l >> 30);
    split[4] = (code.h >> 8) & 0x3FFUL;
    split[5] = (code.h >> 18) & 0x3FFUL;
    split[6] = code.h >> 28;

    for (i = 6; i > 0; i--)
    {
        if (split[i])
            break;
    }
    if (i > 0 && split[i - 1])
    {
        i--;
        capacity = split[i] + (split[i + 1] << 10);
    }
    else
        capacity = split[i];

    return g_strdup_printf ("%lu %s", capacity, unit[i + shift]);
}

static u64 u64_range (u64 start, u64 end)
{
    u64 res;

    res.h = end.h - start.h;
    res.l = end.l - start.l;

    if (end.l < start.l)
        res.h--;
    if (++res.l == 0)
        res.h++;

    return res;
} 

static const char *dmi_mapped_address_size (u64 start, u64 end)
{
    const char *sizes;

    if (start.h == end.h && start.l == end.l)
        sizes = g_strdup ("Unknown");
    else
        sizes = dmi_print_memory_size (u64_range(start, end), 0);

    return sizes;
}

static const char *dmi_memory_device_speed (guint16 code1, guint code2)
{
    const char *speed;

    if (code1 == 0xFFFF)
    {
        if (code2 == 0)
            speed = g_strdup ("Unknown");
        else
            speed = g_strdup_printf ("%u", code2);
    }
    else
    {
        if (code1 == 0)
            speed = g_strdup ("Unknown");
        else
            speed = g_strdup_printf ("%u", code1);
    }

    return speed;
}

static void ascii_filter (char *bp, size_t len)
{
    size_t i;

    for (i = 0; i < len; i++)
        if (bp[i] < 32 || bp[i] >= 127)
            bp[i] = '.';
}

static char *_dmi_string (char *data, int filter, char s)
{
    char *bp = data;

    bp += data[1];
    while (s > 1 && *bp)
    {
        bp += strlen (bp);
        bp++;
        s--;
    }

    if (!*bp)
        return g_strdup ("Unknown");

    if (filter)
        ascii_filter (bp, strlen(bp));

    return g_strdup (bp);
}

static const char *dmi_string (char *data, char s)
{
    char *bp;

    if (s == 0)
        return g_strdup ("Not Specified");

    bp = _dmi_string (data, 1, s);
    if (bp == NULL)
        return g_strdup ("Unknown");

    return bp;
}

static const char *dmi_memory_device_type (char code)
{
    static const char *type[] = {
            "Other", /* 0x01 */
            "Unknown",
            "DRAM",
            "EDRAM",
            "VRAM",
            "SRAM",
            "RAM",
            "ROM",
            "Flash",
            "EEPROM",
            "FEPROM",
            "EPROM",
            "CDRAM",
            "3DRAM",
            "SDRAM",
            "SGRAM",
            "RDRAM",
            "DDR",
            "DDR2",
            "DDR2 FB-DIMM",
            "Reserved",
            "Reserved",
            "Reserved",
            "DDR3",
            "FBD2",
            "DDR4",
            "LPDDR",
            "LPDDR2",
            "LPDDR3",
            "LPDDR4",
            "Logical non-volatile device",
            "HBM",
            "HBM2",
            "DDR5",
            "LPDDR5" /* 0x23 */
    };
    if (code >= 0x01 && code <= 0x23)
        return g_strdup (type[code - 0x01]);

    return g_strdup ("Unknown");
}

static const char *get_dmi_memory_device (char *data, guint index)
{
    const char *string = NULL;

    switch (index)
    {
        case 0:
            string = dmi_memory_device_speed (WORD (data + 0x15), data[1] >= 0x5C ? DWORD (data + 0x54) : 0);
            break;
        case 1:
            string = dmi_string (data, data[0x10]);
            break;
        case 2:
            string = dmi_memory_device_type (data[0x12]);
            break;
        default:
            string  = g_strdup ("Unknown");
            break;
    }

    return string;
}

static const char *dmi_memory_array_ec_type (char code)
{
    static const char *type[] = {
        "Other", /* 0x01 */
        "Unknown",
        "None",
        "Parity",
        "Single-bit ECC",
        "Multi-bit ECC",
        "CRC" /* 0x07 */
    };

    if (code >= 0x01 && code <= 0x07)
        return g_strdup (type[code - 0x01]);

    return g_strdup ("<OUT OF SPEC>");
}

static const char *dmi_decode (char *data, guint index, guint sub_index)
{
    const char *string = NULL;
    guint16 ret;

    switch (index)
    {
        case 0:
            string = dmi_string (data, data[0x05]);
            break;
        case 1:
            string = dmi_string (data, data[0x05]);
            break;
        case 4:
            ret = WORD (data + 0x14);
            if (ret)
                string = g_strdup_printf ("%u", ret);
            else
                string  = g_strdup ("Unknown");
            break;
        case 16:
            string = dmi_memory_array_ec_type (data[0x06]);
            break;
        case 17:
            string = get_dmi_memory_device (data, sub_index);
            break;
        case 19:
            string = dmi_mapped_address_size (QWORD (data + 0x0F), QWORD (data + 0x17));
            break;
        default:
            string  = g_strdup ("Unknown");
            break;
    }

    return string;
}

static const char *dmi_table_decode (LoongsonDmidecode *dmi, guint index, guint sub_index)
{
    char *data;
    int   i = 0;

    data = dmi->dmi_table_data;
    while ((i < dmi->dmi_num || !dmi->dmi_num) && data + 4 <= dmi->dmi_table_data + dmi->dmi_len)
    {
        char *next;

        if (data[1] < 4 || (data[0] == 127))
            break;
        i++;

        next = data + data[1];
        while ((guint64)(next - dmi->dmi_table_data + 1) < dmi->dmi_len
            && (next[0] != 0 || next[1] != 0))
            next++;
        next += 2;

        if ((guint64)(next - dmi->dmi_table_data) > dmi->dmi_len)
            break;
        data = next;
    }
    i = 0;
    data = dmi->dmi_table_data;

    while ((i < dmi->dmi_num || !dmi->dmi_num) && data + 4 <= dmi->dmi_table_data + dmi->dmi_len)
    {
        char *next;

        if (data[1] < 4 || data[0] == 127)
            break;
        i++;

        next = data + data[1];
        while ((guint64)(next - dmi->dmi_table_data + 1) < dmi->dmi_len
            && (next[0] != 0 || next[1] != 0))
            next++;
        next += 2;

        if ((guint)data[0] == index)
            return dmi_decode (data, index, sub_index);

        data = next;
        if (data[0] == 127 && (dmi->dmi_flags & FLAG_STOP_AT_EOT))
            break;
    }

    return g_strdup ("Unknown");
}

static gboolean dmi_table (LoongsonDmidecode *dmi)
{
    size_t size = dmi->dmi_len;

    dmi->dmi_table_data = dmi_read_file (&size, SYS_TABLE_FILE);
    dmi->dmi_len = size;

    if (dmi->dmi_table_data == NULL)
        return FALSE;

    return TRUE;
}

static gboolean smbios3_decode (LoongsonDmidecode *dmi)
{
    u64  offset;

    if (dmi->dmi_data[0x06] > 0x20)
        return FALSE;

    if (!checksum (dmi->dmi_data, dmi->dmi_data[0x06]))
        return FALSE;

    offset = QWORD (dmi->dmi_data + 0x10);
    if (offset.h && sizeof(off_t) < 8)
        return FALSE;

    dmi->dmi_base = ((off_t)offset.h << 32) | offset.l;
    dmi->dmi_len = DWORD (dmi->dmi_data + 0x0C);
    dmi->dmi_num = 0;
    dmi->dmi_flags = FLAG_NO_FILE_OFFSET | FLAG_STOP_AT_EOT;

    if (!dmi_table (dmi))
        return FALSE;

    return TRUE;
}

static gboolean smbios_decode (LoongsonDmidecode *dmi)
{
    if (dmi->dmi_data[0x05] > 0x20)
        return FALSE;

    if (!checksum (dmi->dmi_data, dmi->dmi_data[0x05])
     || memcmp (dmi->dmi_data + 0x10, "_DMI_", 5) != 0
     || !checksum (dmi->dmi_data + 0x10, 0x0F))
        return FALSE;

    dmi->dmi_base = DWORD (dmi->dmi_data + 0x18);
    dmi->dmi_len = WORD (dmi->dmi_data + 0x16);
    dmi->dmi_num = WORD (dmi->dmi_data + 0x1C);
    dmi->dmi_flags = FLAG_NO_FILE_OFFSET;

    if (!dmi_table (dmi))
        return FALSE;

    return TRUE;
}

static void loongson_dmidecode_dispose (GObject *object)
{
    LoongsonDmidecode *dmi;

    dmi = LOONGSON_DMIDECODE (object);
    if (dmi->dmi_data != NULL)
        g_free (dmi->dmi_data);

    if (dmi->dmi_table_data != NULL)
        g_free (dmi->dmi_table_data);

    G_OBJECT_CLASS (loongson_dmidecode_parent_class)->dispose (object);
}

static void loongson_dmidecode_class_init (LoongsonDmidecodeClass *class)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (class);

    gobject_class->dispose = loongson_dmidecode_dispose;
}

static void loongson_dmidecode_init (LoongsonDmidecode *dmi)
{
    size_t size = 0;

    dmi->found = FALSE;

    if ((dmi->dmi_data = dmi_read_file (&size, SYS_ENTRY_FILE)) != NULL)
    {
        if (size >= 24 && memcmp (dmi->dmi_data, "_SM3_", 5) == 0)
        {
            if (smbios3_decode (dmi))
                dmi->found = TRUE;
        }
        else if (size >= 31 && memcmp (dmi->dmi_data, "_SM_", 4) == 0)
        {
            if (smbios_decode (dmi))
                dmi->found = TRUE;
        }
    }
}

guint dmi_get_cpu_max_speed (LoongsonDmidecode *dmi)
{
    const char *data;
    guint speed;

    g_return_val_if_fail (LOONGSON_IS_DMIDECODE (dmi), 0);

    data = dmi_table_decode (dmi, 4, 0);

    if(!strcmp(data, "Unknown"))
        speed = 2200;
    else
        speed = atoi (data);

    g_free (data);
    return speed;
}

const char *dmi_get_memory_capacity (LoongsonDmidecode *dmi)
{
    const char *data;

    g_return_val_if_fail (LOONGSON_IS_DMIDECODE (dmi), NULL);

    data = dmi_table_decode (dmi, 19, 0);

    return data;
}

const char *dmi_get_memory_frequency (LoongsonDmidecode *dmi)
{
    const char *data;

    g_return_val_if_fail (LOONGSON_IS_DMIDECODE (dmi), NULL);

    data = dmi_table_decode (dmi, 17, 0);

    return data;
}

const char *dmi_get_memory_channel (LoongsonDmidecode *dmi)
{
    const char *data;

    g_return_val_if_fail (LOONGSON_IS_DMIDECODE (dmi), NULL);

    data = dmi_table_decode (dmi, 17, 1);

    return data;
}

const char *dmi_get_memory_type (LoongsonDmidecode *dmi)
{
    const char *data;

    g_return_val_if_fail (LOONGSON_IS_DMIDECODE (dmi), NULL);

    data = dmi_table_decode (dmi, 17, 2);

    return data;
}

const char *dmi_get_memory_verification (LoongsonDmidecode *dmi)
{
    const char *data;

    g_return_val_if_fail (LOONGSON_IS_DMIDECODE (dmi), NULL);

    data = dmi_table_decode (dmi, 16, 0);

    return data;
}

const char *dmi_get_product_name (LoongsonDmidecode *dmi)
{
    const char *data;

    g_return_val_if_fail (LOONGSON_IS_DMIDECODE (dmi), NULL);

    data = dmi_table_decode (dmi, 1, 0);

    return data;
}

const char *dmi_get_bios_version (LoongsonDmidecode *dmi)
{
    const char *data;

    g_return_val_if_fail (LOONGSON_IS_DMIDECODE (dmi), NULL);

    data = dmi_table_decode (dmi, 0, 0);

    return data;
}

LoongsonDmidecode *loongson_dmidecode_new (void)
{
    LoongsonDmidecode *dmi;

    dmi = g_object_new (LOONGSON_TYPE_DMIDECODE, NULL);

    if (!dmi->found)
        return NULL;

    return dmi;
}
