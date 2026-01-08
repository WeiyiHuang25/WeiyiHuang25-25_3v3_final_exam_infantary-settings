# CMSIS库使用说明

ARM® Cortex™ 微控制器软件接口标准 (CMSIS) 是 Cortex-M 处理器系列的与供应商无关的硬件抽象层。CMSIS 可实现与处理器和外设之间的一致且简单的软件接口，CMSIS的目的是让不同厂家的Cortex M的MCU至少在内核层次上能够做到一定的一致性，提高软件移植的效率。

官网：https://www.arm.com/why-arm/technologies/cmsis

在安装完芯片驱动包后，会在Keil\ARM目录下添加进CMSIS的库函数文件和例程。可以用这些来给我们使用需要的功能。

CMSIS的主要内容:

| 名称           | 说明                                                         |
| -------------- | ------------------------------------------------------------ |
| **CMSIS-DSP**  | Arm针对各种Cortex-M处理器内核进行了优化的丰富DSP功能的集合   |
| **CMSIS-RTOS** | 主要用于RTOS的API，可与中间件和库组件实现一致的软件层        |
| **CMSIS-Core** | 提供与cortex内核与外围寄存器之间的接口                       |
| **CMSIS-DAP**  | Cortex调试访问端口（DAP）的标准化接口                        |
| **CMSIS-NN**   | 高效的神经网络内核的集合                                     |
| **CMSIS-SVD**  | 包含完整微控制器系统（包括外设）的程序员视图的系统视图描述 XML 文件 |

## CMSIS-DSP

如果你需要在某个项目里面用到复杂数学运算并且没有高精度需求，那么不要使用标准数学库，`math.h`内的函数都是双精度浮点型的而且是纯软件计算，没有硬件进行加速。cortex内核中绝大部分带浮点协处理器(FPU)都只支持单精度浮点数的计算，为了提高运行效率可以使用CMSIS-DSP库，在支持dsp的处理器中他会调用硬件资源进行加速，不支持dsp的处理器他也会用查表的方法极大的提高运行效率。同样的计算使用CMSIS-DSP库和标准数学库的运行速度差距可达到1000倍。在c语言中，若直接输入一个带小数的类型是默认为双精度浮点数，为了能够使用硬件加速请在每一个浮点数后面加入f，例如：

```c
1.00 \\ 双精度浮点数
1e-3 \\ 双精度浮点数 指数表达
1.0f \\ 单精度浮点数
1e-3 \\ 单精度浮点数 指数表达
```

### 如何导入arm_math

在安装完对应芯片的驱动包后，在`Keil\ARM\CMSIS\Lib\ARM`能找到许多arm_math.lib的二进制文件，选择对应芯片型号的cortex内核的二进制文件，复制到工程里面导入，随后进入`Keil\ARM\CMSIS\Include`找到`arm_math.h`,`arm_const_structs.h`,`arm_common_tables.h`，将这三个文件复制进工程里面即可。

例如stm32f10x是使用cortex-M3内核的，可以直接复制`arm_cortexM3l_math.lib`过去自己的工程内使用。

### 如何使用arm_math

在`Keil\ARM\CMSIS\Documentation\DSP\html`内打开`index.html`或者访问[CMSIS DSP Software Library ](https://www.keil.com/pack/doc/CMSIS/DSP/html/index.html)可以访问CMSIS-DSP库的说明文档，内有各个函数详细的使用说明。

若使用SEML内置数学库，在导入进去arm_math后在main.h内`#include "arm_math.h"`后SEML数学库会自动使用CMSIS-DSP的内容来加快计算速度。

