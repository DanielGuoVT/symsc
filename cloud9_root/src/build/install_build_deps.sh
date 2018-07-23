#!/bin/bash -e

# Adapted from Chromium's install-build-deps.sh

if ! egrep -q \
    'Ubuntu (10\.04|10\.10|11\.04|11\.10|lucid|maverick|natty|oneiric)' \
    /etc/issue; then
  echo "Only Ubuntu 10.04 (lucid) through 11.10 (oneiric) are currently" \
      "supported" >&2
  exit 1
fi

if ! uname -m | egrep -q "x86_64"; then
  echo "Only x86_64 architecture is currently supported" >&2
  exit
fi

if [ "x$(id -u)" != x0 ]; then
  echo "Running as non-root user."
  echo "You might have to enter your password one or more times for 'sudo'."
  echo
fi

DEV_LIST="dejagnu flex bison protobuf-compiler libprotobuf-dev
          libboost-thread-dev libboost-system-dev build-essential"

# TODO: Determine the run-time libraries

sudo apt-get update

# We initially run "apt-get" with the --reinstall option and parse its output.
# This way, we can find all the packages that need to be newly installed
# without accidentally promoting any packages from "auto" to "manual".
# We then re-run "apt-get" with just the list of missing packages.
echo "Finding missing packages..."
packages="${DEV_LIST}"
# Intentially leaving $packages unquoted so it's more readable.
echo "Packages required: " $packages
echo
new_list_cmd="sudo apt-get install --reinstall $(echo $packages)"
if new_list="$(yes n | LANG=C $new_list_cmd)"; then
  # We probably never hit this following line.
  echo "No missing packages, and the packages are up-to-date."
elif [ $? -eq 1 ]; then
  # We expect apt-get to have exit status of 1.
  # This indicates that we canceled the install with "yes n|".
  new_list=$(echo "$new_list" |
    sed -e '1,/The following NEW packages will be installed:/d;s/^  //;t;d')
  new_list=$(echo "$new_list" | sed 's/ *$//')
  if [ -z "$new_list" ] ; then
    echo "No missing packages, and the packages are up-to-date."
  else
    echo "Installing missing packages: $new_list."
    sudo apt-get install ${new_list}
  fi
  echo
else
  # An apt-get exit status of 100 indicates that a real error has occurred.

  # I am intentionally leaving out the '"'s around new_list_cmd,
  # as this makes it easier to cut and paste the output
  echo "The following command failed: " ${new_list_cmd}
  echo
  echo "It produces the following output:"
  yes n | $new_list_cmd || true
  echo
  echo "You will have to install the above packages yourself."
  echo
  exit 100
fi