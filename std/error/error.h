#ifndef GC_ERROR_H
#define GC_ERROR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

typedef struct {
  unsigned char unused;
} GcNoErrorData;

#define GC_ERROR_ENUM(Name, Variant, DataType, Format, ...) Name##_##Variant,

#define GC_ERROR_PAYLOAD(Name, Variant, DataType, Format, ...) DataType Variant;

#define GC_ERROR_KIND_CASE(Name, Variant, DataType, Format, ...)               \
  case Name##_##Variant:                                                       \
    return #Variant;

#define GC_ERROR_FORMAT_CASE(Name, Variant, DataType, Format, ...)             \
  case Name##_##Variant: {                                                     \
    DataType data = error.as.Variant;                                          \
    (void)data;                                                                \
    return snprintf(buffer, capacity, Format __VA_OPT__(, ) __VA_ARGS__);      \
  }

#define GC_ERROR_PRINT_CASE(Name, Variant, DataType, Format, ...)              \
  case Name##_##Variant: {                                                     \
    DataType data = error.as.Variant;                                          \
    (void)data;                                                                \
    return fprintf(stream, Format __VA_OPT__(, ) __VA_ARGS__);                 \
  }

#define GC_DEFINE_ERROR(Name, Errors)                                          \
  typedef enum { Errors(Name, GC_ERROR_ENUM) } Name##Kind;                     \
                                                                               \
  typedef struct Name {                                                        \
    Name##Kind kind;                                                           \
    union {                                                                    \
      Errors(Name, GC_ERROR_PAYLOAD)                                           \
    } as;                                                                      \
  } Name;                                                                      \
                                                                               \
  static inline bool Name##_is(Name error, Name##Kind kind) {                  \
    return error.kind == kind;                                                 \
  }                                                                            \
                                                                               \
  static inline const char *Name##_kind_name(Name error) {                     \
    switch (error.kind) {                                                      \
      Errors(Name, GC_ERROR_KIND_CASE) default : return "UNKNOWN";             \
    }                                                                          \
  }                                                                            \
                                                                               \
  static inline int Name##_format(char *buffer, size_t capacity, Name error) { \
    switch (error.kind) {                                                      \
      Errors(Name, GC_ERROR_FORMAT_CASE) default : return -1;                  \
    }                                                                          \
  }                                                                            \
                                                                               \
  static inline int Name##_print(FILE *stream, Name error) {                   \
    switch (error.kind) {                                                      \
      Errors(Name, GC_ERROR_PRINT_CASE) default : return -1;                   \
    }                                                                          \
  }

#define GC_ERROR(Name, Variant, ...)                                           \
  ((Name){.kind = Name##_##Variant, .as.Variant = __VA_ARGS__})

#define GC_ERROR_NO_DATA(Name, Variant)                                        \
  ((Name){.kind = Name##_##Variant, .as.Variant = {0}})

#endif
