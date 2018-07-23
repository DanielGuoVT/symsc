#!/usr/bin/env bash

# This script will download binutils into third_party/binutils and build it.

# Do NOT CHANGE this, unless you know what you're doing
BINUTILS_SNAPSHOT="binutils-2.22.tar.bz2"


THIS_DIR="$(dirname "${0}")"
BINUTILS_DIR="${THIS_DIR}/../third_party/binutils"
BINUTILS_DWD_DIR="${THIS_DIR}/../third_party/.downloads"
BINUTILS_BUILD_DIR="${BINUTILS_DIR}/../binutils-install"
STAMP_FILE="${BINUTILS_BUILD_DIR}/cr_build_revision"

BINUTILS_URL="http://ftp.gnu.org/gnu/binutils"

set -e

force_local_build=
while [[ $# > 0 ]]; do
  case $1 in
    --force-local-build)
      force_local_build=yes
      ;;
    --help)
      echo "usage: $0 [--force-local-build] "
      echo "--force-local-build: Force compilation"
      exit 1
      ;;
  esac
  shift
done

# Check if there's anything to be done, exit early if not.
if [[ -f "${STAMP_FILE}" ]]; then
  PREVIOUSLY_BUILT_REVISON=$(cat "${STAMP_FILE}")
  if [[ -z "$force_local_build" ]] && \
			 [[ "${PREVIOUSLY_BUILT_REVISON}" = "${BINUTILS_SNAPSHOT}" ]]; then
    echo "Binutils snapshot ${BINUTILS_SNAPSHOT} already built."
    exit 0
  fi
fi
# To always force a new build if someone interrupts their build half way.
rm -f "${STAMP_FILE}"

BINUTILS_OUTPUT="${BINUTILS_DWD_DIR}/${BINUTILS_SNAPSHOT}"

if [ ! -f "${BINUTILS_OUTPUT}" ]; then
		mkdir -p "${BINUTILS_DWD_DIR}"
		echo Downloading Binutils snapshot "${BINUTILS_SNAPSHOT}" at "${BINUTILS_URL}"
		if which curl > /dev/null; then
				curl -L --fail "${BINUTILS_URL}/${BINUTILS_SNAPSHOT}" -o "${BINUTILS_OUTPUT}"
		elif which wget > /dev/null; then
				wget "${BINUTILS_URL}/${BINUTILS_SNAPSHOT}" -O "${BINUTILS_OUTPUT}"
		else
				echo "Neither curl nor wget found. Please install one of these."
				exit 1
		fi

		if [ ! -f "${BINUTILS_OUTPUT}" ]; then
				echo Could not download Binutils
				exit 1
		fi
		echo Binutils downloaded successfully
else
		echo Binutils snapshot already cached
fi

rm -rf "${BINUTILS_DIR}"
rm -rf "${BINUTILS_BUILD_DIR}"

mkdir -p "${BINUTILS_DIR}"
mkdir -p "${BINUTILS_BUILD_DIR}"
BINUTILS_BUILD_DIR=$(readlink -f "${BINUTILS_BUILD_DIR}")

tar -xjf "${BINUTILS_OUTPUT}" -C "${BINUTILS_DIR}" --strip-components=1

# Echo all commands
set -x

NUM_JOBS=3
cd "${BINUTILS_DIR}"
if [[ ! -f ./config.status ]]; then
	./configure --prefix="${BINUTILS_BUILD_DIR}" \
			--enable-gold \
			--enable-plugins
fi

make all -j"${NUM_JOBS}"
make install
cd -

# Force the gold linker
rm -f "${BINUTILS_BUILD_DIR}/bin/ld"
ln "${BINUTILS_BUILD_DIR}/bin/ld.gold" "${BINUTILS_BUILD_DIR}/bin/ld"

echo "${BINUTILS_SNAPSHOT}" >"${STAMP_FILE}"
