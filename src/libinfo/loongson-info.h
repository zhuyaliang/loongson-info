#ifndef __LOONGSON_INFO_H__
#define __LOONGSON_INFO_H__ 

#include <glib-object.h>

G_BEGIN_DECLS

#define LOONGSON_TYPE_INFO         (loongson_info_get_type ())

G_DECLARE_FINAL_TYPE (LoongsonInfo, loongson_info, LOONGSON, INFO, GObject);

LoongsonInfo   *loongson_info_new                        (void);

const char     *loongson_info_get_calculation_part       (LoongsonInfo *info);

const char     *loongson_info_get_cpu_cache              (LoongsonInfo *info);

int             loongson_info_get_cpu_speed              (LoongsonInfo *info);

const char     *loongson_info_get_cpu_model_name         (LoongsonInfo *info);

const char     *loongson_info_get_cpu_sizes              (LoongsonInfo *info);

const char     *loongson_info_get_cpu_technology         (LoongsonInfo *info);

int             loongson_info_get_cpu_threads            (LoongsonInfo *info);

const char     *loongson_info_get_extended_instruction   (LoongsonInfo *info);

const char     *loongson_info_get_hw_virt                (LoongsonInfo *info);

const char     *loongson_info_get_junction_temperature   (LoongsonInfo *info);

int             loongson_info_get_max_cpu_speed          (LoongsonInfo *info);

const char     *loongson_info_get_max_memory_capacity    (LoongsonInfo *info);

const char     *loongson_info_get_memory_channel         (LoongsonInfo *info);

const char     *loongson_info_get_memory_verification    (LoongsonInfo *info);

const char     *loongson_info_get_memory_type            (LoongsonInfo *info);

const char     *loongson_info_get_micro_architecture     (LoongsonInfo *info);

const char     *loongson_info_get_mmu_type               (LoongsonInfo *info);

const char     *loongson_info_get_packaging_method       (LoongsonInfo *info);

int             loongson_info_get_physical_core          (LoongsonInfo *info);

const char     *loongson_info_power_waste                (LoongsonInfo *info);

G_END_DECLS

#endif /* __LOONGSON_INFO_H__ */
