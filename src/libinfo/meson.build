pkg = import('pkgconfig')
libinfo_dir = meson.current_source_dir()

subdir = 'libinfo'

install_headers(
  'loongson-info.h',
  install_dir: join_paths(ls_pkgincludedir, subdir),
)

sources = files(
  'loongson-info.c',
)

symbol_map = join_paths(meson.current_source_dir(), 'symbol.map')
ldflags = cc.get_supported_link_arguments('-Wl,--version-script,@0@'.format(symbol_map))

libinfo = shared_library(
  ls_name,
  sources: sources + gdbus_src,
  version: libversion,
  include_directories: top_srcdir,
  dependencies : [gio_dep, gio_os_dep],
  c_args: '-DG_LOG_DOMAIN="@0@"'.format(meson.project_name()),
  link_args: ldflags,
  link_depends: symbol_map,
  install: true,
)

pkg.generate(
  libinfo,
  name: 'Loongson Info Service',
  description: 'Client Library for communicating with loongson info service',
  filebase: ls_name,
  subdirs: ls_api_name,
  variables: [
    'exec_prefix=${prefix}',
    'datadir=${prefix}/' + get_option('datadir'),
    'interfacesdir=${datadir}/loongson-info/interfaces',
  ],
)
