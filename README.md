# testcase-gen

面向**信息学奥赛（OI）**场景的测点生成 agent skill：给定题面 markdown 与标程 `std.cpp`，自动生成一套覆盖数据范围、特殊性质与边界坑点、且能击败错解的**测点**。

> 本 skill 遵循 **SKILL.md 开放格式**：一个目录 + 一个 `SKILL.md`（frontmatter `name` / `description`）+ 支撑文件。核心资产（模板、脚本、规范、vendored testlib）与具体 agent 无关，**一处开发、多端安装**。

## 它做什么

输入三个素材，产出一套可提交的数据：

| 输入 | 说明 |
| --- | --- |
| `problem.md` | 题面（含数据范围、测试点表、特殊性质） |
| `std.cpp` | 标程（正确解，标准答案的唯一来源） |
| `wrong/`（可选） | 错解目录，里面所有 `.cpp` 都要被数据击败 |

| 输出 | 说明 |
| --- | --- |
| `data/1.in … data/N.out` | 测点 |
| `README.md` | 每个测点的编号/上限/性质/针对的坑 |
| `verify-report.md` | 错解自检报告（AC/WA/TLE/RE） |

核心能力：

- **覆盖测试点表**：题面有「测试点编号 → 上限 → 特殊性质」表时，严格按表逐点映射（含 `^`、`$x \sim y$` 等记号）。
- **特殊性质翻译**：把「`L_j = R_j`」「`R_j ≤ 32`」等自然语言性质转成生成代码，并保证带性质的测点严格满足。
- **无表时默认出 20 个测点**：题面只给数据范围时，默认 20 点覆盖四档——题目样例（原样收录）、中档常规测试数据、小边界/易错点、大满 `n` 卡暴力点。
- **大数据卡暴力**：满 `n` 测点构造最坏输入（长区间/逆序/全等），让朴素暴力 TLE 而非侥幸通过。
- **错解自检 + 加固**：逐个运行 `wrong/` 下的错解，未击败的自动加固数据直到击败。

## 安装

### 通用说明

- skill 名为 `testcase-gen`，目录名与其一致，符合各 agent 的命名约束。
- 安装就是**把整个目录放进 agent 的 skills 目录**，支撑文件（`templates/`、`scripts/`、`references/`、`vendor/`）随之就位，无需单独处理。
- **开发调试用软链**（改一处、处处生效），**分发用复制/克隆**（自包含）。
- 需要 g++；testlib 已 vendor 在 `vendor/testlib.h`，无需额外安装。

### Claude Code

```bash
# 全局
mkdir -p "$HOME/.claude/skills"
ln -s "$HOME/Repository/testcase-gen" "$HOME/.claude/skills/testcase-gen"

# 项目级（仅某仓库可用）
ln -s "$HOME/Repository/testcase-gen" "<repo>/.claude/skills/testcase-gen"
```

触发：`/testcase-gen`，或自然语言「帮我出数据 / 造测点 / 生成测试数据」。

### Codex

```bash
# 全局（Codex 递归扫描 ~/.codex/skills/**/SKILL.md）
mkdir -p "$HOME/.codex/skills"
ln -s "$HOME/Repository/testcase-gen" "$HOME/.codex/skills/testcase-gen"
```

触发：由 `name` / `description` 自动匹配，在对话中说明任务即可被加载。Codex 目前以全局 `~/.codex/skills/` 为主（见[官方文档](https://github.com/openai/codex/blob/main/docs/skills.md)）。

### OpenCode

```bash
# 全局（也兼容 ~/.claude/skills/、~/.agents/skills/）
ln -s "$HOME/Repository/testcase-gen" "$HOME/.config/opencode/skills/testcase-gen"

# 项目级（也兼容 .claude/skills/、.agents/skills/）
ln -s "$HOME/Repository/testcase-gen" "<repo>/.opencode/skills/testcase-gen"
```

触发：由 `description` 自动触发。

### 兼容性

三个 agent 都采用 `SKILL.md` + `name` / `description` 这两个必填字段，本 skill 的 frontmatter 三处通用：

- **OpenCode** 额外识别 `license` / `compatibility` / `metadata`（均可选，本 skill 未用，不影响加载）。
- **Codex** 忽略未知 frontmatter 字段。

## 快速开始

```text
<某道题>/
├── problem.md       题面
├── std.cpp          标程
└── wrong/           可选：bf.cpp、贪心错解等（都会被自检并加固）

# 在题目目录下触发 skill，按指引跑完流程后得到：
├── data/            1.in … N.out
├── README.md        测点说明（编号/上限/性质/针对的坑）
├── verify-report.md 错解自检报告
└── work/            gen/validator/checker 源码、testlib.h、bin/
```

## 工作原理

skill 按 `SKILL.md` 里的流程执行：

1. **读题面** → 抽取输入/输出格式、全局数据范围、测试点表、特殊性质。
2. **读标程** → 定算法复杂度，决定大数据规模与卡暴力的目标。
3. **定方案** → 题面指明测试点就照题面逐点映射；只给数据范围则默认 20 点（样例 / 中档常规 / 小边界 / 大满 `n` 四档），逐点定上限/性质/档位/针对的坑。
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
