#
#
#
{
  'targets': [
    {
      'target_name': 'present',
      'type': 'executable',
      'cflags': [
	'-g',
	'-O0',
      ],
      'sources': [
        'present.c',
      ],
      'include_dirs': [
        '../../../cloud9/include',
      ],

      'link_settings': {
        'libraries': [
			'-pthread',
        ],
      },
    },
  ],
}

