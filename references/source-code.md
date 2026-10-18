# 样例源码与流程图对照

需要理解源代码如何展开为流程图时，配合[八张图的样例索引](examples.md)阅读本说明。根据待绘内容选择相关代码段，不必一次加载全部源码。

## 文件与适用范围

- [FAGenerator-preprocess.cpp](source/FAGenerator-preprocess.cpp)：预处理函数、递归宏展开，以及 isEscaped、findLeftBracket 两个辅助函数。
- [FAGenerator-preprocess.h](source/FAGenerator-preprocess.h)：上述函数所需的成员容器、标识符编码与函数声明。

样例从项目的 FAGenerator.cpp 与 FAGenerator.h 提取，保留所选函数的原有函数体和成员声明。为方便独立阅读与 C++17 语法检查，补充了标准库头文件，将 cpp 的项目头文件引用改为配套的精简头文件；省略了界面、自动机构造和代码生成等无关部分。

这是一份固定的源码摘录，用于学习算法流程图的表达方式。它不依赖原项目所在目录，也不会随原项目更新。具体算法、输入约定及边界处理属于示例，不应作为其他项目的实现要求；绘制其他算法时，以相应源码为依据。

## 图与代码段

下表行号均对应随 skill 附带的 FAGenerator-preprocess.cpp。

| 图 | 内容 | 代码行 | 图中体现的操作 |
| --- | --- | --- | --- |
| [图 a](../assets/preprocess-eight-figures/figure-3-2-a.png) | 总体流程 | 94—381 | 按顺序调用或执行删除空格、规则分类、宏展开、字符类、取反、正闭包和可选改写，最终保存 processedExp2id。宏展开函数的内部流程由图 d 展开。 |
| [图 b](../assets/preprocess-eight-figures/figure-3-2-b.png) | 删除普通空格 | 97—110 | 对应 i、j、k 三层循环，以及 len 减一、j 回退和 resize。 |
| [图 c](../assets/preprocess-eight-figures/figure-3-2-c.png) | 规则读取、分类与编码 | 112—195 | 对应按等号拆行、登记 macro2exp、branch 分类、L 后缀拆分和编码保存。 |
| [图 d](../assets/preprocess-eight-figures/figure-3-2-d.png) | 递归宏展开 | 32—90 | 对应转义保留、字符类跳过、名称读取、active 插入检查、递归调用、返回后移除与扫描位置更新。 |
| [图 e](../assets/preprocess-eight-figures/figure-3-2-e.png) | 字符类展开 | 206—268 | 对应方括号定位、转义读取、范围检查、字符枚举、替换与 j 的调整。 |
| [图 f](../assets/preprocess-eight-figures/figure-3-2-f.png) | 取反操作数定位及括号补全 | 270—304 | 对应从右向左扫描、普通或转义操作数、括号深度统计、错误出口和两次 insert。这里只是取反运算的预处理，不包含自动机取补算法。 |
| [图 g](../assets/preprocess-eight-figures/figure-3-2-g.png) | 正闭包改写 | 306—339 | 对应跳过转义、把 + 改为 *、处理位于开头的符号、三类前置操作数复制及 j 更新。 |
| [图 h](../assets/preprocess-eight-figures/figure-3-2-h.png) | 可选运算改写 | 341—377 | 对应跳过转义、处理位于开头的问号、把 ? 改为竖线、按操作数插入左括号和 #)，以及 j 更新。 |

## 辅助函数与数据结构

- isEscaped 位于 cpp 第 6—10 行，用于判断反斜杠转义。图中按使用位置描述作用，无须单独拆图。
- findLeftBracket 位于 cpp 第 14—28 行，从右括号向左查找匹配位置，供图 g、h 中的操作数定位使用。
- reExps 保存输入规则，macro2exp 保存宏定义，exp2tokenId 是预处理函数内的待构图表达式列表，processedExp2id 保存最终预处理结果。
- keyword2id 保存关键字与编码，idTokenId 保存标识符编码，commentTokenIds 保存注释编码，active 是宏展开递归路径上的名称集合。

## 阅读时注意

- 先从总体图定位阶段，再将相应详情图中的判断、处理和循环回接逐项对应到代码。
- 总体图表示整个函数的组织关系；详情图可以合并相同目的的相邻语句，但应保留下标更新、递归返回及实际存在的错误路径。
- 代码中的 # 是空串标记，图 h 的 #) 来自可选运算改写。它属于该示例的约定，不是流程图符号。
- 修改样例源码时，应同步核对关联的图、说明和代码行号；不要仅替换为项目最新文件而保留旧图。

## 来源版本标识

以下 SHA-256 标识提取时读取的完整原始文件，供检查源码来源版本；skill 的实际使用只需读取上述摘录。

- FAGenerator.cpp：`72c050cb42286ca4f53f9e62e64917c2762329129002f97f8dc5ffb607cd48aa`
- FAGenerator.h：`a22ffd4d25efd25c627b1fd13330edec67ddb7abf4c7164e755a56d91459b9f3`
