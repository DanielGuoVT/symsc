#
#
#
{
  'targets': [
    {
      'target_name': 'aes-openssl',
      'type': 'executable',
      'cflags': [
	'-g',
	'-O0',
      ],
      'sources': [
        'aes-openssl.c',
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

