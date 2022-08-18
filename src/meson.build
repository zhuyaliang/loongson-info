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
  dependencies : [gtk_deps, gtop_dep],
  include_directories: top_srcdir,
  install : true,
  install_dir : get_option('bindir')
  )

gdbus_src = gnome.gdbus_codegen(
    'info-dbus-generated',
    join_paths(data_dir, 'cn.loongson.info.xml'),
    interface_prefix : 'cn.loongson.',
    namespace : 'Bus'
    )

executable(
    'loongson-infod',
    gdbus_src,
    sources : ['loongson-infod.h', 'loongson-infod.c', 'server.c', 'hardinfo.c', 'dmidecode.c'],
    dependencies : [gio_dep, gio_os_dep, gtop_dep],
    install : true,
    install_dir : join_paths(get_option('prefix'), get_option('libexecdir'), 'loongson-info')
)

subdir('libinfo')
