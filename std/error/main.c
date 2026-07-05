#include "error.h"

#include "../result/result.h"
#include "../test/assert.h"

#include <string.h>

typedef struct {
  const char *data;
  size_t length;
} StringView;

typedef struct {
  StringView option;
  size_t argument_index;
} UnknownOptionData;

#define CLI_ERRORS(Name, X)                                                    \
  X(Name, UNKNOWN_OPTION, UnknownOptionData,                                   \
    "unknown option '%.*s' at argument %zu", (int)data.option.length,          \
    data.option.data, data.argument_index)                                     \
  X(Name, OUT_OF_MEMORY, GcNoErrorData, "out of memory")

GC_DEFINE_ERROR(CliError, CLI_ERRORS)
DEFINE_RESULT(ParseResult, int, CliError);

int main(void) {
  CliError unknown = GC_ERROR(CliError, UNKNOWN_OPTION,
                              ((UnknownOptionData){
                                  .option = {.data = "--wat", .length = 5},
                                  .argument_index = 3,
                              }));

  GC_ASSERT(CliError_is(unknown, CliError_UNKNOWN_OPTION));
  GC_ASSERT(!CliError_is(unknown, CliError_OUT_OF_MEMORY));
  GC_ASSERT(strcmp(CliError_kind_name(unknown), "UNKNOWN_OPTION") == 0);

  char buffer[128];
  int length = CliError_format(buffer, sizeof buffer, unknown);
  GC_ASSERT(length == 36);
  GC_ASSERT(strcmp(buffer, "unknown option '--wat' at argument 3") == 0);

  CliError no_memory = GC_ERROR_NO_DATA(CliError, OUT_OF_MEMORY);
  GC_ASSERT(CliError_is(no_memory, CliError_OUT_OF_MEMORY));
  GC_ASSERT(CliError_format(buffer, sizeof buffer, no_memory) == 13);
  GC_ASSERT(strcmp(buffer, "out of memory") == 0);

  FILE *stream = tmpfile();
  GC_ASSERT(stream != NULL);
  GC_ASSERT(CliError_print(stream, unknown) == length);
  rewind(stream);
  GC_ASSERT(fgets(buffer, sizeof buffer, stream) != NULL);
  GC_ASSERT(strcmp(buffer, "unknown option '--wat' at argument 3") == 0);
  fclose(stream);

  ParseResult result = RESULT_ERR(ParseResult, unknown);
  GC_ASSERT(result_is_err(result));
  GC_ASSERT(CliError_is(unwrap_err(result), CliError_UNKNOWN_OPTION));
}
