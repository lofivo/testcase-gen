#!/usr/bin/env python3
# 生成器模板（cyaron 版，可选）——按题面改写后使用
#
# 用法：  python3 gen.py <测试点编号>  >  <编号>.in
# 约定：  与 gen.cpp 同接口（argv[1]=tid，stdout 输出 .in），供 verify.sh 分流调用。
#         何时用本模板：图/树等结构化强的题（链、菊花、随机树、二叉树、DAG、随机图）。
#         数组/区间/字符串题仍用 gen.cpp（C++ testlib 更快）。
#
# 硬约束（见 ADR-0003）：
#   - 只准写 .in（print / input_writeln 到 stdout），禁 IO.output_gen。
#     .out 仍由 verify.sh 统一调 std 产出，错解自检仍走 TL_ms 判定。
#   - 可复现：random.seed(tid) 保证同 tid 同输出；不要引入无 seed 的随机源。
#   - 特殊性质仍是硬约束：先按性质构造骨架，再叠加随机扰动（见 references/property-to-code.md）。
#
# cyaron 常用 API（详见 references/cyaron-cheatsheet.md）：
#   Graph.chain(n) / Graph.flower(n) / Graph.tree(n, chain=0..1, flower=0..1)
#   Graph.binary_tree(n) / Graph.graph(n, m, directed=..., weight_limit=...)
#   Graph.DAG(n, m) / Graph.connected(n, m) / Graph.hack_spfa(...)
#   randint(a, b) / choice / shuffle（Python random，seed 后可复现）
#   str(graph) 按行输出边；带权图输出 u v w。
#
# 前置：pip install cyaron（版本锁定见 skill 前置检查）。

import random
import sys

from cyaron import Graph
from cyaron import randint


def parse_tid() -> int:
    if len(sys.argv) < 2:
        print("usage: gen.py <tid>", file=sys.stderr)
        sys.exit(2)
    return int(sys.argv[1])


def main() -> None:
    tid = parse_tid()
    # 可复现：同 tid 同输出（对标 testlib registerGen）。
    random.seed(tid)

    # ============ 一、每个测点的参数（按题面「测试点表」填） ============
    # 下面以「无向带权图 + 单源最短路查询」为例（n 点 m 边，s 为源点）。
    # 换成别的题时，改这里声明的变量即可。
    n = 0
    m = 0
    mode = 0  # 特殊性质开关：0=无性质；1/2/... 对应题面性质 A/B/...
    directed = False

    # 按题面测试点表逐行映射。例如：
    # if tid == 1: n, m = 10, 15
    # elif tid in (2, 3): n, m = 100, 500
    # elif tid in (6, 7): n, m = 50000, 100000; mode = 1  # 性质 A
    # 占位，防止未填就运行出垃圾数据：
    if tid == 1:
        n, m = 2, 1
    else:
        n, m = 2, 1

    # ============ 二、生成数据（按上面的参数） ============
    # TODO: 按题面输入格式逐行 print。注意特殊性质 mode 的分支。
    # 禁止：任何 IO(...).output_gen(...) 调用（.out 由 verify.sh 产出）。

    # 例：按 mode 构造图骨架
    if mode == 1:  # 性质 A：树是一条链
        graph = Graph.chain(n, directed=directed)
    elif mode == 2:  # 性质 B：菊花图
        graph = Graph.flower(n, directed=directed)
    elif mode == 3:  # 性质 C：随机树（m 忽略）
        graph = Graph.tree(n, directed=directed)
    else:  # 无性质：随机图（去自环/重边按题面调整）
        m = min(m, n * (n - 1) // 2) if not directed else min(m, n * (n - 1))
        graph = Graph.graph(n, m, directed=directed,
                            self_loop=False, repeated_edges=False)

    s = randint(1, n)
    print(n, m, s)
    # Graph 默认 str 按行输出 u v w；无权题面用下一行的 unweighted_edge。
    print(graph)
    # print(graph.to_str(output=Graph.unweighted_edge))  # 无权题面用这行
    # 若题面边格式特殊（如先输出边数），按需调整打印顺序。


if __name__ == "__main__":
    main()
