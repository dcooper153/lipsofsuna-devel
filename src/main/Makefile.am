INCLUDES = -I$(srcdir)/..
AM_CFLAGS = $(LIPS_CFLAGS) $(LUA_CFLAGS) $(GRAPPLE_CFLAGS) $(BULLET_CFLAGS)

bin_PROGRAMS = \
	lipsofsuna

lipsofsuna_SOURCES = \
	main.c

lipsofsuna_LDADD = \
	-L../system -llipssystem

install-data-local:
	$(mkinstalldirs) $(DATADIR)
	$(mkinstalldirs) $(SAVEDIR)

