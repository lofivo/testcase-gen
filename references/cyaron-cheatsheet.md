# cyaron 速查（py轨，可选）

本 skill 默认 cpp 轨（testlib `gen.cpp`）。仅当题目结构化强（图/树）时可选 py 轨（cyaron `gen.py`）。
无论哪条轨：`.out` 仍由 `verify.sh` 统一调 `std` 产出，`validator` 仍是 testlib，错解自检口径不变（见 ADR-0003）。

## 选轨

- 数组/区间/字符串/数列 → cpp 轨（更快，满 `n` 大数据首选 C++）。
- 图/树/DAG/连通图/卡最短路 → py 轨（`Graph.*` 一行出骨架）。
- 第 3 步测点方案必须写明“本题目用 py 轨，因为……”；`work/gen.py` 存在即分流。

## 硬约束

1. 只写 `.in`：`print` 到 stdout（`verify.sh` 重定向到 `data/x.in`）。**禁 `IO.output_gen`**，build 会直接拦截含 `output_gen` 的 `gen.py`。
2. 可复现：模板已 `random.seed(tid)`；不要引入无 seed 的随机源。cyaron 底层全走 Python `random`，seed 后可复现。
3. 性质仍是硬约束：先按性质构造骨架，再叠加随机扰动；满 `n` 性质点不得丢性质。

## Graph 常用（一行出骨架）

```python
from cyaron import Graph
from cyaron import randint

Graph.chain(n, directed=False)                 # 链
Graph.flower(n, directed=False)                # 菊花
Graph.tree(n, directed=False)                  # 随机树
Graph.tree(n, chain=0.7)                       # 偏链的随机树（0..1 插值）
Graph.tree(n, flower=0.7)                      # 偏菊花的随机树
Graph.binary_tree(n)                           # 二叉树
Graph.graph(n, m, directed=False,
            self_loop=False, repeated_edges=False)  # 随机图
Graph.graph(n, m, weight_limit=(1, 10))        # 带权图（输出 u v w）
Graph.DAG(n, m)                                # 有向无环图
Graph.connected(n, m)                          # 连通图
Graph.hack_spfa(n, m)                          # 卡 SPFA 构造
```

输出：`print(graph)` 按行输出 `u v w`；无权题面用 `print(graph.to_str(output=Graph.unweighted_edge))`。
节点编号打乱防标号规律：cyaron 内部已 shuffle；如需定向加噪，在 `weight_gen` / `father_gen` 传 lambda。

## testlib ↔ cyaron 对照（图/树）

| 需求 | testlib（cpp轨） | cyaron（py轨） |
| --- | --- | --- |
| 链 | 连 `(i,i+1)` 再 `shuffle` | `Graph.chain(n)` |
| 菊花 | 中心连其余 | `Graph.flower(n)` |
| 随机树 | prufer 或逐点连已有点 | `Graph.tree(n)` |
| 偏链/偏菊花 | 手写 father 分布 | `Graph.tree(n, chain=0.7)` |
| 二叉树 | 逐点挂左右儿子 | `Graph.binary_tree(n)` |
| 随机图（无重边自环） | `set<pair>` 去重 | `Graph.graph(n,m,self_loop=False,repeated_edges=False)` |
| DAG | 边定向小→大 | `Graph.DAG(n,m)` |
| 卡暴力最坏图 | 手写对抗构造 | `Graph.hack_spfa` + 手写长链/稠密 |

## 性能提醒

- 大数据满 `n`（如 5×10⁴ 点、10⁵ 边、10⁶ 向量）优先 cpp 轨；Python 生成慢一个量级是正常的。
- py 轨适合中小结构化构造与原型；若 py 轨生成超时，先降规模验证逻辑，再切 cpp 轨放大。

## 版本

- `pip install cyaron`，`build.sh` 会校验可 import 并打印版本；出题产物建议在 `README.md` 记录本次 cyaron 版本。
- 已验证：`0.7.0`（`requires_python>=3.6`），`python3.13` 可用。
