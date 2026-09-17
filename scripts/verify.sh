#!/usr/bin/env bash
# ============ 测点生成 + 错解自检 ============
# 在题目工作目录运行（cwd = 题目目录）。前置：已运行 build.sh。
# 用法：  verify.sh <测试点数量 N> [时间上限_ms，默认 2000]
# 产出：  data/1.in..N.in、data/1.out..N.out、verify-report.md（题目根目录）
#
# 判定口径（见 docs/adr/0001）：
#   - 一个错解只要在任一测点非 AC（WA/TLE/RE）即「被击败」。
#   - 全部 AC 则「未被击败」，需加固数据。

set -u
shopt -s nullglob

N=${1:?usage: verify.sh <N> [TL_ms]}
TL=${2:-2000}

BIN=work/bin
DATA=data
WRONG="$BIN/wrong"

mkdir -p "$DATA"

echo "== [1/2] 生成数据 + 校验 + 标程出答案 =="
# 双轨分流（见 ADR-0003）：work/gen.py 存在即 py轨，否则走 cpp轨 work/bin/gen。
if [ -f work/gen.py ]; then
  GEN_CMD="python3 work/gen.py"
  echo "    py轨（cyaron）：python3 work/gen.py <tid>"
else
  GEN_CMD="$BIN/gen"
fi
for ((i=1; i<=N; i++)); do
  $GEN_CMD "$i" > "$DATA/$i.in"
  if ! "$BIN/validator" < "$DATA/$i.in"; then
    echo "!! 校验失败：测试点 $i 的输入不合法" >&2
    exit 1
  fi
  "$BIN/std" < "$DATA/$i.in" > "$DATA/$i.out"
done
echo "    完成：$N 个测点。"

echo "== [2/2] 错解自检 =="
if [ ! -d "$WRONG" ] || [ -z "$(ls -A "$WRONG" 2>/dev/null)" ]; then
  echo "    无错解（wrong/ 为空或不存在），跳过错解自检。"
  {
    echo "# 错解自检报告"
    echo
    echo "（无错解，跳过错解自检。）"
  } > "verify-report.md"
  exit 0
fi

# 时间上限换算为 timeout 的秒数（向上取整）
SEC=$(( (TL + 999) / 1000 ))
[ "$SEC" -lt 1 ] && SEC=1

REPORT="verify-report.md"
{
  echo "# 错解自检报告"
  echo
  echo "时间上限：${TL}ms/测点；共 $N 个测点。"
  echo
  echo "| 错解 | AC | WA | TLE | RE | 是否被击败 |"
  echo "| --- | --- | --- | --- | --- | --- |"
} > "$REPORT"

all_defeated=1
for w in "$WRONG"/*; do
  [ -x "$w" ] || continue
  name=$(basename "$w")
  ac=0; wa=0; tle=0; re=0
  for ((i=1; i<=N; i++)); do
    tmp=$(mktemp)
    timeout "$SEC" "$w" < "$DATA/$i.in" > "$tmp" 2>/dev/null
    code=$?
    if [ "$code" -eq 124 ] || [ "$code" -eq 137 ]; then
      tle=$((tle+1))
    elif [ "$code" -ne 0 ]; then
      re=$((re+1))
    elif "$BIN/checker" "$DATA/$i.in" "$DATA/$i.out" "$tmp" >/dev/null 2>&1; then
      ac=$((ac+1))
    else
      wa=$((wa+1))
    fi
    rm -f "$tmp"
  done

  if [ "$ac" -eq "$N" ]; then
    defeated="否 ❌"
    all_defeated=0
  else
    defeated="是 ✅"
  fi
  echo "| $name | $ac | $wa | $tle | $re | $defeated |" >> "$REPORT"
  echo "    $name: AC=$ac WA=$wa TLE=$tle RE=$re -> $defeated"
done

echo
if [ "$all_defeated" -eq 1 ]; then
  echo "== ✅ 全部错解已被击败，数据强度达标 =="
else
  echo "== ❌ 存在未击败的错解，需加固数据（见上表） =="
fi
