#
# Cloud9 Parallel Symbolic Execution Engine
# 
# Copyright (c) 2012, Dependable Systems Laboratory, EPFL
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Dependable Systems Laboratory, EPFL nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE DEPENDABLE SYSTEMS LABORATORY, EPFL BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# All contributors are listed in CLOUD9-AUTHORS file.
#

{
  'variables': {
    'stp_path': '../third_party/stp',
    'stp_src': '../third_party/stp/src',
    'valid_extensions': [
      '-name', '*.h',
      '-o', '-name', '*.hpp',
      '-o', '-name', '*.cpp',
      '-o', '-name', '*.c',
    ],
    'lex_tool': 'flex',
    'yacc_tool': 'bison -d -y --debug -v',
  }, # variables
  
  'target_defaults': {
    'cflags': [
      '-O3',
      '-fomit-frame-pointer',
      '-Wno-deprecated',
    ],
    'defines': [
      'NDEBUG',
      # Required by MiniSAT
      '__STDC_LIMIT_MACROS',
      '__STDC_FORMAT_MACROS',
      'EXT_HASH_MAP',
    ],
  }, # target_defaults
  
  'targets': [
    {
      'target_name': 'libast',
      'type': 'static_library',
      'sources': [
        '<!@(find <(stp_src)/AST -maxdepth 1 <(valid_extensions))',
        '<!@(find <(stp_src)/STPManager -maxdepth 1 <(valid_extensions))',
        '<!@(find <(stp_src)/absrefine_counterexample -maxdepth 1 <@(valid_extensions))',   
        '<!@(find <(stp_src)/AST/NodeFactory -maxdepth 1 <(valid_extensions))',
        '<!@(find <(stp_src)/c_interface -maxdepth 1 <(valid_extensions))',
        #'<!@(find <(stp_src)/cpp_interface -maxdepth 1 <(valid_extensions))',
        '<!@(find <(stp_src)/to-sat <(valid_extensions))',
      ],
    },
    {
      'target_name': 'libstpmgr',
      'type': 'static_library',
      'sources': [
      ],
    },
    {
      'target_name': 'libprinter',
      'type': 'static_library',
      'sources': [
        '<!@(find <(stp_src)/printer -maxdepth 1 <(valid_extensions))',
      ],
    },
    {
      'target_name': 'libabstractionrefinement',
      'type': 'static_library',
      'sources': [
      ],
    },
  ], # targets
}
