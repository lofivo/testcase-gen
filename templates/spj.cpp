// SPJ（特殊评测）判题器骨架 —— 复制本文件为 work/checker.cpp 后，按题面删改。
// 用法（testlib 三参，与 verify.sh 调用约定一致）：
//   ./checker <输入> <标准答案 out> <选手输出>
//
// 选写法前先读 references/spj-guide.md。三段常见写法：
//   [A] 多解/任意合法方案：忽略与 out 的逐字一致，独立验证 ans 合法
//   [B] 浮点误差：容差严格取自题面声明；题面未写容差则本题不是浮点题
//   [C] 输出结构验证：格式/范围/自洽性检查，不依赖 out
//
// 强制两步自检（防过松/过紧）：
//   1) std 产出的 .out 跑本 checker → 必须 AC
//   2) 构造非法输出（含「格式合法但语义错」）→ 必须 WA
// out 仅作参考基线：SPJ 题 checker 不要求 ans 与 out 逐字一致。

#include "testlib.h"
#include <cmath>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
    registerTestlibCmd(argc, argv);
    // testlib 三参映射：<in> → inf   <out>(标准答案,参考) → ans   <选手输出> → ouf
    // 故：ans 是 std 的 .out（仅参考基线）；ouf 是选手输出。

    int n = inf.readInt();  // 按题面读输入

    // ===== [A] 多解/任意合法方案 =====
    // 思路：从 ouf 读候选方案 → 逐条验证合法 → 检查是否达成目标。
    // int k = ouf.readInt(1, n, "方案规模");
    // for (int i = 1; i <= k; i++) {
    //     int x = ouf.readInt(1, n, "元素");
    //     // TODO: 按题面验证合法性，不合法 → quitf(_wa, "第 %d 个元素非法", i);
    // }
    // // TODO: 验证目标（最优性/约束满足）；若只需合法不要求最优，到此即可 accept。

    // ===== [B] 浮点误差 =====
    // 容差仅在题面声明时启用；相对误差写法示例：
    // double expected = ans.readDouble();  // 或由 inf 独立算出 expected
    // double got = ouf.readDouble();
    // double eps = 1e-6;  // ← 必须与题面声明一致，勿擅自放宽
    // if (fabs(got - expected) > eps * max(1.0, fabs(expected)))
    //     quitf(_wa, "误差过大: expected=%g got=%g", expected, got);

    // ===== [C] 输出结构验证 =====
    // int m = ouf.readInt(1, n, "输出规模");
    // for (int i = 1; i <= m; i++) ouf.readInt(1, n, "值");
    // ouf.readEof();

    // 未实现时先失败，避免空 checker 误判全 AC：
    quitf(_fail, "SPJ checker 未实现：请按 spj-guide.md 完成题面判定逻辑");
}
