#!/bin/bash

set -e

[ -z "${SRC_DIR}" ] && export SRC_DIR="`pwd`"
[ -z "${BUILD_DIR}" ] && export BUILD_DIR=/build
mkdir -p "${BUILD_DIR}"

BAZEL_OPTIONS="--jobs 1"
BAZEL_TEST_OPTIONS="$BAZEL_OPTIONS --test_output=errors"


if [[ "$1" == "clang_tidy" ]]; then
  export CC=/usr/bin/clang-6.0
  CC=/usr/bin/clang-6.0 bazel build \
        $BAZEL_OPTIONS \
        //src/... //test/... //benchmark/... //include/...
  ./ci/gen_compilation_database.sh
  ./ci/fix_compilation_database.py
  ./ci/run_clang_tidy.sh |& tee /clang-tidy-result.txt
  grep ": error:" /clang-tidy-result.txt | cat > /clang-tidy-errors.txt
  num_errors=`wc -l /clang-tidy-errors.txt | awk '{print $1}'`
  if [[ $num_errors -ne 0 ]]; then
    exit 1
  fi
  exit 0
elif [[ "$1" == "bazel.asan" ]]; then
  bazel build -c dbg \
        $BAZEL_OPTIONS \
        --copt=-fsanitize=address \
        --linkopt=-fsanitize=address \
        //...
  bazel test -c dbg \
        $BAZEL_TEST_OPTIONS \
        --copt=-fsanitize=address \
        --linkopt=-fsanitize=address \
        //...
  exit 0
elif [[ "$1" == "bazel.coverage" ]]; then
  mkdir -p /coverage
  rm -rf /coverage/*
  bazel coverage \
    $BAZEL_OPTIONS \
    --instrument_test_targets \
    --experimental_cc_coverage \
    --combined_report=lcov \
    --instrumentation_filter="-3rd_party,-benchmark,-test" \
    --coverage_report_generator=@bazel_tools//tools/test/CoverageOutputGenerator/java/com/google/devtools/coverageoutputgenerator:Main \
    //...
  genhtml bazel-out/_coverage/_coverage_report.dat \
          --output-directory /coverage
  tar czf /coverage.tgz /coverage
  exit 0
fi
