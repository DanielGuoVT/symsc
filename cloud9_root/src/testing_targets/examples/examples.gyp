#
#
#

{
  'targets': [
    {
      'target_name': 'prod-cons',
      'type': 'executable',
      'sources': [
        'prod-cons.c',
      ],
      'include_dirs': [
        '../../cloud9/include',
      ],

      'link_settings': {
        'libraries': [
          '-pthread',
        ],
      },
    },
  ],
}
