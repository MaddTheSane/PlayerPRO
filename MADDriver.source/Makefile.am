# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

# Shared libraries built in this directory
lib_LTLIBRARIES = libPlayerPRO.la

LIBRARY_CURRENT    = @LIBRARY_CURRENT@
LIBRARY_REVISION   = @LIBRARY_REVISION@
LIBRARY_AGE        = @LIBRARY_AGE@

# INCLUDES = -I$(top_builddir)/include -I$(top_srcdir)/include

libPlayerPRO_la_LDFLAGS = -no-undefined \
  -version-info $(LIBRARY_CURRENT):$(LIBRARY_REVISION):$(LIBRARY_AGE)

libPlayerPRO_la_LIBADD = $(PLAYERPRO_LIB_DEPLIBS)

# libPlayerPRO_la_CFLAGS = -O3

libPlayerPRO_la_SOURCES = \
ByteSwap.c RDriver.h\
DelayOutPut.c MainDriver.c RDriverInt.h\
Effects.c MyDebugStr.c TickRemover.c\
FileUtils.h \
Interrupt.c \
Win-FileUtils.c\
MAD.h OutPut8.c\
PPPlug.h\
PPPrivate.h realft.c\
PlayerPROCore.h

libPlayerPRO_la_SOURCES_OSS = UNIX-OSS.c

libPlayerPRO_la_SOURCES_ESD = UNIX-esound.c

libPlayerPRO_la_SOURCES_ALSA = Lin-alsa.c

libPlayerPRO_la_SOURCES_WIN = Win-WaveDriver.c Win-PlugImport.c

libPlayerPRO_la_SOURCES_UNIX = Lin-PlugImport.c

