// 校验器模板 —— 按题面改写后使用
//
// 用法：  ./validator  <  x.in     （校验失败以非零退出）
// 职责：  校验 .in 满足题面的【全局数据范围】（即「对于所有测试数据均有」的部分）。
//         特殊性质由生成器保证（生成时即满足）；若想在此兜底，可按测点加 ensuref。
//
// 读取 API（失败时自动报错并 exit，无需手动判断）：
//   inf.readInt(min, max, "名称")      整数，含边界
//   inf.readLong / readDouble / readToken / readWord / readLine
//   inf.readSpace() / readEoln() / readEof()
//   ensuref(cond, "格式串", ...)       显式断言（校验特殊性质用）
//
// 关键原则：每读一个数都带上下界，范围严格取自题面「数据范围」。

#include "testlib.h"
using namespace std;

int main(int argc, char* argv[]) {
    registerValidation(argc, argv);

    // ============ 按题面输入格式逐行读取 + 校验（以下为例） ============
    int n = inf.readInt(1, 50000, "n");   // 上限取自题面
    inf.readSpace();
    int q = inf.readInt(1, 1024, "q");
    inf.readEoln();

    for (int i = 1; i <= n; ++i) {
        int a = inf.readInt(-100000, 100000, "a_i");
        if (i < n) inf.readSpace(); else inf.readEoln();
    }

    for (int j = 1; j <= q; ++j) {
        int L = inf.readInt(1, n, "L_j");
        inf.readSpace();
        int R = inf.readInt(L, n, "R_j");
        inf.readEoln();

        // 若需要按测点校验特殊性质（可选），用 argv 或文件名得知测点编号后：
        // ensuref(L == R, "性质 A：L_j 必须等于 R_j");
    }

    inf.readEof();
    return 0;
}
