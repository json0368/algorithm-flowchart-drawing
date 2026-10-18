# 流程图样例索引

以下八张图以正则表达式预处理为例，展示算法展开程度、中文节点表述、循环分支及紧凑布局。每张图同时提供可编辑 draw.io 文件与 PNG 预览。

需要对照源代码理解画法时，阅读[样例源码与流程图对照](source-code.md)，其中提供相关函数、必要数据结构及八张图对应的代码行段。

根据待绘算法选择相关样例查看，不必一次加载全部图片。样例中的图号、算法、变量和图的数量属于具体示例；绘制其他算法时，按实际代码、报告结构及用户要求调整。

| 样例 | 内容 | 可参考的画法 | 文件 |
| --- | --- | --- | --- |
| a | 正则表达式预处理总体流程 | 总体图与各处理阶段的衔接 | [draw.io](../assets/preprocess-eight-figures/figure-3-2-a.drawio) · [PNG](../assets/preprocess-eight-figures/figure-3-2-a.png) |
| b | 删除普通空格 | 嵌套循环、字符复制和下标更新 | [draw.io](../assets/preprocess-eight-figures/figure-3-2-b.drawio) · [PNG](../assets/preprocess-eight-figures/figure-3-2-b.png) |
| c | 单条规则的读取、分类与编码分配 | 多分支分类、编码记录与局部处理 | [draw.io](../assets/preprocess-eight-figures/figure-3-2-c.drawio) · [PNG](../assets/preprocess-eight-figures/figure-3-2-c.png) |
| d | 递归宏展开及循环引用检查 | 递归调用、活动集合维护和异常出口 | [draw.io](../assets/preprocess-eight-figures/figure-3-2-d.drawio) · [PNG](../assets/preprocess-eight-figures/figure-3-2-d.png) |
| e | 字符类定位、转义读取、范围展开与替换 | 在一张图中组织定位、读取、枚举和替换 | [draw.io](../assets/preprocess-eight-figures/figure-3-2-e.drawio) · [PNG](../assets/preprocess-eight-figures/figure-3-2-e.png) |
| f | 取反运算的操作数定位及括号补全 | 操作数分类、括号匹配与分支汇合 | [draw.io](../assets/preprocess-eight-figures/figure-3-2-f.drawio) · [PNG](../assets/preprocess-eight-figures/figure-3-2-f.png) |
| g | 正闭包运算改写 | 操作数定位、字符串改写与循环回接 | [draw.io](../assets/preprocess-eight-figures/figure-3-2-g.drawio) · [PNG](../assets/preprocess-eight-figures/figure-3-2-g.png) |
| h | 可选运算改写 | 条件分支、表达式替换与遍历继续 | [draw.io](../assets/preprocess-eight-figures/figure-3-2-h.drawio) · [PNG](../assets/preprocess-eight-figures/figure-3-2-h.png) |

需要参考总体与详情图的划分时，先看 a，再选相应详情图。需要参考循环和下标更新时，可看 b、g、h；需要参考较多分支或递归时，可看 c、d、e、f。

图中相同字母的同页圆形连接点表示流程衔接。原生文件可用于查看节点尺寸、字体、端口及折线路径；PNG 用于观察整体构图。样例图片不能替代新图在目标报告尺寸下的可读性检查。
