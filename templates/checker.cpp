// 精确匹配 checker —— 逐 token 比较，一般无需改动
//
// 用法：  ./checker  <输入> <标准答案 out> <选手输出>
//        argv[1]=输入  argv[2]=标准答案(ans)  argv[3]=选手输出(ouf)
//        退出码 0 = AC（一致），非 0 = WA。
//
// 注意：这是「唯一解、精确匹配」型 checker（v1 范围）。
//       「输出任意一种合法方案」的 SPJ 型题目不在 v1 内，届时需另写 checker。

#include "testlib.h"

using namespace std;

int main(int argc, char* argv[]) {
    setName("compare sequences of tokens");
    registerTestlibCmd(argc, argv);

    int n = 0;
    string j, p;

    while (!ans.seekEof() && !ouf.seekEof()) {
        n++;
        ans.readWordTo(j);
        ouf.readWordTo(p);

        if (j != p)
            quitf(_wa, "%d%s words differ - expected: '%s', found: '%s'",
                  n, englishEnding(n).c_str(),
                  compress(j).c_str(), compress(p).c_str());
    }

    if (ans.seekEof() && ouf.seekEof()) {
        if (n == 1)
            quitf(_ok, "\"%s\"", compress(j).c_str());
        else
            quitf(_ok, "%d tokens", n);
    } else {
        if (ans.seekEof())
            quitf(_wa, "Participant output contains extra tokens");
        else
            quitf(_wa, "Unexpected EOF in the participants output");
    }
}
