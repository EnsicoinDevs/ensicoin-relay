IDIR=include
BDIR=bin

SUFFIXES += .d
NODEPS:=clean tags svn
SOURCES:=$(shell find src/ -name "*.cpp")
DEPFILES:=$(patsubst %.cpp,%.d,$(SOURCES))

CXX=g++
CXXFLAGS=-I$(IDIR) -Wshadow -Wall -Wextra -DASIO_STANDALONE
OFLAGS=-O3 -march=native
LIBS=-pthread

ODIR=obj

OBJS:=$(patsubst %.cpp, obj/%.o, $(shell find src/ -name '*.cpp' -type f -printf "%f "))

ensicoin-relay: $(OBJS)
	$(CXX) -o $(BDIR)/$@ $^ $(CXXFLAGS) $(LIBS)

include Rulefile

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(BDIR)/*
