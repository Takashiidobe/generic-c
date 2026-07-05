# generic-c build & test runner
#
#   just            # build + run every main.c (clang, strict, -Werror)
#   just test-gcc   # same, with gcc
#   just asan       # build + run under AddressSanitizer + UBSan
#   just lint       # aggressive clang warnings (no -Werror)
#   just fmt        # format every C source and header
#   just clean

cc     := env_var_or_default("CC", "clang")
std    := "-std=gnu2x"

# strict warning set — kept -Werror-clean on both clang and gcc.
# this codebase leans on GNU extensions (statement expressions, flexible
# array members, typeof), so -Wpedantic/-Weverything live in `lint` instead.
warn   := "-Wall -Wextra -Werror -Wundef -Wvla -Wpointer-arith " + \
          "-Wstrict-prototypes -Wmissing-prototypes -Wwrite-strings " + \
          "-Wredundant-decls -Wdate-time"

# every runnable translation unit
mains  := "std/array/main.c std/btreemap/main.c std/btreeset/main.c " + \
          "std/alignment/main.c std/cli/main.c std/error/main.c " + \
          "std/hashmap/main.c " + \
          "std/hashset/main.c " + \
          "std/linked-list/main.c " + \
          "std/option/main.c std/result/main.c std/tuple/main.c " + \
          "std/vecdeque/main.c questions/01-two-sum/main.c"

out    := justfile_directory() / "build"

default: test

# compile every main.c without running
build extra="":
    @mkdir -p {{out}}
    @for src in {{mains}}; do \
        name=$(echo "$src" | tr '/' '-' | sed 's/\.c$//'); \
        echo "  CC   $src"; \
        {{cc}} {{std}} {{warn}} {{extra}} "$src" -o "{{out}}/$name" || exit 1; \
    done

# compile + run every main.c
test extra="": (build extra)
    @echo "--- running ---"
    @for src in {{mains}}; do \
        name=$(echo "$src" | tr '/' '-' | sed 's/\.c$//'); \
        echo "  RUN  $src"; \
        "{{out}}/$name" || exit 1; \
    done
    @echo "all passed."

# same, forcing gcc
test-gcc:
    @CC=gcc just test

# build + run under AddressSanitizer + UndefinedBehaviorSanitizer
asan:
    @just test "-fsanitize=address,undefined -fno-sanitize-recover=all -g -O1"

# aggressive clang-only warnings, non-fatal (surfaces -Wconversion, reserved
# identifiers, etc. without blocking the build)
lint:
    #!/usr/bin/env bash
    set -uo pipefail
    # suppress GNU-extension and pre-C23/C11 compat noise — this lib targets
    # gnu2x on purpose, so those diagnostics are not actionable.
    supp="-Wno-declaration-after-statement -Wno-gnu-statement-expression \
          -Wno-gnu-statement-expression-from-macro-expansion \
          -Wno-gnu-zero-variadic-macro-arguments \
          -Wno-gnu-flexible-array-initializer -Wno-gnu-alignof-expression \
          -Wno-padded -Wno-unsafe-buffer-usage -Wno-covered-switch-default \
          -Wno-pre-c23-compat -Wno-pre-c11-compat -Wno-pre-c2x-compat \
          -Wno-c++98-compat -Wno-implicit-void-ptr-cast \
          -Wno-gnu-auto-type"
    for src in {{mains}}; do
        echo "=== $src ==="
        clang {{std}} -Weverything $supp -fsyntax-only "$src"
    done

# format every C source and header
fmt:
    #!/usr/bin/env bash
    set -euo pipefail
    find std questions -type f \( -name '*.c' -o -name '*.h' \) -print0 \
      | sort -z \
      | xargs -0 clang-format -i

clean:
    rm -rf {{out}}
