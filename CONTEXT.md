# 测点生成（Testcase Generation）

面向信息学奥赛（OI）出题的测点生成能力：给定题面 markdown 与标程 `std.cpp`，产出满足数据范围与特殊性质、且具备区分度的标准数据包。

## Language

**测点（Test Point / Testcase）**:
一个 `x.in` 与 `x.out` 文件对，是最终交付的数据单元。
_Avoid_: 测试数据、测试样例

**题面（Statement）**:
题目的 markdown 文本，含题意、输入/输出格式、数据范围、特殊性质与测试点表。

**数据范围（Constraints）**:
题面声明的全局变量边界，如 `1 ≤ n ≤ 5×10⁴`。
_Avoid_: 数据限制

**测试点表（Test-point Table）**:
题面中按测试点编号列出各点上限与特殊性质的表格。
_Avoid_: 数据范围表、子任务表

**特殊性质（Special Property）**:
对某组测试点输入的额外约束，如「`L_j = R_j`」，用于约束该组测点的生成。
_Avoid_: 性质、特殊限制

**标程（Reference Solution / std.cpp）**:
正确解，是标准答案 `out` 的唯一来源；其复杂度决定「大数据」的规模。
_Avoid_: 正解、AC 代码

**错解（Wrong Solution）**:
一切不应 AC 的解法——含假算法（贪心冒充 DP 等）与暴力。统一放 `wrong/`，须被至少一个测点击败。
_Avoid_: 假算法、伪 AC

**暴力（Brute Force / bf.cpp）**:
正确但低效的解法，是错解的一种，靠大数据使其 TLE 被击败（而非 WA）。
_Avoid_: 朴素解、naive

**生成器（Generator）**:
产出 `.in` 的程序。

**校验器（Validator）**:
校验 `.in` 满足数据范围与特殊性质的程序。

**判题器（Checker）**:
判定选手输出相对标准答案对错的程序；入参固定 `<in> <out> <ans>`，基于 testlib，产物为 AC/WA。
_Avoid_: SPJ checker、special judge（那是其子类）

**特殊评测（SPJ）**:
判题器的一类：验证选手输出本身合法（多解任一合法方案、浮点误差内一致），而非与参考答案逐字相同。
_Avoid_: special judge、多解题判题

**错解自检（Discrimination Check）**:
逐个运行 `wrong/` 里的错解，确认每个都被至少一个测点击败（WA/TLE/RE），未击败则加固数据。

**数据强度（Discrimination）**:
数据能区分正确解与错解/暴力的能力。
_Avoid_: 区分度、卡常强度
