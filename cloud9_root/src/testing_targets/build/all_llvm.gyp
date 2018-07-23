#
#
# Cloud9 Parallel Symbolic Execution Engine
# 

{
  'targets': [
    {
      'target_name': 'AllTestingTargets',
      'type': 'none',
      'dependencies': [
        'libcxx.gyp:*',
    # '../ots/ots-standalone.gyp:ot-sanitise',
    #  '../examples/examples.gyp:*',
#	'../cache_side_channel/tests/tests.gyp:*',
	'../cache_side_channel/aes/aes.gyp:*',
	'../cache_side_channel/aes-openssl/aes-openssl.gyp:*',
	'../cache_side_channel/camellia/camellia.gyp:*',
	'../cache_side_channel/cast5/cast5.gyp:*',
	'../cache_side_channel/cast5-tom/cast5-tom.gyp:*',
	'../cache_side_channel/chaskey/chaskey.gyp:*',
	'../cache_side_channel/des/des.gyp:*',
	'../cache_side_channel/des-libgcrypt/des-libgcrypt.gyp:*',
	'../cache_side_channel/fcrypt/fcrypt.gyp:*',
	'../cache_side_channel/kv_name/kv_name.gyp:*',
	'../cache_side_channel/kasumi/kasumi.gyp:*',
	'../cache_side_channel/misty1/misty1.gyp:*',
	'../cache_side_channel/khazad/khazad.gyp:*',
	'../cache_side_channel/lblock/lblock.gyp:*',
	'../cache_side_channel/piccolo/piccolo.gyp:*',
	'../cache_side_channel/present/present.gyp:*',
	'../cache_side_channel/rfc2268/rfc2268.gyp:*',
	'../cache_side_channel/seed/seed.gyp:*',
	'../cache_side_channel/twine/twine.gyp:*',
	'../cache_side_channel/twofish/twofish.gyp:*',
      ],
    },
  ],
}
