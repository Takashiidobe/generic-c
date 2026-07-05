#include "cli.h"

#include "../test/assert.h"

#include <math.h>
#include <string.h>

enum {
  OPTION_VERBOSE,
  OPTION_QUIET,
  OPTION_OUTPUT,
};

static const GcOptionSpec options[] = {
    {.id = OPTION_VERBOSE,
     .short_name = 'v',
     .long_name = GC_STRING_VIEW("verbose"),
     .argument = GC_OPTION_NONE},
    {.id = OPTION_QUIET,
     .short_name = 'q',
     .long_name = GC_STRING_VIEW("quiet"),
     .argument = GC_OPTION_NONE},
    {.id = OPTION_OUTPUT,
     .short_name = 'o',
     .long_name = GC_STRING_VIEW("output"),
     .argument = GC_OPTION_REQUIRED},
};

static void test_line_reader(void) {
  FILE *stream = tmpfile();
  GC_ASSERT(stream != NULL);
  GC_ASSERT(fputs("first line\nsecond line\r\n", stream) >= 0);
  rewind(stream);

  GcLineReader reader = {0};
  GcReadLineResult result = gc_read_line(&reader, stream);
  GC_ASSERT(result_is_ok(result));
  Option(GcStringView) line = unwrap(result);
  GC_ASSERT(option_is_some(line));
  GC_ASSERT(
      gc_string_view_equal(unwrap_some(line), GC_STRING_VIEW("first line")));

  result = gc_read_line(&reader, stream);
  GC_ASSERT(result_is_ok(result));
  line = unwrap(result);
  GC_ASSERT(option_is_some(line));
  GC_ASSERT(
      gc_string_view_equal(unwrap_some(line), GC_STRING_VIEW("second line")));

  result = gc_read_line(&reader, stream);
  GC_ASSERT(result_is_ok(result));
  GC_ASSERT(option_is_none(unwrap(result)));

  gc_line_reader_free(&reader);
  fclose(stream);
}

static void test_argument_parser(void) {
  char *argv[] = {
      (char[]){"tool"},      (char[]){"-vq"}, (char[]){"--output=file.txt"},
      (char[]){"input.txt"}, (char[]){"--"},  (char[]){"-ignored"},
  };

  GcCliArgsResult result =
      gc_cli_parse(6, argv, options, sizeof options / sizeof options[0]);
  GC_ASSERT(result_is_ok(result));
  GcCliArgs args = unwrap(result);

  GC_ASSERT(vec_len(args.options) == 3);
  GC_ASSERT(args.options[0].id == OPTION_VERBOSE);
  GC_ASSERT(option_is_none(args.options[0].value));
  GC_ASSERT(args.options[1].id == OPTION_QUIET);
  GC_ASSERT(option_is_none(args.options[1].value));
  GC_ASSERT(args.options[2].id == OPTION_OUTPUT);
  GC_ASSERT(gc_string_view_equal(unwrap_some(args.options[2].value),
                                 GC_STRING_VIEW("file.txt")));

  GC_ASSERT(vec_len(args.positionals) == 2);
  GC_ASSERT(
      gc_string_view_equal(args.positionals[0], GC_STRING_VIEW("input.txt")));
  GC_ASSERT(
      gc_string_view_equal(args.positionals[1], GC_STRING_VIEW("-ignored")));
  gc_cli_args_free(&args);
}

static void test_argument_errors(void) {
  char *unknown_argv[] = {(char[]){"tool"}, (char[]){"--wat"}};
  GcCliArgsResult result = gc_cli_parse(2, unknown_argv, options,
                                        sizeof options / sizeof options[0]);
  GC_ASSERT(result_is_err(result));
  GC_ASSERT(GcCliError_is(unwrap_err(result), GcCliError_UNKNOWN_OPTION));

  char *missing_argv[] = {(char[]){"tool"}, (char[]){"--output"}};
  result = gc_cli_parse(2, missing_argv, options,
                        sizeof options / sizeof options[0]);
  GC_ASSERT(result_is_err(result));
  GC_ASSERT(GcCliError_is(unwrap_err(result), GcCliError_MISSING_VALUE));
}

static void test_value_parsing(void) {
  GcIntResult integer = gc_parse_int(GC_STRING_VIEW("-42"));
  GC_ASSERT(result_is_ok(integer));
  GC_ASSERT(unwrap(integer) == -42);
  GC_ASSERT(result_is_err(gc_parse_int(GC_STRING_VIEW("4x"))));

  GcSizeResult size = gc_parse_size(GC_STRING_VIEW("1024"));
  GC_ASSERT(result_is_ok(size));
  GC_ASSERT(unwrap(size) == 1024);
  GC_ASSERT(result_is_err(gc_parse_size(GC_STRING_VIEW("-1"))));

  GcDoubleResult number = gc_parse_double(GC_STRING_VIEW("3.25"));
  GC_ASSERT(result_is_ok(number));
  GC_ASSERT(fabs(unwrap(number) - 3.25) < 1e-12);

  GcBoolResult boolean = gc_parse_bool(GC_STRING_VIEW("yes"));
  GC_ASSERT(result_is_ok(boolean));
  GC_ASSERT(unwrap(boolean));
  GC_ASSERT(result_is_err(gc_parse_bool(GC_STRING_VIEW("maybe"))));
}

int main(void) {
  test_line_reader();
  test_argument_parser();
  test_argument_errors();
  test_value_parsing();
}
