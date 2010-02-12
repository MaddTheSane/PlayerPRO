OS=$(shell uname -s)
ifeq ($(OS), Linux)
include Makefile-linux
else
ifeq ($(OS), Darwin)
include Makefile-Mac
else
include Makefile-win32
endif
endif
