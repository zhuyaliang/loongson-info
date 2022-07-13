/* vi: set sw=4 ts=4 wrap ai: */
/*
 * loongson-infod.h: This file is part of loongson-info.
 *
 * Copyright (C) 2022 Xiaotian Wu <wuxiaotian@loongson.cn>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 * */

#ifndef __INFO_DAEMON_H__
#define __INFO_DAEMON_H__  1

#include <glib-object.h>

G_BEGIN_DECLS

#define INFO_TYPE_DAEMON           (info_daemon_get_type ())

#if GLIB_CHECK_VERSION(2, 44, 0)
G_DECLARE_FINAL_TYPE (InfoDaemon, info_daemon, INFO, DAEMON, GObject);
#else
#define INFO_DAEMON(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), INFO_TYPE_DAEMON, InfoDaemon))
#define INFO_DAEMON_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), INFO_TYPE_DAEMON, InfoDaemonClass))
#define INFO_IS_DAEMON(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), INFO_TYPE_DAEMON))
#define INFO_IS_DAEMON_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), INFO_TYPE_DAEMON))
#define INFO_DAEMON_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), INFO_TYPE_DAEMON, InfoDaemonClass))

typedef struct _InfoDaemon                InfoDaemon;
typedef struct _InfoDaemonClass           InfoDaemonClass;
typedef struct _InfoDaemonPrivate         InfoDaemonPrivate;

struct _InfoDaemonClass
{
       GObjectClass     parent_class;
};
GType     info_daemon_get_type                   (void) G_GNUC_CONST;
#endif

InfoDaemon*     info_daemon_new    (GMainLoop *loop, gboolean replace);

G_END_DECLS

#endif /* __INFO_DAEMON_H__ */
