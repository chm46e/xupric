project('xupric', 'c',
        version: '1.0',
        default_options: ['warning_level=3'])

add_global_arguments('-Wno-pedantic', language : 'c')

src = [
  'src/cfg/cfg.c',
  'src/cfg/config.c',
  'src/frame/offs/offs.c',
  'src/frame/style/style.c',
  'src/frame/download.c',
  'src/frame/frame.c',
  'src/frame/view.c',
  'src/fun/sql/bookmark.c',
  'src/fun/sql/cookie.c',
  'src/fun/sql/history.c',
  'src/fun/fun.c',
  'src/uri/uri.c',
  'src/util/atom.c',
  'src/util/path.c',
  'src/util/util.c',
  'src/xupric.c',
]
incs = include_directories('src/')

cc = meson.get_compiler('c')
cfg_lib_dirs = ['/usr/local/lib/', '/usr/lib', '/usr/share/lib']
cfg_dep = cc.find_library('libconfuse', dirs: cfg_lib_dirs)
sql_dep = dependency('sqlite3')
gtk_dep = dependency('gtk+-3.0')
x11_dep = dependency('x11', method: 'pkg-config')
webkit_dep = dependency('webkit2gtk-4.0', method: 'pkg-config')

ldflags = ['-l:libconfuse.a', '-lgthread-2.0']

custom_target('before_build',
  output: 'ignore_',
  command: ['src/sh/before_build.sh'],
  build_by_default: true
)

exec = executable('xupric', sources: src, dependencies : [gtk_dep, x11_dep, webkit_dep, cfg_dep, sql_dep], include_directories: incs, install: true, build_by_default: true)

custom_target('after_build',
  depends: exec,
  output: 'ignore__',
  command: ['src/sh/after_build.sh'],
  build_by_default: true
)

meson.add_install_script('src/sh/install.sh')
