#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include "cmockery.h"
#include "libpq-int.h"

#include "../check_nondefault_extensions.h"

#include "check_nondefault_extensions_mock.c"

/*
 * Prepares a PGresult object with one column and 'length' rows, whose values
 * are strings from 'extensions'
 */
static PGresult *
prepare_pgresult(const char *const *extensions, size_t length)
{
	PGresAttValue **tuples = palloc(sizeof(PGresAttValue *) * length);

	for (size_t i = 0; i < length; i++)
	{
		PGresAttValue *tuple = palloc(sizeof(PGresAttValue));

		tuple->value = palloc(strlen(extensions[i]) + 1);
		memcpy(tuple->value, extensions[i], strlen(extensions[i]) + 1);
		tuple->len = strlen(extensions[i]);

		tuples[i] = tuple;
	}

	PGresult   *result = palloc(sizeof(PGresult));

	result->ntups = length;
	result->numAttributes = 1;
	result->tuples = tuples;

	return result;
}

static void
test_fails_when_there_is_nondefault_extension()
{
	DbInfo		dbi;

	dbi.db_name = "test";
	old_cluster.dbarr.ndbs = 1;
	old_cluster.dbarr.dbs = &dbi;

	const char *SQL_RESULT[] = {"generic-extension"};
	PGresult   *result = prepare_pgresult(SQL_RESULT, sizeof(SQL_RESULT) / sizeof(SQL_RESULT[0]));

	will_return(executeQueryOrDie, result);

	if (0 == setjmp(fatal_caller))
	{
		check_nondefault_extensions();
	}

	assert_true(check_pg_fatal());
}

static void
test_fails_when_there_are_nondefault_extensions()
{
	DbInfo		dbi;

	dbi.db_name = "test";
	old_cluster.dbarr.ndbs = 1;
	old_cluster.dbarr.dbs = &dbi;

	const char *SQL_RESULT[] = {"generic-extension", "generic-extension-2"};
	PGresult   *result = prepare_pgresult(SQL_RESULT, sizeof(SQL_RESULT) / sizeof(SQL_RESULT[0]));

	will_return(executeQueryOrDie, result);

	if (0 == setjmp(fatal_caller))
	{
		check_nondefault_extensions();
	}

	assert_true(check_pg_fatal());
}

static void
test_succeeds_when_there_are_only_default_extensions()
{
	DbInfo		dbi;

	dbi.db_name = "test";
	old_cluster.dbarr.ndbs = 1;
	old_cluster.dbarr.dbs = &dbi;

	const char *SQL_RESULT[] = {};
	PGresult   *result = prepare_pgresult(SQL_RESULT, sizeof(SQL_RESULT) / sizeof(SQL_RESULT[0]));

	will_return(executeQueryOrDie, result);

	check_nondefault_extensions();

	assert_false(check_pg_fatal());
}

int
main(int argc, char *argv[])
{
	cmockery_parse_arguments(argc, argv);

	const		UnitTest tests[] = {
		unit_test_setup_teardown(test_fails_when_there_is_nondefault_extension, mock_setup, mock_teardown),
		unit_test_setup_teardown(test_fails_when_there_are_nondefault_extensions, mock_setup, mock_teardown),
		unit_test_setup_teardown(test_succeeds_when_there_are_only_default_extensions, mock_setup, mock_teardown),
	};

	return run_tests(tests);
}
