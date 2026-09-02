#!/usr/bin/env bash
# ============ 编译所有组件 ============
# 在题目工作目录运行（cwd = 题目目录）。
# 前置：work/ 下已有 gen.cpp / validator.cpp / checker.cpp / testlib.h，
#       根目录有 std.cpp，可选 wrong/*.cpp。
# 产物：work/bin/{gen, validator, checker, std} 与 work/bin/wrong/*

set -euo pipefail
shopt -s nullglob

CXX=${CXX:-g++}
FLAGS="-std=c++17 -O2 -I work"

mkdir -p work/bin work/bin/wrong

echo "== 编译 gen / validator / checker =="
$CXX $FLAGS work/gen.cpp       -o work/bin/gen
$CXX $FLAGS work/validator.cpp -o work/bin/validator
$CXX $FLAGS work/checker.cpp   -o work/bin/checker

echo "== 编译标程 std.cpp =="
$CXX $FLAGS std.cpp -o work/bin/std

echo "== 编译错解 wrong/*.cpp =="
wrong_files=(wrong/*.cpp)
if [ ${#wrong_files[@]} -eq 0 ]; then
  echo "    无错解（wrong/*.cpp 不存在），跳过错解编译。"
else
  for f in "${wrong_files[@]}"; do
    name=$(basename "$f" .cpp)
    if $CXX $FLAGS "$f" -o "work/bin/wrong/$name"; then
      echo "    编译成功: $name"
    else
      echo "    !! 编译失败: $f（跳过，请检查该错解能否编译）" >&2
    fi
  done
fi

echo "== 完成 =="
