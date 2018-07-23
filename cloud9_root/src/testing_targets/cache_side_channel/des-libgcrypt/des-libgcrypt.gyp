#
#
#
{
  'targets': [
    {
      'target_name': 'des-libgcrypt',
      'type': 'executable',
      'cflags': [
	'-g',
	'-O0',
      ],
      'sources': [
        'des-libgcrypt.c',
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

