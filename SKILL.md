---
name: testcase-gen
description: 给定题目 markdown 题面与 C++ 标程 std.cpp，生成信息学奥赛（OI）测点——覆盖题面数据范围与特殊性质，并对 wrong/ 下的错解自检与加固。当用户要求「出数据 / 造测点 / 生成测试数据 / 造数据包」时使用。
---

# 测点生成（testcase-gen）

面向 OI 出题：根据题面 `problem.md` 与标程 `std.cpp`，产出 Luogu 风格数据包（`data/1.in … data/N.out`），并保证数据强度——覆盖数据范围、特殊性质、边界坑点，且能击败 `wrong/` 下的所有错解（含暴力）。

## 何时用 / 不适用

- 适用：标准输入/输出题，唯一解、精确匹配答案。
- 不适用（v1 外）：交互题、SPJ（「输出任意一种合法方案」）、提交答案题。遇到这类题先告知用户暂不支持。

## 目录约定

**题目工作目录**（你运行时所在的目录）：
```
problem.md          题面
std.cpp             标程（正确解，标准答案 out 的唯一来源）
wrong/              可选：错解目录，所有 .cpp 都要被数据击败（暴力放 bf.cpp）
data/               测点：1.in…N.out
README.md           测点说明（产物，见流程 11，位于题目根目录）
verify-report.md    错解自检报告（产物，由 verify.sh 生成，位于题目根目录）
work/               中间：gen（二选一：gen.cpp 或 gen.py）/validator/checker 源码 + testlib.h + bin/
```

**skill 本体**（本目录）：
```
vendor/testlib.h            testlib 单头文件（已 vendor，勿改）
templates/{gen.cpp,gen.py,validator.cpp,checker.cpp}   模板（生成器二选一）
scripts/{build.sh,verify.sh}            编译 / 生成+自检（双轨分流）
references/                 规范（按需查阅）
  property-to-code.md       特殊性质 → 生成代码 翻译规范
  pitfall-checklist.md      坑点清单（题面未指明测试点时驱动设点）
  anti-brute-force.md       大测点 / 卡暴力策略
  cyaron-cheatsheet.md      cyaron 速查（py轨可选，仅图/树题）
CONTEXT.md、docs/adr/       词汇表与决策记录
```

## 前置检查

1. `g++` 可用；`vendor/testlib.h` 存在。
2. 题目工作目录里 `problem.md` 与 `std.cpp` 齐全；`wrong/` 可选。
3. py轨（选用 `gen.py` 时）：`python3` 可用且可 `import cyaron`（`pip install cyaron`，建议锁版本并在题目 `README.md` 记录）。

## 流程（严格按序执行）

### 1. 读题面，抽取信息

从 `problem.md` 里提取，并在脑中/笔记里明确：
- **输入格式**、**输出格式**（逐项、逐行，含变量名与顺序）。
- **全局数据范围**：题面「对于所有测试数据均有」的部分（如 `1 ≤ n ≤ 5×10⁴`、`|a_i| ≤ 10⁵`）。
- **测试点表 / 逐点规范**（若有）：表格每行「测试点编号 → 各变量上限 → 特殊性质」，或正文逐档叙述。注意 `^` 表示与上行相同，`$x \sim y$` 表示编号区间。**若题面只给数据范围、未指明测试点情况，标注「默认 20 点方案」，供第 3 步使用。**
- **特殊性质定义**：题面末尾对 A/B/C… 的逐条自然语言描述。

### 2. 读标程，定复杂度与大数据规模

读 `std.cpp`，确定算法与复杂度（`O(n)` / `O(n log n)` / `O(n√n)` / `O(n²)`…）。这决定「大数据」取多大、以及什么暴力该被 TLE 卡掉。见 `references/anti-brute-force.md`。

### 3. 定测点方案

- **题面已指明测试点规范**（详细给出测试点数量、范围与各点/各档的具体性质，无论表格还是逐段文字）：严格照题面映射，不自行增减。每个编号（或编号组）→ 上限 + 特殊性质，逐点列出；带性质的满 `n` 点（如例中 `6,7` 性质 A）照性质生成，别丢性质。
- **题面只有数据范围、未指明测试点情况**：**默认出 20 个测点**，按 `references/pitfall-checklist.md` 覆盖四档（数目按题在 20 内灵活分配，不求均分）——
  - **题目样例**：题面每个「输入样例」原样收录，通常排在最前（1–2 个）；`.out` 由 std 跑出并须与题面样例输出一致。
  - **题目测试数据**：中档常规随机若干，覆盖一般情形。
  - **小测试点**：边界 / 易错分支 / 退化结构（`n=1`、端点、全同·递增·递减、溢出、贪心冒充 DP 等）。
  - **大测试点**：满 `n`、最坏复杂度构造（卡暴力 TLE）。
- 每个测点记录：编号 / 上限 / 性质（无表时标档位，如「样例」「测试数据/常规」「小/边界」「大/满 n」）/ 针对的坑 / 预期复杂度。这份记录就是后面题目根目录下 `README.md` 的底稿。
- **选轨声明**：图/树等结构化强的题可选 py轨（cyaron `gen.py`），其余默认 cpp轨（testlib `gen.cpp`）；满 `n` 大数据优先 cpp轨。方案里写明“本题目用 x 轨，因为……”。

### 4. 准备 work/

- 复制 `vendor/testlib.h` → `work/testlib.h`。
- 复制 `templates/validator.cpp`、`templates/checker.cpp` → `work/`。
- 生成器二选一：cpp轨复制 `templates/gen.cpp` → `work/`；py轨复制 `templates/gen.py` → `work/`（`work/gen.py` 存在即分流，两者同时存在时优先 py轨）。

### 5. 写生成器（cpp轨 gen.cpp / py轨 gen.py 二选一）

cpp轨按 `templates/gen.cpp` 的结构填写（`switch (tid)` 逐点设参，`rnd.*` 生成）；py轨按 `templates/gen.py` 填写（`if tid == ...` 逐点设参，`Graph.*` 出骨架，见 `references/cyaron-cheatsheet.md`）。**py轨硬约束**：只准 `print` 到 stdout 写 `.in`，禁 `IO.output_gen`（build 会拦截）；`random.seed(tid)` 保证可复现。

以下两条对两轨相同：
- 逐点/逐组设置参数（上限、`mode` 性质开关）：cpp轨在 `switch (tid)` 里，py轨在 `if tid == ...` 里。
- 生成逻辑按性质翻译——**严格照 `references/property-to-code.md`**：先构造满足性质，再叠加随机扰动；性质组内也要覆盖该性质的边界。
- 生成后，务必让每个 `.in` 都落在全局数据范围内。

### 6. 写 validator.cpp

按 `templates/validator.cpp`，把「全局数据范围」写进去：每个 `inf.readInt(min, max, ...)` 的边界严格取题面值。特殊性质由生成器保证（可选在 validator 内按测点加 `ensuref` 兜底）。

### 7. 编译

```bash
bash <skill>/scripts/build.sh
```
分流编译：cpp轨编译 gen/validator/checker/std 及 `wrong/*.cpp`；py轨校验 `python3` + `cyaron` 可用、`gen.py` 语法通过且无 `output_gen`，再编译 validator/checker/std 及 `wrong/*.cpp`。若某个错解编译失败，看报错——可能是它自己写错，报告给用户，但不要因此中断（其余照常）。

### 8. 生成 + 错解自检

```bash
bash <skill>/scripts/verify.sh <N> [TL_ms]
```
- `N` = 测点总数。
- `TL_ms` = 单点时限（毫秒），取题面声明时限，无则默认 2000。它决定错解是否被判 TLE。

这一步产出 `data/1.in…N.out` 和题目根目录的 `verify-report.md`，并在 stdout 打印每个错解的 AC/WA/TLE/RE 与「是否被击败」。

### 9. 加固（循环，直到达标）

读题目根目录的 `verify-report.md`：
- 有错解**未被击败**（全部 AC）→ 数据太弱，必须加固：
  - 错解是**假算法**（应被 WA）：读该错解代码，定位它错在哪一步（贪心/特判/漏情况），按 `references/pitfall-checklist.md` 构造能戳穿它的数据，新增或替换测点。
  - 错解是**暴力**（应被 TLE）：按 `references/anti-brute-force.md` 加大规模或加强对抗性构造。
  - 改完回到第 5 步重写生成器（`gen.cpp` 或 `gen.py`，或调参），再走 7→8。
- **全部击败** → 进入第 10 步。
- 若 `wrong/` 为空：跳过错解自检，但仍要出大数据点防暴力（按题面最大 `n`）。

### 10. 抽查性质

对每个「性质组」的 `.in` 人工抽查：确认输入**确实满足**该性质、且落在范围与格式内。这是最容易翻车的一步，务必逐组核对。

### 11. 写 README.md

在题目根目录的 `README.md` 汇总（底稿来自第 3 步的记录）：
- 每个测点：编号 / 变量上限 / 特殊性质 / 针对的坑 / 预期复杂度。
- 所用生成轨（cpp/testlib 或 py/cyaron + cyaron 版本号）。
- 错解自检摘要（来自 `verify-report.md`）：每个错解被哪些点、以何种方式（WA/TLE/RE）击败。

### 12. 交付

向用户报告：
- 产物清单：`data/1.in…N.out`、题目根目录的 `README.md`、`verify-report.md`。
- 自检结果：每个错解是否被击败、以何种方式。
- 生成中发现的题面/标程疑点（若有）。

## 关键规范（务必遵循）

- **特殊性质是硬约束**：带性质的测点，输入必须满足该性质；用 `references/property-to-code.md` 的「先构造后扰动」避免退化。
- **大数据防暴力**：满 `n` 点必出，且构造最坏输入而非纯随机，见 `references/anti-brute-force.md`。
- **坑点覆盖**：边界、溢出（`long long`）、负值/零、退化结构、贪心冒充 DP，见 `references/pitfall-checklist.md`。
- **判定口径**：错解只要在任一测点非 AC 即「被击败」；暴力被 TLE 击败、假算法被 WA 击败，两者都要有（见 `docs/adr/0001`）。
- **可复现**：testlib `registerGen` 保证同一 seed 同一输出；py轨用 `random.seed(tid)` 对等保证。不要引入无 seed 的随机源。
- **py轨禁 output_gen**：cyaron 只准写 `.in` 到 stdout；`.out` 仍由 `verify.sh` 统一调 `std` 产出（见 `docs/adr/0003`）。
