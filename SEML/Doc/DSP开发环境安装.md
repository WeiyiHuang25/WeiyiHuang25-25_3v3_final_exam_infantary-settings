# DSP开发环境安装
## Code Compaser Studio 配置

打开[Code Composer Studio 集成式开发环境 (IDE)](https://www.ti.com.cn/tool/cn/download/CCSTUDIO/)，选择对应操作系统版本的进行下载安装。

安装完成后下载SDK，以[C2000WARE-软件开发套件](https://www.ti.com.cn/tool/cn/C2000WARE-DIGITALPOWER-SDK#overview)为例，点进去官网,下载安装。

## CCS使用(以F28377S为例)
> 本文参考:https://gitee.com/wu-donghuan/tms320-f28379-d/blob/master/
>
> 视频链接:https://www.bilibili.com/video/BV1Ku411e71e/

### 1. 新建CCS工程

首先打开CCS，菜单栏Project->New CCS Project

![Alt text](Pictures\DSP\image-6.png)

1.Target:选择TMS320F28379D

2.Connection：选择XDS100v2 USB Debug Probe

3.Project name:工程名称，我们这里输入28379d_temp

4.在Project type and tool-chain的Output format：选择eabi（ELF）

5.Projext templates and examples 里选择Empty Project

5.点击Finish

生成的文件如下图所示

![Alt text](Pictures\DSP\image-7.png)

### 2. 导入必备所需文件

### 2.1.cmd文件

.cmd 的专业名称叫链接器配置文件，是存放链接器的配置信息----给出了程序空间和数据空间的设置，及编译后各程序段在程序或数据空间的具体位置。

默认位置：

C:\ti\c2000\C2000Ware_4_03_00_00\device_support\f2837xd\common\cmd

C:\ti\c2000\C2000Ware_4_03_00_00\device_support\f2837xd\headers\cmd

在工程目录新建文件夹《**cmd**》

将

**2837xS_Generic_FLASH_lnk.cmd**

2837xS_Generic_RAM_lnk.cmd

F2837xS_Headers_nonBIOS.cmd

三个文件复制到**1.cmd**文件夹下

![1690797740595](Pictures\DSP\1690797740595.jpg)

### 2.2.数学库

文件名：rts2800_fpu32_fast_supplement_eabi.lib

位置：C:\ti\c2000\C2000Ware_4_03_00_00\libraries\math\FPUfastRTS\c28\lib

文件名：rts2800_fpu32_eabi.lib

位置：C:\ti\ccs1230\ccs\tools\compiler\ti-cgt-c2000_22.6.0.LTS\lib

在工程目录新建文件夹《**math_lib**》

将两个文件复制到**math_lib**文件夹下

### 2.3.官方库函数

源文件夹位置：C:\ti\c2000\C2000Ware_4_03_00_00\driverlib\f2837xd\driverlib

在工程目录新建文件夹《**driverlib**》

《**driverlib**》文件夹里新建两个文件夹《**inc**》、《**src**》

将源文件里的文件按照如下形式复制

头文件.h 全都放进《**inc**》文件夹

C函数.c 全都放进《**src**》文件夹

### 2.4.common文件

common文件一般是直接控制寄存器的。

源文件夹位置：C:\ti\c2000\C2000Ware_4_03_00_00\device_support\f2837xd\common

在工程目录新建文件夹《**common**》

复制源文件夹里的 《**include**》、《**source**》文件夹到《**common**》

### 2.5.headers直接控制寄存器文件

配合common文件使用。一些结构体之类的。

源文件夹位置：C:\ti\c2000\C2000Ware_5_00_00_00\device_support\f2837xd\headers

同理，将文件复制。

在工程目录新建文件夹《**headers**》

### 2.6.user文件

用CCS导入任意一个例程，以gpio为例，文件路径：C:\ti\c2000\C2000Ware_5_00_00_00\driverlib\f2837xs\examples\cpu1\gpio

点击file-Import，弹出如下页面，选择CCS Projects

![1690806446136](Pictures\DSP\1690806446136.jpg)

打开上面文件所在路径的文件夹，会弹出如下页面，选择一个工程点击Finish即导入完成。

![1690806582535](Pictures\DSP\1690806582535.jpg)

在工程目录新建文件夹《**user**》,将这四个文件复制到刚刚创建的文件夹里面，把gpio_ex2_toggle.c的内容复制到main.c内。

![1690806808699](Pictures\DSP\1690806808699.jpg)



## 3.配置工程属性

Debug：将代码烧录进DRAM，掉电及丢失，但是速度相对较快，主要用于烧录调试用。

Release：将代码烧录进FLASH，掉电不丢失，但是速度相对较慢，主要用于烧录调试好的代码。

工程右键，选择Properties

![image-20230617214346396](https://gitee.com/wu-donghuan/tms320-f28379-d/raw/master/28379d_temp/28379D%E5%B7%A5%E7%A8%8B%E6%90%AD%E5%BB%BA%E6%95%99%E7%A8%8B/28379D%E5%B7%A5%E7%A8%8B%E6%90%AD%E5%BB%BA%E6%95%99%E7%A8%8B.assets/image-20230617214346396.png)

###  3.1.添加数学库

![1690798146715](Pictures\DSP\1690798146715.jpg)

###  3.2.添加预定义符号

#### Debug

> CPU1
>
> _LAUNCHXL_F28377S
>

![image-20230617214957460](https://gitee.com/wu-donghuan/tms320-f28379-d/raw/master/28379d_temp/28379D%E5%B7%A5%E7%A8%8B%E6%90%AD%E5%BB%BA%E6%95%99%E7%A8%8B/28379D%E5%B7%A5%E7%A8%8B%E6%90%AD%E5%BB%BA%E6%95%99%E7%A8%8B.assets/image-20230617214957460.png)

#### Release

> _LAUNCHXL_F28377S
>
> _FLASH
>
> _STANDALONE
>
> CPU1

### 3.3.添加头文件路径

将头文件路径都添加进来，如下图所示：

![1690799397125](Pictures\DSP\1690799397125.jpg)

最好把路径改成工作区路径 "**${PROJECT_ROOT}**"

### 3.4.添加文件搜索路径

如下图所示：

![1690799671915](Pictures\DSP\1690799671915.jpg)

把以.lib结尾的文件添加进去。

### 3.5. 编译查找问题

- cmd的重复定义

  如果是Debug 就是需要烧录到RAM 所以把2837xS_Generic_FLASH_lnk.cmd禁用掉

![1690808198362](C:\Users\Sen7Yellow\OneDrive\project\Dc-Ac\SEML\Doc\Pictures\DSP\1690808198362.jpg)

​		右键flash那个文件，点击"Exclude form build"即可禁用。

​		如果是Release则把2837xS_Generic_RAM_lnk.cmd禁用掉。

### 3.6. 尝试编译烧录

![1690808365079](C:\Users\Sen7Yellow\OneDrive\project\Dc-Ac\SEML\Doc\Pictures\DSP\1690808365079.jpg)

点击小铁锤旁边的小箭头可以选择编译类型，分别对Debug和Release进行测试。

选择debug模式是烧录到dram中，烧录速度快，但断电后代码会消失。release模式是烧录到flash中，烧录速度慢。

使用debug模式烧录，如果代码能正常执行说明debug模式没问题。

使用release模式烧录，如果代码能正常执行且reset后依然能够正常执行，说明release模式没问题。

## 常见问题

#### Error: C2000 SysConfig version 5.00.00.00 requires at least version 1.15.0 of SysConfig.

![Error: C2000 SysConfig version 5.00.00.00 requires at least version 1.15.0 of SysConfig.](Pictures\DSP\image.png)

> step1:安装1.15.0以上的[SYSCONFIG](https://www.ti.com/tool/download/SYSCONFIG)，前往官网下载安装。
>
> step2:替换掉老版本的config
>
> ![Alt text](Pictures\DSP\image-1.png)
>
> 打开工程，找到SysConfig，右击打开Products，选中Sysconfig
>
> ![Alt text](Pictures\DSP\image-2.png)
>
> 点击Edit，然后点击Open Preferences
>
> ![Alt text](Pictures\DSP\image-3.png)
>
> 选择刚刚安装的SysConfig目录
>
> ![Alt text](Pictures\DSP\image-4.png)
>
> 选择完毕后选择大于1.15.0以上的SysConfig版本
>
> ![Alt text](Pictures\DSP\image-5.png)

#### Texas Instruments XDS100v2 USB Debug Probe_0/C28xx_CPU1 : Target must be connected before loading program.

编译正常通过，但是下载的时候提示这个问题，出现这个问题通常是因为项目之前改过名。解决方法如下：

> ![1690852160970](C:\Users\Sen7Yellow\OneDrive\project\Dc-Ac\SEML\Doc\Pictures\DSP\1690852160970.jpg)
>
> ![1690852175447](C:\Users\Sen7Yellow\OneDrive\project\Dc-Ac\SEML\Doc\Pictures\DSP\1690852175447.jpg)
>
> [Texas Instruments XDS2xx USB Debug Probe_0/C28xx : Target must be connected before loading program._那一缕时光的博客-CSDN博客](https://blog.csdn.net/weixin_44117735/article/details/121385194)

# 快速入门指南

以F28377S为例,介绍如何了解如何快速上手Ti的dsp库。有32基础的话个人比较喜欢的入手办法就是看官方例程库配合官方说明文档食用。

[TMS320F2837xS 微控制器 技术参考手册](https://www.ti.com.cn/cn/lit/ug/spruhx5g/spruhx5g.pdf?ts=1690858064798&ref_url=https%3A%2F%2Fwww.ti.com.cn%2Fproduct%2Fzh-cn%2FTMS320F28374S%3FkeyMatch%3DTMS320F28374S%26tisearch%3Dsearch-everything%26usecase%3DGPN)

[C2000 ePWM 开发人员指南](https://www.ti.com.cn/cn/lit/an/zhcaca7a/zhcaca7a.pdf?ts=1690805636341&ref_url=https%3A%2F%2Fcn.bing.com%2F)

[TMS320F2837xS 微控制器 数据手册](https://www.ti.com.cn/cn/lit/ds/symlink/tms320f28374s.pdf?ts=1690772623182&ref_url=https%3A%2F%2Fwww.ti.com.cn%2Fsitesearch%2Fzh-cn%2Fdocs%2Funiversalsearch.tsp%3FlangPref%3Dzh-CN%26searchTerm%3DTMS320F28374SPTPT%26nr%3D24)

## 1.时钟配置

F28377S的时钟树如下图所示：

![1690807559399](Pictures\DSP\1690807559399.jpg)

打开device.c可以看到时钟初始化代码如下：

```c
void Device_init(void)
{
    //
    // 关闭看门狗
    //
    SysCtl_disableWatchdog();

#ifdef CMDTOOL
    CMD_init();
#endif

#ifdef _FLASH
#ifndef CMDTOOL
    //
    // Copy time critical code and flash setup code to RAM. This includes the
    // following functions: InitFlash();
    //
    // The RamfuncsLoadStart, RamfuncsLoadSize, and RamfuncsRunStart symbols
    // are created by the linker. Refer to the device .cmd file.
    //
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
#endif

    //
    // Call Flash Initialization to setup flash waitstates. This function must
    // reside in RAM.
    //
    Flash_initModule(FLASH0CTRL_BASE, FLASH0ECC_BASE, DEVICE_FLASH_WAITSTATES);
#endif
#ifdef CPU1

    //
    // Configure Analog Trim in case of untrimmed or TMX sample
    //
    if((SysCtl_getDeviceParametric(SYSCTL_DEVICE_QUAL) == 0x0U)       &&
       (HWREGH(ANALOGSUBSYS_BASE + ASYSCTL_O_ANAREFTRIMA) == 0x0U))
    {
        Device_configureTMXAnalogTrim();
    }

    //
    // 设置PLL和时钟分频器
    //
    SysCtl_setClock(DEVICE_SETCLOCK_CFG);

    //
    // 确保LSPCLK分频器设定为默认值 (4分频)
    //
    SysCtl_setLowSpeedClock(SYSCTL_LSPCLK_PRESCALE_4);

    //
    // These asserts will check that the #defines for the clock rates in
    // device.h match the actual rates that have been configured. If they do
    // not match, check that the calculations of DEVICE_SYSCLK_FREQ and
    // DEVICE_LSPCLK_FREQ are accurate. Some examples will not perform as
    // expected if these are not correct.
    //
    ASSERT(SysCtl_getClock(DEVICE_OSCSRC_FREQ) == DEVICE_SYSCLK_FREQ);
    ASSERT(SysCtl_getLowSpeedClock(DEVICE_OSCSRC_FREQ) == DEVICE_LSPCLK_FREQ);

#ifndef _FLASH
    //
    // Call Device_cal function when run using debugger
    // This function is called as part of the Boot code. The function is called
    // in the Device_init function since during debug time resets, the boot code
    // will not be executed and the gel script will reinitialize all the
    // registers and the calibrated values will be lost.
    // Sysctl_deviceCal is a wrapper function for Device_Cal
    //
    SysCtl_deviceCal();
#endif

#endif
    //
    // 打开所有外设，可以进去注释掉不需要的注释以降低功耗
    //
    Device_enableAllPeripherals();

    //
    // Initialize result parameter as FAIL
    //
    Example_Result = FAIL;
}
```

其中,设置PLL的`DEVICE_SETCLOCK_CFG`对应宏定义如下：

```c
//
// Define to pass to SysCtl_setClock(). Will configure the clock as follows:
// PLLSYSCLK = 10MHz (XTAL_OSC) * 40 (IMULT) * 1 (FMULT) / 2 (PLLCLK_BY_2)
//
#define DEVICE_SETCLOCK_CFG         (SYSCTL_OSCSRC_XTAL | SYSCTL_IMULT(40) |  \
                                     SYSCTL_FMULT_NONE | SYSCTL_SYSDIV(2) |   \
                                     SYSCTL_PLL_ENABLE)
//
// 200MHz SYSCLK frequency based on the above DEVICE_SETCLOCK_CFG. Update the
// code below if a different clock configuration is used!
//
#define DEVICE_SYSCLK_FREQ          ((DEVICE_OSCSRC_FREQ * 40 * 1) / 2)
```

这里对应的是100MHz的时钟配置，10MHz的晶振频率进行40倍，在进行2分屏得到200MHz主频。这两条宏定义的值要相互对应。

## 2.GPIO

和32一样，dsp也是把GPIO分为Port和Pin，不同的是dsp是以Pin命名引脚，而32是以Port+Pin来命名。dsp的Port和Pin对应关系如下：

| Port   | Pin             |
| ------ | --------------- |
| Port A | GPIO0~GPIO31    |
| Port B | GPIO32~GPIO63   |
| Port C | GPIO64~GPIO95   |
| Port D | GPIO96~GPIO127  |
| Port E | GPIO128~GPIO159 |
| Port F | GPIO160~GPIO168 |

GPIO框图如下：

![1690859601418](Pictures\DSP\1690859601418.jpg)

### 2.1.点亮LED灯

f28377s-launchpad上的LED位于Pin12,Pin13引脚上，使用低电平点亮。

```c
void LED_Init(){
    GPIO_setPinConfig(DEVICE_GPIO_PIN_LED1);  // 设置引脚配置为 普通GPIO
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED1, GPIO_PIN_TYPE_STD); // 设置引脚为 推挽输出或者浮空输入
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED1, GPIO_DIR_MODE_OUT); // 设置为输出模式
    GPIO_setControllerCore(DEVICE_GPIO_PIN_LED1, GPIO_CORE_CPU1); // 用于CPU1

    GPIO_setPinConfig(DEVICE_GPIO_PIN_LED2);
    GPIO_setPadConfig(DEVICE_GPIO_PIN_LED2, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_LED2, GPIO_DIR_MODE_OUT);
    GPIO_setControllerCore(DEVICE_GPIO_PIN_LED2, GPIO_CORE_CPU1);
}
```

进入gpio.h得知可以把引脚配置为：

```c
//*****************************************************************************
//
// Values that can be passed to GPIO_setPadConfig() as the pinType parameter
// and returned by GPIO_getPadConfig().
//
//*****************************************************************************
#define GPIO_PIN_TYPE_STD       0x0000U //!< 推挽或者浮空输入模式
#define GPIO_PIN_TYPE_PULLUP    0x0001U //!< 上拉输入模式
#define GPIO_PIN_TYPE_INVERT    0x0002U //!< 反转输入模式
#define GPIO_PIN_TYPE_OD        0x0004U //!< 开漏输出模式
//*****************************************************************************
//
//! Values that can be passed to GPIO_setDirectionMode() as the \e pinIO
//! parameter and returned from GPIO_getDirectionMode().
//
//*****************************************************************************
typedef enum
{
    GPIO_DIR_MODE_IN,                   //!< 该引脚为GPIO输入模式
    GPIO_DIR_MODE_OUT                   //!< 该引脚为GPIO输出模式
} GPIO_Direction;
//*****************************************************************************
//
//! Values that can be passed to GPIO_setControllerCore() as the \e core
//! parameter.
//
//*****************************************************************************
typedef enum
{
    GPIO_CORE_CPU1,             //!< 由cpu1控制
    GPIO_CORE_CPU1_CLA1         //!< 由cpu1的cla1控制
} GPIO_CoreSelect;
```

继续翻阅gpio.h，不难发现对引脚的读写函数为:

```c
GPIO_togglePin(uint32_t pin); // 反转引脚电平
GPIO_writePin(uint32_t pin, uint32_t outVal); // 给引脚写0或者写1
GPIO_readPin(uint32_t pin); // 读取引脚电平
```

于是我们的主函数可以写为：

```c
int main(void)
{
    //
    // Initializes system control, device clock, and peripherals
    //
    Device_init();

    //
    // Initializes PIE and clear PIE registers. Disables CPU interrupts.
    // and clear all CPU interrupt flags.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // 初始化gpio
    //
    LED_Init();
    GPIO_writePin(DEVICE_GPIO_PIN_LED1,1);
    GPIO_writePin(DEVICE_GPIO_PIN_LED2,0);
    
    //
    // Enables CPU interrupts
    //
    Interrupt_enableMaster();

    //
    // Loop.
    //
    for(;;)
    {
        GPIO_togglePin(DEVICE_GPIO_PIN_LED1);
        GPIO_togglePin(DEVICE_GPIO_PIN_LED2);
        DEVICE_DELAY_US(100000);
    }
}
```

效果是红蓝两LED闪烁

### 2.2.读取按键输入

由于f28377s-launchpad上没有专门的用户按键，所以用杜邦线将pin2插入到gnd来代替按键按下

```c
void KEY_Init()
{
    GPIO_setPinConfig(DEVICE_GPIO_PIN_KEY); // 设置引脚配置为 普通GPIO
    GPIO_setPadConfig(DEVICE_GPIO_PIN_KEY, GPIO_PIN_TYPE_PULLUP); // 设置引脚为 推挽输出或者浮空输入
    GPIO_setDirectionMode(DEVICE_GPIO_PIN_KEY, GPIO_DIR_MODE_IN); // 设置为输入模式
    GPIO_setQualificationPeriod(3,510); // 设置GPIO0~7(组3)采样窗口为510.
    GPIO_setQualificationMode(DEVICE_GPIO_PIN_KEY,GPIO_QUAL_6SAMPLE); // 六个采样周期有效
    GPIO_setControllerCore(DEVICE_GPIO_PIN_KEY, GPIO_CORE_CPU1); // 用于CPU1
}
```

出现了两个新函数:`GPIO_setQualificationPeriod`和`GPIO_setQualificationMode`，其他与设置LED基本一样。翻阅gpio.h。

```c

//*****************************************************************************
//
//! Values that can be passed to GPIO_setQualificationMode() as the
//! \e qualification parameter and returned by GPIO_getQualificationMode().
//
//*****************************************************************************
typedef enum
{
    GPIO_QUAL_SYNC,                     //!< Synchronization to SYSCLK
    GPIO_QUAL_3SAMPLE,                  //!< Qualified with 3 samples
    GPIO_QUAL_6SAMPLE,                  //!< Qualified with 6 samples
    GPIO_QUAL_ASYNC                     //!< No synchronization
} GPIO_QualificationMode;
//*****************************************************************************
//
//! Sets the qualification mode for the specified pin.
//!
//! \param pin is the identifying GPIO number of the pin.
//! \param qualification specifies the qualification mode of the pin.
//!
//! This function sets the qualification mode for the specified pin. The
//! parameter \e qualification can be one of the following values:
//! - \b GPIO_QUAL_SYNC
//! - \b GPIO_QUAL_3SAMPLE
//! - \b GPIO_QUAL_6SAMPLE
//! - \b GPIO_QUAL_ASYNC
//!
//! To set the qualification sampling period, use
//! GPIO_setQualificationPeriod().
//!
//! \return None.
//
//*****************************************************************************
extern void
GPIO_setQualificationMode(uint32_t pin, GPIO_QualificationMode qualification);
//*****************************************************************************
//
//! Sets the qualification period for a set of pins
//!
//! \param pin is the identifying GPIO number of the pin.
//! \param divider specifies the output drive strength.
//!
//! This function sets the qualification period for a set of \b 8 \b pins,
//! specified by the \e pin parameter. For instance, passing in 3 as the value
//! of \e pin will set the qualification period for GPIO0 through GPIO7, and a
//! value of 98 will set the qualification period for GPIO96 through GPIO103.
//! This is because the register field that configures the divider is shared.
//!
//! To think of this in terms of an equation, configuring \e pin as \b n will
//! configure GPIO (n & ~(7)) through GPIO ((n & ~(7)) + 7).
//!
//! \e divider is the value by which the frequency of SYSCLKOUT is divided. It
//! can be 1 or an even value between 2 and 510 inclusive.
//!
//! \return None.
//
//*****************************************************************************
extern void
GPIO_setQualificationPeriod(uint32_t pin, uint32_t divider);
```

查阅技术参考手册831页可知，这两条函数分别是配置输入滤波器的信号有效周期和采样频率的，使用`GPIO_QUAL_3SAMPLE`的只有当信号连续三次采样都为相同电平才会改变电平状态，通常用来进行消抖。而采样频率是每时钟频率/分频值才采样一次，设置为`GPIO_QUAL_ASYNC`则不会进行滤波。

![1690860357301](C:\Users\Sen7Yellow\OneDrive\project\Dc-Ac\SEML\Doc\Pictures\DSP\1690860357301.jpg)

采样频率配置每八个为一组，计算公式为GPIO (n & ~(7))+3，即GPIO0-GPIO7的配置引脚为3,GPIO96-GPIO103配置为98。

于是，读取按键电平的主函数如下：

```c
int main(void)
{
    uint8_t temp;
    //
    // Initializes system control, device clock, and peripherals
    //
    Device_init();

    //
    // Initializes PIE and clear PIE registers. Disables CPU interrupts.
    // and clear all CPU interrupt flags.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // GPIO初始化
    //
    GPIO_init();

    //
    // Enables CPU interrupts
    //
    Interrupt_enableMaster();

    //
    // Loop.
    //
    for(;;)
    {
        if(temp == 1)
        {
            GPIO_togglePin(DEVICE_GPIO_PIN_LED1);
            GPIO_togglePin(DEVICE_GPIO_PIN_LED2);
        }
        if(GPIO_readPin(DEVICE_GPIO_PIN_KEY) != 1)
        {
            while(GPIO_readPin(DEVICE_GPIO_PIN_KEY)==0);
            temp = !temp;
        }
        DEVICE_DELAY_US(100000);
    }
}
```

## 3.中断系统

f28377s的中断系统由三级中断构成，第一级为外设中断，第二级位PIE中断，第三级位CPU中断。

![1690861653723](C:\Users\Sen7Yellow\OneDrive\project\Dc-Ac\SEML\Doc\Pictures\DSP\1690861653723.jpg)

如图3.1所示，tim1和tim2中断直接连接到cpu，外部中断是连接到PIE中断再连接CPU中断的，所以使能中断需要使能外部中断、PIE中断、CPU中断。

dsp的中断服务函数定义和stm32不太一样，dsp的中断服务函数采用回调函数的形式，通过定义 

### 3.1.外部中断

了解完中断系统后再看前面2.1的初始化代码的含义了

```c
    //
    // 初始化系统时钟、使能位外设
    //
    Device_init();

    //
    // 初始化PIE并清除PIE寄存器。禁用CPU中断。并清除所有CPU中断标志。
    //
    Interrupt_initModule();

    //
    // 用指向shell中断服务例程(ISR)的指针初始化PIE向量表。
    //
    Interrupt_initVectorTable();
```

外部中断初始化只要

```c
    GPIO_init(); // 初始化io口
	GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_FALLING_EDGE); // 设置XINT1中断类型为下降沿触发
    GPIO_setInterruptPin(DEVICE_GPIO_PIN_KEY, GPIO_INT_XINT1); // 将外部中断和GPIO口绑定
    GPIO_enableInterrupt(GPIO_INT_XINT1); // 使能XINT1
    Interrupt_register(INT_XINT1, &EXTI_Interrupt_Handler); // 注册中断回调函数
```

