LIBADD = $(LISRV_LIBS)
AM_CFLAGS = $(LISRV_CFLAGS) -I$(top_srcdir)
AM_LDFLAGS = -module -avoid-version -no-undefined
EXTRA_DIST = skeleton/mkext.sh

extdir = $(EXTSDIR)
ext_LTLIBRARIES = \
	libcreature.la \
	libnetwork.la \
	libnpc.la \
	libskeleton.la \
	libspawner.la

libcreature_la_LIBADD = $(LIBADD)
libcreature_la_SOURCES = \
	creature/ext-creature.c \
	creature/ext-creature.h \
	creature/ext-module.c \
	creature/ext-module.h \
	creature/ext-script.c

libnetwork_la_LIBADD = $(LIBADD)
libnetwork_la_SOURCES = \
	network/ext-module.c \
	network/ext-module.h \
	network/ext-client.c \
	network/ext-client.h \
	network/ext-script.c

libnpc_la_LIBADD = $(LIBADD)
libnpc_la_SOURCES = \
	npc/ext-module.c \
	npc/ext-module.h \
	npc/ext-npc.c \
	npc/ext-npc.h \
	npc/ext-script.c

libskeleton_la_LIBADD = $(LIBADD)
libskeleton_la_SOURCES = \
	skeleton/ext-module.c \
	skeleton/ext-module.h \
	skeleton/ext-script.c

libspawner_la_LIBADD = $(LIBADD)
libspawner_la_SOURCES = \
	spawner/ext-module.c \
	spawner/ext-module.h \
	spawner/ext-spawner.c \
	spawner/ext-spawner.h \
	spawner/ext-script.c

all-local:
	@if test "x$(RELPATH)" = "xyes"; then \
		mkdir -p ../../../build/lib/extensions; \
		for i in $(ext_LTLIBRARIES); do \
			name=`echo $$i | sed s/la$$/so/`; \
			if test -e .libs/$$name; then cp .libs/$$name ../../../build/lib/extensions; fi; \
		done; \
	fi
