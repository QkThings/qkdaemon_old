#!/usr/bin/python

from os import getcwd, chdir, path, environ
from distutils.dir_util import copy_tree
from subprocess import call

def cp(root_src,root_dest,rel_path):
	copy_tree(path.join(root_src, rel_path), path.join(root_dest, rel_path))
	
	

def deploy():
	rootdir = getcwd()
	
#	environ["LD_LIBRARY_PATH"] += path.join(rootdir, "../../qkcore/release")
	environ["LD_LIBRARY_PATH"] += "/home/mribeiro/bitbucket/qkthings/software/qkcore/release"

	print "Done"


if __name__ == "__main__":
	deploy();


