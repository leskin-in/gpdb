#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include "cmockery.h"
#include "libpq-int.h"

#include "../check_nondefault_extensions.h"


static void
test_fails_when_there_is_nondefault_extension()
{

	bool

	assert_false
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
