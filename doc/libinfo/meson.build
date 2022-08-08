doc_module = 'libinfo'

gnome.gtkdoc(
  doc_module,
  main_xml: doc_module + '-docs.xml',
  src_dir: libinfo_dir,
  dependencies : [gio_dep, gio_os_dep],
  namespace: 'loongson_info',
  fixxref_args: '--html-dir=' + join_paths(ls_prefix, gnome.gtkdoc_html_dir(doc_module)),
  gobject_typesfile: files(doc_module + '.types'),
  install: true,
)
