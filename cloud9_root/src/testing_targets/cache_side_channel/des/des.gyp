#
#
#
{
  'targets': [
    {
      'target_name': 'des',
      'type': 'executable',
      'cflags': [
	'-g',
	'-O0',
      ],
      'sources': [
        'des.c',
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

