# 双轨生成器：默认 testlib，图/树题可选 cyaron

## 背景

`property-to-code.md` 已覆盖链/菊花/随机树的 testlib 范式，但二叉树/DAG/连通图/卡最短路等仍需手写，LLM 易错且费行。cyaron（`Graph.chain/flower/tree/binary_tree/DAG/connected/hack_spfa`）可一行出骨架。

## 决定

双轨并存，不替换：

- 默认 cpp 轨（testlib `gen.cpp`）；图/树结构化强的题可选 py 轨（cyaron `gen.py`），满 `n` 大数据优先 cpp 轨。
- 分流规则：`work/gen.py` 存在即 py 轨（两者并存时优先 py 轨并告警）；测点方案必须声明选轨理由。
- 守链路：cyaron 只写 `.in` 到 stdout，禁 `IO.output_gen`（`build.sh` 拦截）；`.out` 仍由 `verify.sh` 统一调 `std` 产出；`validator/checker` 仍是 testlib；错解自检口径（ADR-0001）不变。
- 可复现：py 轨 `random.seed(tid)` 对标 `registerGen`；题目 `README.md` 记录 cyaron 版本号。

## 备选

- 纯替换为 cyaron：否。会丢掉 validator/checker 生态、离线可复现与 C++ 大数据性能。
- 仅文档翻译（不引入运行时）：否。调用方明确允许 `pip install cyaron` 并愿锁版本，运行时收益大于维护成本。

## 后果

- 新增 `templates/gen.py` + `references/cyaron-cheatsheet.md`；`build/verify.sh` 分流；skill 维护两套生成器模板。
- LLM 必须先做选轨声明，避免误用 py 轨做满 `n` 大数据导致生成过慢。
