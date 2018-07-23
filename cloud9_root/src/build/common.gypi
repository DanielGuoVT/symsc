#
# Cloud9 Parallel Symbolic Execution Engine
# 

{
  'variables': {
    # TODO(bucur): Add "-g0" when clang is used, as debug linking is slow
    'make_clang_dir%': 'third_party/llvm-build/Release+Asserts',
    'emit_llvm%': 0,
    'clang%': 1,
  }, # variables
  'conditions': [
    ['clang==1', {
      'make_global_settings': [
        ['CC', '<(make_clang_dir)/bin/clang'],
        ['CXX', '<(make_clang_dir)/bin/clang++'],
        ['LINK', '$(CXX)'],
        ['CC.host', '$(CC)'],
        ['CXX.host', '$(CXX)'],
        ['LINK.host', '$(LINK)'],
      ], # make_global_settings
    }],
  ], # conditions
}
