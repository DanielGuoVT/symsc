#!/usr/bin/env python
#
# Copyright 2012 EPFL. All rights reserved.

"""Manages a GCE cloud running Cloud9."""

__author__ = "stefan.bucur@epfl.ch (Stefan Bucur)"


import argparse
import json
import logging
import os
import sys
import time


try:
  from gcelib import gce_util
  from gcelib import gce_v1beta12
  from gcelib import shortcuts
except ImportError:
  print "ERROR: You need to have the GCE library installed."
  print "https://developers.google.com/storage/docs/gsutil_install"
  sys.exit(1)
  

CLOUD9_PROJECT = "cloud9-gce"
CLOUD9_IMAGE = "cloud9-base"

STARTUP_SCRIPT_PATH = os.path.join(os.path.dirname(__file__),
                                   "startup_script.sh")

class Cloud9Node(object):
  def __init__(self):
    pass


class Cloud9Manager(object):
  def __init__(self, nodes):
    self.nodes = nodes


class GCEManager(object):
  def __init__(self):
    self.api = None
  
  def Initialize(self):
    credentials = gce_util.get_credentials()
    
    self.api = gce_v1beta12.GoogleComputeEngine(
        credentials,
        default_project=CLOUD9_PROJECT,
        default_zone="us-east1-a",
        default_image=CLOUD9_IMAGE,
        default_machine_type="n1-standard-1")
  
  def _GetNewInstanceNames(self, count=1):
    base_name = "inst-%d" % int(time.time()*1000)
    return ["%s-%d" % (base_name, i) for i in range(count)]
  
  def GetInstances(self):
    return self.api.all_instances(filter="image eq '.*%s'" % CLOUD9_IMAGE)
  
  def CountInstances(self):
    return sum(1 for _ in self.GetInstances())
  
  def AddInstances(self, count=1):
    with open(STARTUP_SCRIPT_PATH, "r") as f:
      startup_script = f.read()
      
    metadata = [
      {
        "key": "startup-script",
        "value": startup_script,
      }
    ]
    
    svc_accounts = [
      "https://www.googleapis.com/auth/devstorage.read_write",
    ]
     
    self.api.insert_instances(
        names=self._GetNewInstanceNames(count),
        networkInterfaces=shortcuts.network(),
        metadata=gce_v1beta12.Metadata(metadata),
        serviceAccounts=gce_v1beta12.ServiceAccount("default", svc_accounts))
    
  def RemoveInstances(self, count=1):
    del_instances = []
    for instance in self.GetInstances():
      if count <= 0:
        break
      del_instances.append(instance)
      count -= 1
      
    if del_instances:
      self.api.delete_instances(del_instances)


def HandleAdd(args):
  gce_manager = GCEManager()
  gce_manager.Initialize()
  
  total_inst_count = gce_manager.CountInstances()
  print "Found %d instances running" % total_inst_count
  
  new_inst_count = 0
  
  if args.min_cap >= 0:
    new_inst_count = max(args.min_cap, total_inst_count) - total_inst_count
    if args.count >= 0:
      new_inst_count = min(args.count, new_inst_count)
  elif args.count >= 0:
    new_inst_count = args.count
    
  if not new_inst_count:
    print "No new instances are being created"
    return
  
  print "Creating %d new instances" % new_inst_count
  
  gce_manager.AddInstances(new_inst_count)


def HandleRemove(args):
  gce_manager = GCEManager()
  gce_manager.Initialize()
  
  total_inst_count = gce_manager.CountInstances()
  print "Found %d instances running" % total_inst_count
  
  del_inst_count = 0
  
  if args.max_cap >= 0:
    del_inst_count = total_inst_count - min(args.max_cap, total_inst_count)
    if args.count >= 0:
      del_inst_count = min(args.count, del_inst_count)
  elif args.count >= 0:
    del_inst_count = args.count
    
  if not del_inst_count:
    print "No instances are deleted"
    return
  
  print "Removing %d existing instances" % del_inst_count
    
  gce_manager.RemoveInstances(del_inst_count)
  
  
def HandleList(args):
  gce_manager = GCEManager()
  gce_manager.Initialize()

  if args.print_cloud9:
    data = []
    for instance in gce_manager.GetInstances():
      data.append({
        "name": instance.name,
        "host": instance.networkInterfaces[0].accessConfigs[0].natIP,
        "cores": 1,
        "root": "/opt/cloud9",
        "user": "bucur",
        "expdir": "/var/cloud9",
      })
    json.dump(data, sys.stdout, indent=2)
  else:
    for instance in gce_manager.GetInstances():
      print instance.name
    

def HandleRun(args):
  print args


def Main():  
  parser = argparse.ArgumentParser(description="GCE cloud management")
  parser.add_argument("-v", "--verbose", action="store_true", default=False,
                      help="Show low-level information")
  
  subparsers = parser.add_subparsers(help="Management operations")
  
  add_parser = subparsers.add_parser("add",
                                     help="Add more nodes")
  add_parser.add_argument("count", type=int, nargs="?")
  add_parser.add_argument("--min-cap", type=int,
                          help="Maximum limit of total instances.")
  add_parser.set_defaults(handler=HandleAdd)

  remove_parser = subparsers.add_parser("remove",
                                        help="Remove nodes")
  remove_parser.add_argument("count", type=int, nargs="?")
  remove_parser.add_argument("--max-cap", type=int,
                             help="Minimum limit of total instances.")
  remove_parser.set_defaults(handler=HandleRemove)
  
  list_parser = subparsers.add_parser("list",
                                      help="List running instances")
  list_parser.add_argument("--print-cloud9", action="store_true", default=False,
                           help="Print instances in Cloud9 format.")
  list_parser.set_defaults(handler=HandleList)
  
  run_parser = subparsers.add_parser("run",
                                     help="Run Cloud9 command")
  run_parser.add_argument("cmdline", nargs=argparse.REMAINDER)
  run_parser.set_defaults(handler=HandleRun)
  
  args = parser.parse_args()
  
  logging.basicConfig(level=logging.INFO if args.verbose else logging.WARN)
  args.handler(args)


if __name__ == "__main__":
  Main()
