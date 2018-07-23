#
#
#
{
  'targets': [
    {
      'target_name': 'kv_name',
      'type': 'executable',
      'cflags': [
	'-g',
	'-O0',
      ],
      'sources': [
        'kv_name.c',
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

