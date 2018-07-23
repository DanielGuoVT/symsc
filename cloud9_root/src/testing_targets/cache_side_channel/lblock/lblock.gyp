#
#
#
{
  'targets': [
    {
      'target_name': 'lblock',
      'type': 'executable',
      'cflags': [
	'-g',
	'-O0',
      ],
      'sources': [
        'lblock.c',
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

