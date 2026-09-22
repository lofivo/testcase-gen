# SPJ 接入：题面特定判题器 + 骨架模板，通用判题器保持精确匹配

v1 曾把 SPJ（多解/浮点/输出合法性验证）整体排除；本次纳入，但形态是：通用 `templates/checker.cpp` 保持逐 token 精确匹配不动，SPJ 题由 LLM 按题面另写 `work/checker.cpp`（testlib 三参，`build.sh`/`verify.sh` 链路零改动），辅以 `templates/spj.cpp` 骨架与 `references/spj-guide.md`。否决了把精确匹配 checker 改成双模式（命令行/宏切换）——模式开关会把题面逻辑塞进通用件，违背 ADR-0002 混合架构；SPJ 判定逻辑天然是「每题专属」的运行时产物。std/.out 仍必须产出作参考基线，但 SPJ 题 checker 不要求与 out 逐字一致；浮点容差只认题面声明；写完强制两步自检（std 过 AC、非法输出 WA）防过松过紧。错解自检口径（ADR-0001）不变。
