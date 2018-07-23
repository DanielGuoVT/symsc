# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'conditions': [
    ['OS=="win"', {
      'target_defaults': {
        'defines': [
          'NOMINMAX', # To suppress max/min macro definition.
          'WIN32',
        ],
      },
    }],
  ],
  'variables': {
    'gcc_cflags': [
      '-ggdb',
      '-W',
      '-Wall',
      '-Wno-unused-parameter',
      '-fno-strict-aliasing',
      '-fPIE',
      '-fstack-protector',
      # Cloud9: Generate optimized code
      '-fno-exceptions',
      '-O2',
    ],
    'gcc_ldflags': [
      '-ggdb',
      '-fpie',
      '-Wl,-z,relro',
      '-Wl,-z,now',
    ],
  },
  'includes': [
    'ots-common.gypi',
  ],
  'target_defaults': {
    'defines': [
      'OTS_DEBUG',
    ],
    'conditions': [
      ['OS=="linux"', {
        'cflags': [
          '<@(gcc_cflags)',
        ],
        'ldflags': [
          '<@(gcc_ldflags)',
        ],
        'defines': [
          '_FORTIFY_SOURCE=2',
        ],
        'link_settings': {
          'libraries': ['-lz'],
        },
      }],
      ['OS=="mac"', {
        'xcode_settings': {
          'GCC_DYNAMIC_NO_PIC': 'NO',            # No -mdynamic-no-pic
          'GCC_SYMBOLS_PRIVATE_EXTERN': 'YES',   # -fvisibility=hidden
          'OTHER_CFLAGS': [
            '<@(gcc_cflags)',
          ],
        },
        'link_settings': {
          'libraries': [
            '/System/Library/Frameworks/ApplicationServices.framework',
            '/usr/lib/libz.dylib'
          ],
        },
      }],
      ['OS=="win"', {
        'link_settings': {
          'libraries': [
            '-lzdll.lib',
            '-lWs2_32.lib', # This is needed for htons/ntohs.
          ],
        },
        'msvs_settings': {
          'VCLinkerTool': {
            'DelayLoadDLLs': ['zlib1.dll'],
          },
        },
      }],
    ],
  },
  'targets': [
    {
      'target_name': 'ots',
      'type': 'static_library',
      'sources': [
        '<@(ots_sources)',
      ],
      'include_dirs': [
        '<@(ots_include_dirs)',
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '<@(ots_include_dirs)',
        ],
      },
      # Cloud9: Statically link the C++ library
      'dependencies': [
        '../build/libcxx.gyp:libcxx',
      ],
    },
    {
      'target_name': 'idempotent',
      'type': 'executable',
      'sources': [
        'test/idempotent.cc',
      ],
      'dependencies': [
        'ots',
      ],
      'conditions': [
        ['OS=="linux"', {
          'cflags': [
            '<!(pkg-config freetype2 --cflags)',
          ],
          'ldflags': [
            '<!(pkg-config freetype2 --libs)',
          ],
        }],
      ],
    },
    # Cloud9: Generate the OTS executable
    {
      'target_name': 'ot-sanitise',
      'type': 'executable',
      'sources': [
        'test/ot-sanitise.cc',
      ],
      'dependencies': [
        'ots',
      ],
    },
  ],
}
