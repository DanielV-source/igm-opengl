## Makefile

CXXFLAGS=-Wl,--copy-dt-needed-entries -Wall -losg -losgViewer -losgDB
CXX=g++

all: spinningcube_osg

spinningcube_osg: spinningcube_osg.cpp
	$(CXX) -o $@ $< $(CXXFLAGS)