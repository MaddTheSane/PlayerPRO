OS=$(shell uname -s)
ifeq ($(OS), Darwin)
include Makefile-Mac
else
# TODO: check for configure and Makefile-other and run neccesary tools to make them
include Makefile-other
endif
