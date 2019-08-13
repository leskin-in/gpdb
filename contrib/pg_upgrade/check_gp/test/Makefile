subdir=contrib/pg_upgrade/check_gp
top_builddir=../../../..

include $(top_builddir)/src/Makefile.global

TARGETS=check_nondefault_extensions

include $(top_builddir)/src/Makefile.mock

override CPPFLAGS := $(CPPFLAGS) -I../.. -I$(libpq_srcdir)

check_nondefault_extensions.t: check_nondefault_extensions_test.o
check_nondefault_extensions.t: $(CMOCKERY_OBJS) $(top_srcdir)/$(subdir)/check_nondefault_extensions.o
	$(CC) $(CFLAGS) $(LDFLAGS) $(CPPFLAGS) \
		$^ \
		$(libpq_pgport) \
		-o check_nondefault_extensions.t
