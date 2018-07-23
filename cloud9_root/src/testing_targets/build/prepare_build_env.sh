#!/bin/bash
#
# Cloud9 Parallel Symbolic Execution Engine
# 
# Copyright (c) 2012 Google Inc. All Rights Reserved.
# Author: sbucur@google.com (Stefan Bucur)
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


THIS_DIR=$(dirname $(readlink -f "${BASH_SOURCE[0]}"))

export LLVM_PATH="${THIS_DIR}/../../third_party/llvm-build"
export BINUTILS_PATH="${THIS_DIR}/../../third_party/binutils-install"

function check-ld-plugins() {
  echo -n "-- Checking whether LD supports plug-ins... "

  local LD="${BINUTILS_PATH}/bin/ld"
  if $LD --plugin 2>&1 | grep -q "missing argument"; then
    echo "OK"
  else
    echo "FAIL"
    return 1
  fi
  return 0
}

function export-build-tools() {
  export CC="${THIS_DIR}/gcc.proxy"
  export CXX="${THIS_DIR}/g++.proxy"
  export AR="${THIS_DIR}/ar.proxy"
  export RANLIB="${THIS_DIR}/ranlib.proxy"
  export NM="${THIS_DIR}/nm.proxy"

  export LDFLAGS="-flto -Wl,-plugin-opt=also-emit-llvm -lrt"
  export ARFLAGS="-cru"
  export AR_FLAGS="-cru"
}

function export-path() {
  echo "-- Cleaning up \$PATH ..."
  PATH="$(printf "%s" "${PATH}" | /usr/bin/awk -v RS=: -v ORS=: '!($0 in a) {a[$0]; print}')"
  PATH="${PATH%:}"
  export PATH
}


function main() {
  if ! check-ld-plugins; then
    return
  fi

  PATH="${LLVM_PATH}/Release+Asserts/bin:$PATH"
  PATH="${BINUTILS_PATH}/bin:$PATH"

  export-path
  export-build-tools
}

main
