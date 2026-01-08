# SEML：Singularity Embedded Middlewares Layer Library
**奇点机器人实验室嵌入式软件中间件层库**

<div align=center>
	<p>
		<img src="https://img.shields.io/badge/version-alpha-blue"/>
		<img src="https://img.shields.io/badge/license-GNU3.0-green"/>
    </p>
   	<p>
		<img src="https://gitee.com/SenLinks/super-capacitor-control-board/badge/star.svg"/>
		<img src="https://gitee.com/SenLinks/super-capacitor-control-board/badge/fork.svg"/>
	</p>
</div>

## 目录

[TOC]

## 简介

​	SEML旨在将已有的代码解耦，模块化后封装成一个服务于组织内所有软件项目的中间件层，形成实验室内部软件开发标准接口，以解决实验室代码混乱的问题。同时机器人软件开发者需要维护、升级已有的模块，而无需为了同一个功能重复造轮子，提高软件开发效率和质量。如果你认为自己对某个模块有更好的代码方案，或者发现了现有模块不再满足当前使用需求，请仔细阅读完原有代码后向实验室其他队员收集需求意见然后再进行重构。重构时请务必谨慎考虑模块接口的设置！

SEML主要结构如下：

 - 驱动层(Drivers)：提供硬件和软件之间接口
   - 板级支持层(Board Support Pack，BSP)：提供硬件驱动及接口，通常BSP层都是由厂家或者网上开源。
   - 硬件抽象层(Hardware Abstract Layer，HAL)：对BSP层进一步封装，形成统一的接口形式。
 - 中间件(Middlewares)：提供基础的软件层面支持
   - 数学支持库(Math Library)：提供数学运算。
   - 控制算法支持库(Controler Library)：提供各种各样的控制器。
   - 数据结构支持库(Data Structure Library)：提供通用的数据结构类型。
   - 系统库(System Library)：提供诸如内存管理、任务间通讯、外部通讯、调试等功能。
   - 解算库(Solution Library)：提供解算算法，如陀螺仪解算、底盘运动模型解算、功率限制等。
   - 通讯库(Communications Library)：提供电机通讯、裁判系统解算、多机通讯等。
 - 应用层(Application)
   - 底盘功能库：实现基础的底盘功能，并且一些额外功能也在其中。
   - 云台功能库：实现基础的云台功能，并且一些额外功能也在其中。
   - 发射机构功能库：实现基础的发射机构功能，并且一些额外功能也在其中。
   - 自瞄功能库：实现基础的自喵，自喵相关的函数也在其中。
   - 机械臂功能库：工程的爪子。

库函数和各个车之间联动使用git的子模块功能，如果库函数出现更改可以使用git进行同步，这样就一套代码所有车都能用，只要修改每台车的配置信息就行。

## 说明文档

### 库函数说明文档

​	说明文档基于Doxygen生成，访问此[链接](/Doc/html/index.html)([备用链接](/SEML/Doc/html/index.html))即可打开说明文档。

​	若无法打开请使用Doxygen重新生成一下，默认上传SEML库的时候是排除该Doxygen输出文件夹(上千个文件在git上属实看了都头大)。

​	若使用vscode打开会进入html编辑界面，需安装插件："open in browser" 找到作者为TechER的进行安装。按art+B用浏览器打开此文档。

​	在开发时，函数外部注释时候严格按照Doxygen语法书写注释，详见：[SEML库代码规范](.\Doc\代码规范.md)。

​	doxygen语法和使用详见：[干货|教你使用Doxygen制作漂亮的程序文档](https://zhuanlan.zhihu.com/p/510925324)。

### 开发前必看文档

这部分文档为规范类文档，详细阐述了应该如何高效、有质量的进行Code。

​		\- [SEML库代码规范](.\Doc\代码规范.md)

### 核心模块简介

该部分文档为SEML库核心模块，也算是比较复杂的重难点，可以说整个SEML库都是基于这部分模块搭建而成，搞懂了这些模块就能快速上手SEML库。

​		\- [HAL层-外设接口层](.\Drivers\hal\Interface\Interface.md)

​		\- [系统库-消息模块](.\Middlewares\System\message.md)

​		

## git子模块使用说明
在当前项目根目录打开git bush here，输入：
```
  git config --global user.name "Your Name"
  git config --global user.email "youremail@yourdomain.com"
  git submodule add -f https://gitee.com/Zhku_qidian/SEML.git
```
然后使用vscode打开，会出现如下界面

![README1](./Doc/Pictures/README/README1.jpg)

上面是当前工程的git管理，下面就是git的子模块管理。

**若不是要增加功能或者修复bug 请！勿！改！动！SEML内任何内容，请在自己的工程文件其他位置进行修改，防止其他人无法正常使用SEML。**(注释也不行！反正你改了也推不上去,到时候拉不下来别问为啥拉不下来)

如果需要更改可以创建新分支进行修改，若测试没问题可以提交Pull Requests，由管理员进行审核推送到master。

对于接口类的请不要在SEML内任何一个文件内进行定义，请在`"main.h"`中进行宏定义，以防别人无法正常使用该库。

若无法推送，请咨询仓库管理员。

## 本库文件包含说明：

**SEML库文件包含如下：**

### DOC

​	内置Doxygen配置文件，以及文档说明。

### Drivers

​	内置所有基本驱动配置文件**BSP**文件夹和硬件抽象层**HAL**文件夹

#### 		BSP

​	BSP层为驱动配置文件，在网络可以收集到，主控与外部模块的接口层文件基于HAL层的Interface层

#### HAL

​	对BSP层和外设层进一步抽象，使其形成一个统一且通用的接口。

​	相关文档：

​		\- [HAL层-外设接口层](.\Drivers\hal\Interface\Interface.md)

### Middlewares

​	内置许许多多常用~~充满bug，屎山~~的软件算法和辅助代码。

#### math

​	数学支持库，主要目的是为SEML提供一个通用、快速的数学计算支持。主要提供单精度浮点数的软件计算和arm_math的算法，里面所有函数都比标准库`<math.h>`要快。如果需要双精度的可以调用`<math.h>`内的函数或者其他第三方库。

#### Solution

​	解算算法库，包括电源类、惯性运动单元解算、底盘运动模型、功率限制等解算算法。理解大致使用方法后基本都可以直接调用使用，前人搞过的解算算法都在这里面，避免重复造轮子。

#### DataStructure

​	数据结构支持库，主要为SEML数据结构支持，主要是队列和栈。

#### Control

​	控制器支持库，里面就是一些比较常用的控制算法，例如PID之类的。

#### System

​	系统库，主要是辅助代码，为了实现代码解耦,将驱动层透明化(真·让你写嵌入式不用跟底层打交道)~~甚至还想在这里实现个操作系统~~。

​	相关文档：

​		\- [系统库-消息模块](.\Middlewares\System\message.md)

#### Communications

​	通讯库，主要是通讯解算代码。

​	相关文档：

​		\- [通讯库-大疆电机驱动模块](.\Middlewares\Communications\DJI_Motor.md)

# 辅助开发工具

## STM32 CubeMX

留个坑，反正用过的都说好

## git

SEML库是基于git的子模块将各车间的代码联系起来的，所以在使用之前熟悉git的配置和使用：[git环境配置和基于VScode的使用方法](.\Doc\git环境配置.md)。

## VScode

**强烈建议不要使用Keil作为你的代码编辑器**，Keil没有代码补全、语法高亮和静态检查的支持，并且也不支持拓展。为了提高生产力，应该使用更现代化、更智能的代码编辑器来作为你的主要编辑器，比如VScode。

关于VScode和Ozone的使用方法直接引用鹿跃战队的开源文档[VSCode+Ozone使用方法.md](.\Doc\VSCode+Ozone使用方法.md)进行配置。

## Ozone

Ozone是Segger公司开发的一套上位机调试软件，支持市面上大多数烧录器。并且支持示波器、断点和代码烧录，使用VSCode+Ozone+Arm GNU工具链可以完全摆脱又老又臭的Keil。

