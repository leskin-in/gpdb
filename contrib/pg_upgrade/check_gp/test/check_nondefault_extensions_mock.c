#include <setjmp.h>
#include "pg_upgrade.h"
#include "cmockery.h"


ClusterInfo old_cluster;
jmp_buf		fatal_caller;

bool		check_pg_fatal(void);
void		mock_setup(void **);
void		mock_teardown(void **);


void
prep_status(const char *fmt,...)
{
}

PGconn *
connectToServer(ClusterInfo *cluster, const char *db_name)
{
	return NULL;
}

PGresult *
executeQueryOrDie(PGconn *conn, const char *fmt,...)
{
	return (PGresult *) mock();
}

void
pg_log(eLogType type, const char *fmt,...)
{
}

void
check_ok(void)
{
}


/* pg_fatal is a controlled mocked function */

bool		fatal_called = false;

bool
check_pg_fatal()
{
	return fatal_called;
}

void
pg_fatal(const char *fmt,...)
{
	fatal_called = true;

	/* have to keep our promise of "noreturn" */
	longjmp(fatal_caller, 1);
}


/*
 * This must be called before each test
 */
void
mock_setup(void **_)
{
	fatal_called = false;
}

void
mock_teardown(void **_)
{
}
