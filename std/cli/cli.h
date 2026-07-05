#ifndef GC_CLI_H
#define GC_CLI_H

#include "../array/vector.h"
#include "../error/error.h"
#include "../option/option.h"
#include "../result/result.h"

#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char *data;
  size_t length;
} GcStringView;

#define GC_STRING_VIEW(literal)                                                \
  ((GcStringView){.data = (literal), .length = sizeof(literal) - 1})

static inline GcStringView gc_string_view_from_c(const char *string) {
  return (GcStringView){.data = string, .length = strlen(string)};
}

static inline bool gc_string_view_equal(GcStringView left, GcStringView right) {
  return left.length == right.length &&
         memcmp(left.data, right.data, left.length) == 0;
}

DEFINE_OPTION(GcStringView)

typedef struct {
  GcStringView token;
  size_t argument_index;
} GcCliTokenError;

typedef struct {
  int system_error;
} GcCliIoError;

typedef struct {
  GcStringView value;
  const char *expected;
} GcCliValueError;

#define GC_CLI_ERRORS(Name, X)                                                 \
  X(Name, UNKNOWN_OPTION, GcCliTokenError,                                     \
    "unknown option '%.*s' at argument %zu", (int)data.token.length,           \
    data.token.data, data.argument_index)                                      \
  X(Name, MISSING_VALUE, GcCliTokenError,                                      \
    "missing value for option '%.*s' at argument %zu", (int)data.token.length, \
    data.token.data, data.argument_index)                                      \
  X(Name, UNEXPECTED_VALUE, GcCliTokenError,                                   \
    "option '%.*s' does not accept a value at argument %zu",                   \
    (int)data.token.length, data.token.data, data.argument_index)              \
  X(Name, READ_FAILED, GcCliIoError, "failed to read input: %s",               \
    strerror(data.system_error))                                               \
  X(Name, INVALID_VALUE, GcCliValueError, "invalid %s '%.*s'", data.expected,  \
    (int)data.value.length, data.value.data)                                   \
  X(Name, OUT_OF_RANGE, GcCliValueError, "%s out of range: '%.*s'",            \
    data.expected, (int)data.value.length, data.value.data)

GC_DEFINE_ERROR(GcCliError, GC_CLI_ERRORS)

typedef struct {
  Vec(char) buffer;
} GcLineReader;

DEFINE_RESULT(GcReadLineResult, Option(GcStringView), GcCliError);

static inline GcReadLineResult gc_read_line(GcLineReader *reader,
                                            FILE *stream) {
  vec_clear(reader->buffer);

  int character;
  while ((character = fgetc(stream)) != EOF && character != '\n')
    vec_push(reader->buffer, (char)character);

  if (character == EOF && ferror(stream)) {
    int system_error = errno ? errno : EIO;
    return RESULT_ERR(GcReadLineResult,
                      GC_ERROR(GcCliError, READ_FAILED,
                               ((GcCliIoError){.system_error = system_error})));
  }

  if (character == EOF && vec_len(reader->buffer) == 0)
    return RESULT_OK(GcReadLineResult, OPTION_NONE(GcStringView));

  if (vec_len(reader->buffer) > 0 &&
      reader->buffer[vec_len(reader->buffer) - 1] == '\r')
    (void)vec_pop(reader->buffer);

  size_t length = vec_len(reader->buffer);
  vec_push(reader->buffer, '\0');
  GcStringView line = {.data = reader->buffer, .length = length};
  return RESULT_OK(GcReadLineResult, OPTION_SOME(GcStringView, line));
}

static inline void gc_line_reader_free(GcLineReader *reader) {
  vec_free(reader->buffer);
}

typedef enum {
  GC_OPTION_NONE,
  GC_OPTION_REQUIRED,
  GC_OPTION_OPTIONAL,
} GcOptionRequirement;

typedef struct {
  int id;
  char short_name;
  GcStringView long_name;
  GcOptionRequirement argument;
} GcOptionSpec;

typedef struct {
  int id;
  Option(GcStringView) value;
} GcParsedOption;

typedef struct {
  Vec(GcParsedOption) options;
  Vec(GcStringView) positionals;
} GcCliArgs;

DEFINE_RESULT(GcCliArgsResult, GcCliArgs, GcCliError);

static inline const GcOptionSpec *gc_cli_find_long(const GcOptionSpec *specs,
                                                   size_t spec_count,
                                                   GcStringView name) {
  for (size_t index = 0; index < spec_count; ++index) {
    if (gc_string_view_equal(specs[index].long_name, name))
      return &specs[index];
  }
  return NULL;
}

static inline const GcOptionSpec *
gc_cli_find_short(const GcOptionSpec *specs, size_t spec_count, char name) {
  for (size_t index = 0; index < spec_count; ++index) {
    if (specs[index].short_name == name)
      return &specs[index];
  }
  return NULL;
}

static inline GcCliArgsResult gc_cli_parse_error(GcCliArgs *args,
                                                 GcCliErrorKind kind,
                                                 GcStringView token,
                                                 size_t argument_index) {
  vec_free(args->options);
  vec_free(args->positionals);
  GcCliTokenError data = {
      .token = token,
      .argument_index = argument_index,
  };

  if (kind == GcCliError_UNKNOWN_OPTION)
    return RESULT_ERR(GcCliArgsResult,
                      GC_ERROR(GcCliError, UNKNOWN_OPTION, data));
  if (kind == GcCliError_MISSING_VALUE)
    return RESULT_ERR(GcCliArgsResult,
                      GC_ERROR(GcCliError, MISSING_VALUE, data));
  if (kind == GcCliError_UNEXPECTED_VALUE)
    return RESULT_ERR(GcCliArgsResult,
                      GC_ERROR(GcCliError, UNEXPECTED_VALUE, data));
  return RESULT_ERR(GcCliArgsResult,
                    GC_ERROR(GcCliError, UNKNOWN_OPTION, data));
}

static inline void gc_cli_push_option(GcCliArgs *args, const GcOptionSpec *spec,
                                      Option(GcStringView) value) {
  vec_push(args->options, ((GcParsedOption){.id = spec->id, .value = value}));
}

static inline const char *gc_cli_find_separator(const char *text) {
  while (*text && *text != '=')
    ++text;
  return *text == '=' ? text : NULL;
}

static inline GcCliArgsResult gc_cli_parse(int argc, char **argv,
                                           const GcOptionSpec *specs,
                                           size_t spec_count) {
  GcCliArgs args = {0};
  bool positional_only = false;

  for (int index = 1; index < argc; ++index) {
    const char *text = argv[index];
    GcStringView token = gc_string_view_from_c(text);

    if (positional_only || text[0] != '-' || text[1] == '\0') {
      vec_push(args.positionals, token);
      continue;
    }

    if (strcmp(text, "--") == 0) {
      positional_only = true;
      continue;
    }

    if (text[1] == '-') {
      const char *name_start = text + 2;
      const char *separator = gc_cli_find_separator(name_start);
      size_t name_length =
          separator ? (size_t)(separator - name_start) : strlen(name_start);
      GcStringView name = {.data = name_start, .length = name_length};
      const GcOptionSpec *spec = gc_cli_find_long(specs, spec_count, name);
      if (!spec)
        return gc_cli_parse_error(&args, GcCliError_UNKNOWN_OPTION, token,
                                  (size_t)index);

      Option(GcStringView) value = OPTION_NONE(GcStringView);
      if (separator) {
        if (spec->argument == GC_OPTION_NONE)
          return gc_cli_parse_error(&args, GcCliError_UNEXPECTED_VALUE, token,
                                    (size_t)index);
        value = OPTION_SOME(GcStringView, gc_string_view_from_c(separator + 1));
      } else if (spec->argument == GC_OPTION_REQUIRED) {
        if (index + 1 >= argc)
          return gc_cli_parse_error(&args, GcCliError_MISSING_VALUE, token,
                                    (size_t)index);
        value = OPTION_SOME(GcStringView, gc_string_view_from_c(argv[++index]));
      }
      gc_cli_push_option(&args, spec, value);
      continue;
    }

    for (const char *cursor = text + 1; *cursor; ++cursor) {
      const GcOptionSpec *spec = gc_cli_find_short(specs, spec_count, *cursor);
      if (!spec)
        return gc_cli_parse_error(&args, GcCliError_UNKNOWN_OPTION, token,
                                  (size_t)index);

      Option(GcStringView) value = OPTION_NONE(GcStringView);
      if (spec->argument != GC_OPTION_NONE) {
        if (cursor[1] != '\0') {
          value = OPTION_SOME(GcStringView, gc_string_view_from_c(cursor + 1));
        } else if (spec->argument == GC_OPTION_REQUIRED) {
          if (index + 1 >= argc)
            return gc_cli_parse_error(&args, GcCliError_MISSING_VALUE, token,
                                      (size_t)index);
          value =
              OPTION_SOME(GcStringView, gc_string_view_from_c(argv[++index]));
        }
        gc_cli_push_option(&args, spec, value);
        break;
      }
      gc_cli_push_option(&args, spec, value);
    }
  }

  return RESULT_OK(GcCliArgsResult, args);
}

static inline void gc_cli_args_free(GcCliArgs *args) {
  vec_free(args->options);
  vec_free(args->positionals);
}

DEFINE_RESULT(GcIntResult, int, GcCliError);
DEFINE_RESULT(GcSizeResult, size_t, GcCliError);
DEFINE_RESULT(GcDoubleResult, double, GcCliError);
DEFINE_RESULT(GcBoolResult, bool, GcCliError);

static inline GcCliError gc_cli_value_error(GcCliErrorKind kind,
                                            GcStringView value,
                                            const char *expected) {
  GcCliValueError data = {.value = value, .expected = expected};
  if (kind == GcCliError_OUT_OF_RANGE)
    return GC_ERROR(GcCliError, OUT_OF_RANGE, data);
  return GC_ERROR(GcCliError, INVALID_VALUE, data);
}

static inline GcIntResult gc_parse_int(GcStringView input) {
  if (input.length == 0)
    return RESULT_ERR(GcIntResult, gc_cli_value_error(GcCliError_INVALID_VALUE,
                                                      input, "integer"));

  errno = 0;
  char *end;
  intmax_t value = strtoimax(input.data, &end, 10);
  if (end != input.data + input.length)
    return RESULT_ERR(GcIntResult, gc_cli_value_error(GcCliError_INVALID_VALUE,
                                                      input, "integer"));
  if (errno == ERANGE || value < INT_MIN || value > INT_MAX)
    return RESULT_ERR(GcIntResult, gc_cli_value_error(GcCliError_OUT_OF_RANGE,
                                                      input, "integer"));
  return RESULT_OK(GcIntResult, (int)value);
}

static inline GcSizeResult gc_parse_size(GcStringView input) {
  if (input.length == 0 || input.data[0] == '-')
    return RESULT_ERR(GcSizeResult, gc_cli_value_error(GcCliError_INVALID_VALUE,
                                                       input, "size"));

  errno = 0;
  char *end;
  uintmax_t value = strtoumax(input.data, &end, 10);
  if (end != input.data + input.length)
    return RESULT_ERR(GcSizeResult, gc_cli_value_error(GcCliError_INVALID_VALUE,
                                                       input, "size"));
  if (errno == ERANGE || value > SIZE_MAX)
    return RESULT_ERR(GcSizeResult, gc_cli_value_error(GcCliError_OUT_OF_RANGE,
                                                       input, "size"));
  return RESULT_OK(GcSizeResult, (size_t)value);
}

static inline GcDoubleResult gc_parse_double(GcStringView input) {
  if (input.length == 0)
    return RESULT_ERR(
        GcDoubleResult,
        gc_cli_value_error(GcCliError_INVALID_VALUE, input, "number"));

  errno = 0;
  char *end;
  double value = strtod(input.data, &end);
  if (end != input.data + input.length || !isfinite(value))
    return RESULT_ERR(
        GcDoubleResult,
        gc_cli_value_error(GcCliError_INVALID_VALUE, input, "number"));
  if (errno == ERANGE)
    return RESULT_ERR(
        GcDoubleResult,
        gc_cli_value_error(GcCliError_OUT_OF_RANGE, input, "number"));
  return RESULT_OK(GcDoubleResult, value);
}

static inline GcBoolResult gc_parse_bool(GcStringView input) {
  if (gc_string_view_equal(input, GC_STRING_VIEW("true")) ||
      gc_string_view_equal(input, GC_STRING_VIEW("yes")) ||
      gc_string_view_equal(input, GC_STRING_VIEW("1")))
    return RESULT_OK(GcBoolResult, true);
  if (gc_string_view_equal(input, GC_STRING_VIEW("false")) ||
      gc_string_view_equal(input, GC_STRING_VIEW("no")) ||
      gc_string_view_equal(input, GC_STRING_VIEW("0")))
    return RESULT_OK(GcBoolResult, false);
  return RESULT_ERR(GcBoolResult, gc_cli_value_error(GcCliError_INVALID_VALUE,
                                                     input, "boolean"));
}

#endif
