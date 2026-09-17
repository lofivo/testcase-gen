#!/usr/bin/env bash
# ============ 编译所有组件 ============
# 在题目工作目录运行（cwd = 题目目录）。
# 前置：work/ 下已有 validator.cpp / checker.cpp / testlib.h（二轨共有），
#       生成器二选一：work/gen.cpp（cpp轨 testlib）或 work/gen.py（py轨 cyaron），
#       根目录有 std.cpp，可选 wrong/*.cpp。
# 产物：cpp轨 work/bin/{gen, validator, checker, std}；py轨无 work/bin/gen（运行时 python3 work/gen.py）。

set -euo pipefail
shopt -s nullglob

CXX=${CXX:-g++}
FLAGS="-std=c++23 -O2 -I work"

mkdir -p work/bin work/bin/wrong

echo "== 生成器选轨 =="
if [ -f work/gen.py ]; then
  echo "    py轨（cyaron）：检测到 work/gen.py"
  if [ -f work/gen.cpp ]; then
    echo "    !! work/gen.cpp 与 work/gen.py 同时存在，优先使用 py轨（忽略 gen.cpp）" >&2
  fi
  if ! command -v python3 >/dev/null 2>&1; then
    echo "!! py轨需要 python3，未找到。请安装 python3 后重试。" >&2
    exit 1
  fi
  if ! python3 -c "import cyaron" 2>/dev/null; then
    echo "!! py轨需要 cyaron（pip install cyaron），当前 python3 下不可 import。" >&2
    python3 -c "import cyaron" 2>&1 | head -5 >&2 || true
    exit 1
  fi
  python3 -m py_compile work/gen.py
  echo "    cyaron 版本：$(python3 -c "from importlib.metadata import version; print(version('cyaron'))" 2>/dev/null || echo unknown)"
  echo "    gen.py 语法检查通过（可复现要求：模板内 random.seed(tid），禁 output_gen）。"
  if grep -v "^[[:space:]]*#" work/gen.py | grep -q "output_gen"; then
    echo "!! work/gen.py 含 output_gen 调用，违反 ADR-0003（.out 必须由 verify.sh 统一调 std 产出）。请删除后再编译。" >&2
    exit 1
  fi
else
  echo "    cpp轨（testlib）：使用 work/gen.cpp"
  if [ ! -f work/gen.cpp ]; then
    echo "!! work/gen.py 与 work/gen.cpp 都不存在，无法确定生成器。请按模板提供其一。" >&2
    exit 1
  fi
  echo "== 编译 gen =="
  $CXX $FLAGS work/gen.cpp       -o work/bin/gen
fi

echo "== 编译 validator / checker =="
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
