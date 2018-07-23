#
# Copyright notice here
#

{
  'variables': {
    'libcxx_path': '../libcxx',
  },
  'targets': [
    {
      'target_name': 'libcxx',
      'type': 'static_library',
      'cflags': [
        '-nostdinc++',
        '-g',
        '-Os',
        '-fPIC',
        '-std=c++0x',
        '-fstrict-aliasing',
        '-Wall',
        '-Wextra',
        '-Wshadow',
        '-Wconversion',
        '-Wnewline-eof',
        '-Wpadded',
        '-Wmissing-prototypes',
        '-Wstrict-aliasing=2',
        '-Wstrict-overflow=4',
      ],
      'include_dirs': [
        '<(libcxx_path)/include',
      ],
      'sources': [
        # Headers
        '<!@(find <(libcxx_path)/include -maxdepth 1)',

        # Implementation
        '<(libcxx_path)/src/algorithm.cpp',
        '<(libcxx_path)/src/bind.cpp',
        '<(libcxx_path)/src/chrono.cpp',
        '<(libcxx_path)/src/condition_variable.cpp',
        '<(libcxx_path)/src/debug.cpp',
        '<(libcxx_path)/src/exception.cpp',
        '<(libcxx_path)/src/future.cpp',
        '<(libcxx_path)/src/hash.cpp',
        '<(libcxx_path)/src/ios.cpp',
        '<(libcxx_path)/src/iostream.cpp',
        '<(libcxx_path)/src/locale.cpp',
        '<(libcxx_path)/src/memory.cpp',
        '<(libcxx_path)/src/mutex.cpp',
        '<(libcxx_path)/src/new.cpp',
        '<(libcxx_path)/src/random.cpp',
        '<(libcxx_path)/src/regex.cpp',
        '<(libcxx_path)/src/stdexcept.cpp',
        '<(libcxx_path)/src/string.cpp',
        '<(libcxx_path)/src/strstream.cpp',
        '<(libcxx_path)/src/system_error.cpp',
        '<(libcxx_path)/src/thread.cpp',
        '<(libcxx_path)/src/typeinfo.cpp',
        '<(libcxx_path)/src/utility.cpp',
        '<(libcxx_path)/src/valarray.cpp',
      ], # sources

      'direct_dependent_settings': {
        'cflags': [
          '-nostdinc++',
        ],
        'include_dirs': [
          '<(libcxx_path)/include',
        ],
      }, # direct_dependent_settings

      'link_settings': {
        'libraries': [
          '-pthread',
        ],
      },
    },
  ], # targets
}
