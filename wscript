#!/usr/bin/env python

APPNAME = 'elog'
VERSION = '0.3.0'

top = '.'
out = 'build'

def options(opt):
  opt.load('compiler_cxx unittest_gtest')

def configure(cnf):
  cnf.load('compiler_cxx unittest_gtest')
  cnf.env.CXXFLAGS = ['std=c++0x', '-g', '-W', '-Wall', '-O3']

def build(bld):
  bld.recurse('src')
