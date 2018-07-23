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
        '../ots/ots-standalone.gyp:ot-sanitise',
        '../examples/examples.gyp:*',
        '../c9-tests/tests/tests.gyp:*',
      ],
    },
  ],
}
