# testcase-gen

面向**信息学奥赛（OI）出题**的 Claude Code skill：给定题面 markdown 与标程 `std.cpp`，自动生成一套覆盖数据范围、特殊性质与边界坑点、且能击败错解的**标准数据包**。

## 它做什么

输入三个素材，产出一套可提交的数据：

| 输入 | 说明 |
| --- | --- |
| `problem.md` | 题面（含数据范围、测试点表、特殊性质） |
| `std.cpp` | 标程（正确解，标准答案的唯一来源） |
| `wrong/`（可选） | 错解目录，里面所有 `.cpp` 都要被数据击败 |

| 输出 | 说明 |
| --- | --- |
| `data/1.in … data/N.out` | Luogu 风格数据包 |
| `data/README.md` | 每个测点的编号/上限/性质/针对的坑 |
| `data/verify-report.md` | 错解自检报告（AC/WA/TLE/RE） |

核心能力：

- **覆盖测试点表**：题面有「测试点编号 → 上限 → 特殊性质」表时，严格按表逐点映射（含 `^`、`$x \sim y$` 等记号）。
- **特殊性质翻译**：把「`L_j = R_j`」「`R_j ≤ 32`」等自然语言性质转成生成代码，并保证带性质的测点严格满足。
- **无表时自主设点**：按坑点清单（边界/溢出/退化结构/贪心冒充 DP）设约 10–20 个点。
- **大数据卡暴力**：满 `n` 测点构造最坏输入（长区间/逆序/全等），让朴素暴力 TLE 而非侥幸通过。
- **错解自检 + 加固**：逐个运行 `wrong/` 下的错解，未击败的自动加固数据直到击败。

## 安装

把本目录软链到 Claude Code 的全局 skill 目录：

```bash
ln -s "$HOME/Repository/testcase-gen" "$HOME/.claude/skills/testcase-gen"
```

安装后即可用 `/testcase-gen` 触发，或直接说「帮我出数据 / 造测点 / 生成测试数据」。

> 需要 g++（本机已确认 g++ 14.x）。testlib 已作为单头文件 vendor 在 `vendor/testlib.h`，无需额外安装。

## 快速开始

```text
<某道题>/
├── problem.md       题面
├── std.cpp          标程
└── wrong/           可选：bf.cpp、贪心错解等（都会被自检并加固）

# 在题目目录下触发 skill，按指引跑完流程后得到：
├── data/            1.in … N.out + README.md + verify-report.md
└── work/            gen/validator/checker 源码、testlib.h、bin/
```

## 工作原理

skill 按 `SKILL.md` 里的流程执行：

1. **读题面** → 抽取输入/输出格式、全局数据范围、测试点表、特殊性质。
2. **读标程** → 定算法复杂度，决定大数据规模与卡暴力的目标。
3. **定方案** → 有表按表、无表按坑点清单，逐点定上限/性质/针对的坑。
4. **写生成器与校验器** → 填 `templates/` 模板；性质按 `references/property-to-code.md`「先构造、后扰动」。
5. **编译 + 生成 + 自检** → `scripts/build.sh` 编译，`scripts/verify.sh` 生成数据、校验、标程出答案、跑错解自检。
6. **加固循环** → 有错解未被击败就针对它加数据，直到全部击败。
7. **交付** → 产 `data/` 数据包 + `README.md` + 自检报告。

## 目录结构

```
testcase-gen/
├── SKILL.md                        技能正文（流程与规范索引）
├── CONTEXT.md                      领域词汇表
├── docs/adr/                       决策记录
│   ├── 0001-…-verification.md      错解自检（错解必须被击败）
│   └── 0002-hybrid-architecture.md 混合架构
├── vendor/testlib.h                testlib 单头文件（勿改）
├── templates/
│   ├── gen.cpp                     生成器模板（registerGen + switch）
│   ├── validator.cpp               校验器模板（带上下界的 readInt）
│   └── checker.cpp                 精确匹配 checker（逐 token）
├── scripts/
│   ├── build.sh                    编译 gen/validator/checker/std/错解
│   └── verify.sh                   生成 + 校验 + 出答案 + 错解自检 + 报告
└── references/                     规范（skill 运行时按需查阅）
    ├── property-to-code.md         特殊性质 → 生成代码 翻译规范
    ├── pitfall-checklist.md        坑点清单
    └── anti-brute-force.md         大测点 / 卡暴力策略
```

## 术语

见 [`CONTEXT.md`](./CONTEXT.md)。关键区分：

- **标程 `std.cpp`**：正确解，产标准答案。
- **错解 `wrong/`**：一切不该 AC 的解法，统一放这里——假算法（贪心冒充 DP）与暴力（`bf.cpp`）都在内。
- **两种击败方式**：暴力被 **TLE** 击败（数据够大）、假算法被 **WA** 击败（数据戳中它的错）。二者都要有。

## 当前边界（v1）

- 仅标准输入/输出题、唯一解精确匹配。
- 暂不支持：交互题、SPJ（「输出任意一种合法方案」）、提交答案题。

## License

依赖 [testlib](https://github.com/MikeMirzayanov/testlib)（MIT License）。
