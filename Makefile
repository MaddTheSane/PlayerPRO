OS=$(shell uname -s)
ifeq ($(OS), Darwin)
include Makefile-Mac
else



include Makefile-other
endif
