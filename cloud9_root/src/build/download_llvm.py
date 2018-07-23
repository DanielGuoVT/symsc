#!/usr/bin/env python
#
# Copyright notice here

"""Set up LLVM."""

__author__ = "stefan.bucur@epfl.ch (Stefan Bucur)"


import argparse
import os
import subprocess


# Do NOT CHANGE this if you don't know what you're doing
clang_revision = 158819

script_dir = os.path.dirname(__file__)
cloud9_dir = os.path.join(script_dir, os.pardir)

llvm_dir = os.path.join(cloud9_dir, "third_party/llvm")
llvm_build_dir = os.path.join(cloud9_dir, "third_party/llvm-build")
clang_dir = os.path.join(llvm_dir, "tools/clang")
compiler_rt_dir = os.path.join(llvm_dir, "projects/compiler_rt")

stamp_file = os.path.join(llvm_build_dir, "cr_build_revision")

llvm_repo_url = (os.environ.get("LLVM_URL")
                 or "https://llvm.org/svn/llvm-project")


repo_paths = {
  "trunk": {
    "llvm_repo_path": "llvm/trunk",
    "clang_repo_path": "cfe/trunk",
    "compiler_rt_repo_path": "compiler-rt/trunk",
  },

  "rel31": {
    "llvm_repo_path": "llvm/tags/RELEASE_31/final",
    "clang_repo_path": "cfe/tags/RELEASE_31/final",
    "compiler_rt_repo_path": "compiler-rt/tags/RELEASE_31/final",
  },
}

def CheckoutSVNRepo(name, url, revision, checkout_dir):
  print "Getting %s r%d in %s" % (name, revision, checkout_dir)
  if subprocess.call(["svn", "co", "--force",
                      "%s@%d" % (url, revision),
                      checkout_dir]) == 0:
    return

  print "Checkout failed, retrying"
  subprocess.call(["rm", "-rf", checkout_dir])
  subprocess.call(["svn", "co", "--force",
                   "%s@%d" % (url, revision),
                   checkout_dir])


def Main():
  parser = argparse.ArgumentParser(description="Download and/or install LLVM.")
  parser.add_argument("--bootstrap", action="store_true", default=False)
  parser.add_argument("--force-local-build", action="store_true", default=False)
  parser.add_argument("--debug-build", action="store_true", default=False)
  parser.add_argument("--configuration",
                      choices=sorted(repo_paths.keys()), default="trunk")

  args = parser.parse_args()

  try:
    with open(stamp_file, "r") as f:
      last_revision = f.read().strip()
  except IOError:
    last_revision = None

  if str(clang_revision) == last_revision:
    print "Clang already at %s" % clang_revision
    exit(0)

  try:
    os.remove(stamp_file)
  except OSError:
    pass

  CheckoutSVNRepo("LLVM", "%s/%s" % (llvm_repo_url,
                                     repo_paths[args.configuration]["llvm_repo_path"]),
                  clang_revision, llvm_dir)
  CheckoutSVNRepo("Clang", "%s/%s" % (llvm_repo_url,
                                      repo_paths[args.configuration]["clang_repo_path"]),
                  clang_revision, clang_dir)
  CheckoutSVNRepo("compiler-rt", "%/%s" % (llvm_repo_url,
                                           repo_paths[args.configuration]["compiler_rt_repo_path"]),
                  clang_revision, compiler_rt_dir)

  num_jobs = int(subprocess.check_output(["grep", "-c", "^processor",
                                          "/proc/cpuinfo"]))


if __name__ == "__main__":
  Main()
