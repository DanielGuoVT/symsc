#
#
#
{
  'targets': [
    {
      'target_name': 'kasumi',
      'type': 'executable',
      'cflags': [
	'-g',
	'-O0',
      ],
      'sources': [
        'kasumi.c',
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

