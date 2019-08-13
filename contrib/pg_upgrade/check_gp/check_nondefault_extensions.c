#include "pg_upgrade.h"

#include "check_nondefault_extensions.h"

/*
 * Takes an array and prints it using hardcoded separator and quote.
 * It is caller's responsibility to call pfree() on return value
 */
static char *
array_to_string(const char *const array[], size_t length)
{
	const char *const SEPARATOR = ", ";
	const char *const QUOTE = "'";

	char	   *result;
	char	   *result_current_pointer;
	size_t		length_total = 1;
	size_t		i;

	if (length == 0)
	{
		result = palloc0(1);
		return result;
	}

	for (i = 0; i < length; i++)
	{
		length_total += strlen(array[i]);
		length_total += strlen(QUOTE) * 2;
		length_total += strlen(SEPARATOR);
	}

	length_total -= strlen(SEPARATOR);

	result = palloc(length_total);
	result_current_pointer = result;

	for (i = 0; i < length; i++)
	{
		int			sprintf_status;

		if (i > 0)
		{
			if ((sprintf_status = sprintf(result_current_pointer, "%s", SEPARATOR)) < 0)
				pg_fatal("sprintf() failed\n");
			result_current_pointer += sprintf_status;
		}

		if ((sprintf_status = sprintf(result_current_pointer, "%s%s%s", QUOTE, array[i], QUOTE)) < 0)
			pg_fatal("sprintf() failed\n");
		result_current_pointer += sprintf_status;
	}

	return result;
}

void
check_nondefault_extensions(void)
{
	const char *const DEFAULT_EXTENSIONS[] = {"plpgsql", "plperlu"};
	const char *const SEPARATOR = "\n";
	int			dbnum;
	char	   *report = palloc0(1);
	char	   *extensions_list = array_to_string(DEFAULT_EXTENSIONS, sizeof(DEFAULT_EXTENSIONS) / sizeof(*DEFAULT_EXTENSIONS));

	prep_status("Checking for non-default extensions");

	for (dbnum = 0; dbnum < old_cluster.dbarr.ndbs; dbnum++)
	{
		PGresult   *res;
		DbInfo	   *active_db = &old_cluster.dbarr.dbs[dbnum];
		PGconn	   *conn;
		int			i;

		conn = connectToServer(&old_cluster, active_db->db_name);
		res = executeQueryOrDie(conn, "SELECT extname FROM pg_extension WHERE extname NOT IN (%s);", extensions_list);

		for (i = 0; i < PQntuples(res); i++)
		{
			char	   *extension_name = PQgetvalue(res, i, 0);

			report = repalloc(report,
							  strlen(report) + strlen(extension_name) + strlen(SEPARATOR) + 1);
			sprintf(
					&(report[strlen(report)]),
					"%s%s",
					extension_name, SEPARATOR
				);
		}

		PQclear(res);
		PQfinish(conn);
	}

	if (strlen(report))
	{
		pg_log(PG_REPORT, "fatal\n");
		pg_fatal(
				 "All non-default extensions must be dropped before the upgrade.\n"
				 "Non-default extensions found:\n%s",
				 report
			);
	}

	pfree(extensions_list);
	pfree(report);
	check_ok();
}
