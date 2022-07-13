subdir('icons')

install_data(
  ls_namespace + '.conf',
  install_dir: dbus_conf_dir,
)

service = 'loongson-info-daemon.service'
service_conf = configuration_data()
service_conf.set('pkglibexecdir', join_paths(get_option('prefix'), get_option('libexecdir'), 'loongson-info'))
configure_file(
   input: service + '.in',
   output: service,
   configuration: service_conf,
   install: true,
   install_dir: install_systemd_unit_dir,
)
