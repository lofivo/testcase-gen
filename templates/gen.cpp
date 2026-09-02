// 生成器模板 —— 按题面改写后使用
//
// 用法：  ./gen <测试点编号>  >  <编号>.in
// 约定：  单文件生成器，按测试点编号 switch 分支；同性质/同上限的测点共用一个分支。
//         每个分支只设置该测点的参数（上限、特殊性质开关），生成逻辑统一写在下方。
//
// testlib 常用 API（更多见 vendor/testlib.h 顶部注释）：
//   rnd.next(min, max)          整数闭区间随机
//   rnd.next(n)                 等价 rnd.next(0, n-1)
//   rnd.next(1, 1)              生成固定值 1 的惯用写法
//   rnd.wnext(n, t)             带权（偏向小值）随机，t 越大越偏小
//   rnd.perm(n)                 1..n 随机排列
//   rnd.partition(sum, k)       把 sum 随机分成 k 份（保证非空/非负，视版本）
//   rnd.shuffle(vector)         原地打乱
//   rnd.next("[a-z]{1,100}")    按正则生成字符串
//   println(a, b, c)            空格分隔打印并换行；println(vector) 同理
//
// 关键原则：
//   - 用 seed 保证可复现（registerGen 已处理）。
//   - 先按性质"构造"，再叠加随机扰动，避免退化成千篇一律的最简数据。

#include "testlib.h"
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char* argv[]) {
    registerGen(argc, argv, 1);
    int tid = atoi(argv[1]);

    // ============ 一、每个测点的参数（按题面「测试点表」填） ============
    // 下面以「数组 + q 次区间查询」为例（n, q, a[1..n], 查询 (L_j, R_j)）。
    // 换成别的题时，改这里声明的变量即可。
    int n = 0, q = 0;
    int mode = 0;   // 特殊性质开关：0=无性质；1/2/... 对应题面性质 A/B/...

    switch (tid) {
        // —— 按题面测试点表逐行映射。例如：
        // case 1:            n = 1000;  q = 1;    break;   // 最小
        // case 2: case 3:    n = 3000;  q = 50;   break;
        // case 6: case 7:    n = 50000; q = 1024; mode = 1; break;  // 性质 A
        // default:           n = 50000; q = 1024; break;   // 无表时的默认大数据

        // TODO: 在此填写每个测试点（或每组）的参数。
        // 占位，防止未填就编译运行出垃圾数据：
        case 1: n = 1; q = 1; break;
        default: n = 1; q = 1; break;
    }
    // 兜底：若某测点漏填导致 n/q 为 0，用题面全局上限兜底（按需调整）。
    // if (n == 0) n = 50000; if (q == 0) q = 1024;

    // ============ 二、生成数据（按上面的参数） ============
    // TODO: 按题面输入格式，逐行 println。注意特殊性质 mode 的分支。

    // 例：生成 a[1..n]
    vector<int> a(n);
    for (auto& x : a) x = rnd.next(-100000, 100000);
    println(n, q);
    println(a);

    // 例：生成 q 个查询，按 mode 满足特殊性质
    for (int j = 0; j < q; ++j) {
        int L, R;
        if (mode == 1) {            // 性质 A：L_j = R_j
            L = R = rnd.next(1, n);
        } else if (mode == 2) {     // 性质 B：R_j <= 32
            R = rnd.next(1, min(n, 32));
            L = rnd.next(1, R);
        } else if (mode == 3) {     // 性质 C：L_j <= 16 且 R_j >= n-1000
            L = rnd.next(1, 16);
            R = rnd.next(max(L, n - 1000), n);
        } else if (mode == 4) {     // 性质 D：L_j > n/2
            L = rnd.next(n / 2 + 1, n);
            R = rnd.next(L, n);
        } else if (mode == 5) {     // 性质 E：L_j > n/4
            L = rnd.next(n / 4 + 1, n);
            R = rnd.next(L, n);
        } else {                    // 无性质：均匀随机
            L = rnd.next(1, n);
            R = rnd.next(L, n);
        }
        println(L, R);
    }

    return 0;
}
