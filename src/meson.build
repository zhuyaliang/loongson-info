gnome = import('gnome')

sources = [
'main.c',
'loongson-window.c',
'loongson-spec.c',
'loongson-security.c',
'loongson-mainboard.c',
'loongson-perf.c',
'loongson-state.c',
'loongson-utils.c',
]

executable('loongson-info',
  sources : sources,
  dependencies : [gtk_deps,gtop_deps],
  include_directories: top_srcdir,
  install : true,
  install_dir : get_option('bindir')
  )


gdbus_src = gnome.gdbus_codegen(
    'info-dbus-generated',
    'cn.loongson.info.xml',
    interface_prefix : 'cn.loongson.',
    namespace : 'Bus'
    )

executable(
    'loongson-infod',
    gdbus_src,
    sources : ['loongson-infod.h', 'loongson-infod.c', 'server.c'],
    dependencies : [gio_dep, gio_os_dep],
    install : true,
    install_dir : join_paths(get_option('prefix'), get_option('libexecdir'), 'loongson-info')
    )

# no quoting
cdata = configuration_data()
cdata.set('pkglibexecdir', join_paths(get_option('prefix'), get_option('libexecdir'), 'loongson-info'))

# replace @pkglibexecdir@
configure_file(
    input : 'cn.loongson.info.service.in',
    output : 'cn.loongson.info.service',
    install: true,
    install_dir: join_paths(get_option('datadir'), 'dbus-1/system-services'),
    configuration : cdata
    )
