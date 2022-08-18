#ifndef __DMIDECODE_H__
#define __DMIDECODE_H__  1

#include <glib-object.h>

G_BEGIN_DECLS

#define LOONGSON_TYPE_DMIDECODE           (loongson_dmidecode_get_type ())

G_DECLARE_FINAL_TYPE (LoongsonDmidecode, loongson_dmidecode, LOONGSON, DMIDECODE, GObject);

LoongsonDmidecode*   loongson_dmidecode_new       (void);

guint                dmi_get_cpu_max_speed        (LoongsonDmidecode *dmi);

const char          *dmi_get_memory_capacity      (LoongsonDmidecode *dmi);

const char          *dmi_get_memory_frequency     (LoongsonDmidecode *dmi);

const char          *dmi_get_memory_channel       (LoongsonDmidecode *dmi);

const char          *dmi_get_memory_type          (LoongsonDmidecode *dmi);

const char          *dmi_get_memory_verification  (LoongsonDmidecode *dmi);

const char          *dmi_get_product_name         (LoongsonDmidecode *dmi);

const char          *dmi_get_bios_version         (LoongsonDmidecode *dmi);

G_END_DECLS

#endif /* __DMIDECODE_H__ */
