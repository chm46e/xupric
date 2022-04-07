project('sulfer', 'c', version: '0.1.0', 
  default_options: ['warning_level=3', 'c_std=gnu18'])

cc = meson.get_compiler('c')
cfg_lib_dirs = ['/usr/local/lib/', '/usr/lib', '/usr/share/lib']
cfg_dep = cc.find_library('libconfuse', dirs: cfg_lib_dirs) 

sql_dep = dependency('sqlite3')

gtk_dep = dependency('gtk+-3.0')
x11_dep = dependency('x11', method: 'pkg-config')
webkit_dep = dependency('webkit2gtk-4.0', method: 'pkg-config')

src = ['src/sulfer.c', 'src/cfg/cfg.c', 'src/cfg/config.c', 'src/frame/frame.c', 'src/frame/handle.c', 'src/uri/uri.c', 'src/util/util.c', 'src/util/atom.c', 'src/util/path.c', 'src/extras/bookmark.c', 'src/extras/history.c', 'src/frame/download.c', 'src/extras/cookie.c']

incs = include_directories('src/')
ldflags = ['-l:libconfuse.a', '-lgthread-2.0']

custom_target('before_build',
  output: 'ignore_',
  command: ['src/sh/before_build.sh'],
  build_by_default: true
)

exec = executable('sulfer', sources: src, dependencies : [gtk_dep, x11_dep, webkit_dep, cfg_dep, sql_dep], include_directories: incs, install: true, build_by_default: true)

custom_target('after_build',
  depends: exec,
  output: 'ignore__',
  command: ['src/sh/after_build.sh'],
  build_by_default: true
)

meson.add_install_script('src/sh/install.sh')
