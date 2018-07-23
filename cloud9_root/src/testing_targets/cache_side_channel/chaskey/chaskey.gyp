#
#
#
{
  'targets': [
    {
      'target_name': 'chaskey',
      'type': 'executable',
      'cflags': [
	'-g',
	'-O0',
      ],
      'sources': [
        'chaskey.c',
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

