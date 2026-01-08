# 目录

[TOC]

# git bash安装

来自：[Git 详细安装教程(详解 Git 安装过程的每一个步骤)](https://blog.csdn.net/mukes/article/details/115693833)

## git的下载

Git 官网下载对应系统的软件了，下载地址为 [git-scm.com](https://git-scm.com/)或者[gitforwindows.org](http://gitforwindows.org/),或者[阿里镜像](https://registry.npmmirror.com/binary.html?path=git-for-windows/)。

上面的 git-scm 是 Git 的官方，里面有不同系统不同平台的安装包和源代码，而 gitforwindows.org 里只有 windows 系统的安装包`
`阿里镜像直接Ctrl+F 搜索最新版本前缀即可，如2.40.0.

## git的安装

### Step1.使用许可声明

双击下载后的 `Git-2.40.0-64-bit.exe`，开始安装，这个界面主要展示了GPL第 2 版协议的内容，点击 [next] 到第二步。

![在这里插入图片描述](.\Pictures\git环境配置\e1bba68d04694d5fb98bc92500a6b013.png)

### Step2.选择安装目录

可点击 “Browse…” 更换目录，也可直接在方框里面改，我一般直接将 “C” 改为 “D”，这样就直接安装在 D 盘里了。点击 [next] 到第三步。

![在这里插入图片描述](.\Pictures\git环境配置\dd6fab0c1ae049729a7df0105c8951f4.png)

### Step3.选择安装组件

图中这些英文都比较简单，我已经把大概意思翻译出来了，大家根据自己的需要选择勾选。点击 [next] 到第四步。

![在这里插入图片描述](.\Pictures\git环境配置\watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBAbXVrZXM=,size_20,color_FFFFFF,t_70,g_se,x_16.png)

注：最后一个选项打勾的话，需要下载 Windows Terminal 配合 Git Bash使用

![在这里插入图片描述](.\Pictures\git环境配置\watermark,type_d3F5LXplbmhlaQ,shadow_50,text_Q1NETiBAbXVrZXM=,size_20,color_FFFFFF,t_70,g_se,x_16-16940544639864.png)

### Step4.选择开始菜单文件夹

方框内 Git 可改为其他名字，也可点击 “`Browse...`” 选择其他文件夹或者给"`Don't create a Start Menu folder`" 打勾不要文件夹，点击 [next] 到第五步。

![在这里插入图片描述](.\Pictures\git环境配置\78469fca81b541c5986913f233740bb5.png)

安装成功后在开始菜单如图所示

![在这里插入图片描述](.\Pictures\git环境配置\watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L211a2Vz,size_16,color_FFFFFF,t_70.png)

### Step5.选择 Git 默认编辑器

Git 安装程序里面内置了 10 种编辑器供你挑选，比如 `Atom`、`Notepad`、`Notepad++`、`Sublime Text`、`Visual Studio Code`、`Vim` 等等，默认的是 Vim ，选择 Vim 后可以直接进行到下一步，但是 Vim 是纯命令行，操作有点难度，需要学习。如果选其他编辑器，则还需要去其官网安装后才能进行下一步。
下图为默认编辑器 `Vim`.我们常用的编辑器是`Visual Studio Code`，所以需要改成`Visual Studio Code`后再点击 [next] 到第六步。
![在这里插入图片描述](.\Pictures\git环境配置\0518f6fa41f6451b864c2c7a0c4c0f17.png)

### Step6.决定初始化新项目(仓库)的主干名字

第一种是让 Git 自己选择，名字是 `master` ，但是未来也有可能会改为其他名字；第二种是我们自行决定，默认是 `main`，当然，你也可以改为其他的名字。一般默认第一种，点击 [next] 到第七步。

```
注： 第二个选项下面有个 NEW！ ，说很多团队已经重命名他们的默认主干名为 main . 这是因为2020 年非裔男子乔治·弗洛伊德因白人警察暴力执法惨死而掀起的 Black Lives Matter(黑人的命也是命)运动，很多人认为 master 不尊重黑人，呼吁改为 main.
```

![在这里插入图片描述](.\Pictures\git环境配置\5044ca0a1c1a4534a274b0d2df580ec3.png)

### Step7.调整你的 path 环境变量

![在这里插入图片描述](.\Pictures\git环境配置\bf76981c79714e2586af65fbd48eed01.png)

译文如下：

```
Use Git from Git Bash only  This is the most cautious choice as your PATH will not be modified at all. You w only be able to use the Git command line tools from Git Bash. 

仅从 Git Bash 使用 Git 这是最谨慎的选择，因为您的 PATH 根本不会被修改。您将只能使用 Git Bash 中的 Git 命令行工具。

Git from the command line and also from 3rd-party software (Recommended) This option adds only some minimal Git wrappers to your PATH to avoid cluttering your environment with optional Unix tools. You will be able to use Git from Git Bash, the Command Prompt and the Windov PowerShell as well as any third-party software looking for Git in PATH. 

从命令行以及第三方软件进行 Git （推荐）此选项仅将一些最小的 Git 包装器添加到PATH中，以避免使用可选的 Unix 工具使环境混乱。 您将能够使用 Git Bash 中的 Git，命令提示符和 Windov PowerShell 以及在 PATH 中寻找 Git 的任何第三方软件。

Use Git and optional Unix tools from the Command Prompt  Both Git and the optional Unix tools will be added to your PATH. Warning: This will override Windows tools like "find"and "sort". Only use this option if you understand the implications.

使用命令提示符中的 Git 和可选的 Unix 工具 Git 和可选的 Unix 工具都将添加到您的 PATH 中。 警告：这将覆盖 Windows 工具，例如 "find" and "sort". 仅在了解其含义后使用此选项。 
```

第一种是`仅从 Git Bash 使用 Git`。这个的意思就是你只能通过 Git 安装后的 Git Bash 来使用 Git ，其他的什么命令提示符啊等第三方软件都不行。

第二种是`从命令行以及第三方软件进行 Git`。这个就是在第一种基础上进行第三方支持，你将能够从 `Git Bash`，`命令提示符(cmd)` 和 `Windows PowerShell` 以及`可以从 Windows 系统环境变量中寻找 Git `的任何第三方软件中使用 Git。推荐使用这个。

第三种是`从命令提示符使用 Git 和可选的 Unix 工具`。选择这种将覆盖 Windows 工具，如 “ find 和 sort ”。只有在了解其含义后才使用此选项。一句话，适合比较懂的人折腾。

### Step8.选择 SSH 执行文件

![在这里插入图片描述](.\Pictures\git环境配置\a5128524791d4415b2ee612f6357e90d.png)

翻译如下：

```
Use bundled OpenSSH 
This uses ssh. exe that comes with Git.
使用捆绑的 OpenSSH
这使用的 ssh.exe 是 Git 自带的 


Use (Tortoise) Plink 
To use PuTTY, specify the path to an existing copy of (Tortoise) Plink.exe
Set ssh. variant for Tortoise Plink 
使用 TortoisePlink (注，这是一个软件)
要使用 PuTTY，请指定 TortoisePlink.exe 的现有副本的路径
为 TortoisePlink 设置 ssh.variant

Use external OpenSSH 
NEW! This uses an external ssh. exe. Git will not install its own OpenSSH
(and related) binaries but use them as found on the PATH.
使用外部 OpenSSH
新！这使用外部 ssh.exe 文件。 
Git 不会安装自己的 OpenSSH（和相关）二进制文件，而是使用在环境变量 PATH 中找到的它们。
```

### Step9.选择HTTPS后端传输

![在这里插入图片描述](.\Pictures\git环境配置\aee056db19ca4412acb5885e3aca9fdb.png)

翻译如下：

```
use the OpenSSL library 
Server certificates will be validated using the ca-bundle. crt file.
使用 OpenSSL 库
服务器证书将使用 ca-bundle.crt 文件进行验证。
	
Use the native Windows Secure Channel library 
Server certificates will be validated using Windows Certificate Stores.
This option also allows you to use your company's internal Root CA certificates distributed e.g. via Active Directory Domain Services.
使用本机 Windows 安全通道库
服务器证书将使用 Windows 证书存储进行验证。
此选项还允许您使用公司内部分发的内部根 CA 证书，例如通过 Active Directory 域服务。
```

这两种选项有什么区别呢？

来自https://stackoverflow.com/questions/62456484/whats-the-difference-between-openssl-and-the-native-windows-secure-channel-libr

> 如果在具有企业管理证书的组织中使用 Git，则将需要使用安全通道。如果你仅使用 Git 来访问公共存储库（例如 GitHub ），或者你的组织不管理自己的证书，那么使用 SSL 后端（它们只是同一协议的不同实现）就可以了。

也就是说，作为普通用户，只是用 Git 来访问 Github、GitLab 等网站，选择前者就行了。点击 [next] 到第十步。

### Step10.配置行尾符号转换

![在这里插入图片描述](.\Pictures\git环境配置\0f1968ce5c6a46539ef0dfa324b7ec4c.png)

```
Checkout Windows-style, commit Unix-style line endings 
Git will convert LF to CRLF when checking out text files. 
When committing text files, CRLF will be converted to LF. For cross-platform projects, this is the recommended setting on Windows("core. autocrif"is set to "true").
签出 Windows 样式，提交 Unix 样式的行结尾
Git 签出文本文件时，会将 LF 转换为 CRLF。
提交文本文件时，CRLF 将转换为 LF。
对于跨平台项目，这是 Windows 上的建议设置（"core.autocrif" 设置为 "true"）。

Checkout as-is, commit Unix-style line endings 
Git will not perform any conversion when checking out text files. 
When committing text files, CRLF will be converted to LF. For cross-platform projects, this is the recommended setting on Unix("core.autocrif" is set to "input").
按原样签出，提交 Unix 样式的行结尾
Git 在签出文本文件时不会执行任何转换。提交文本文件时，CRLF 将转换为 LF。
对于跨平台项目，这是在 Unix 上的建议设置（"core.autocrif" 设置为 "input"）。

Checkout as-is, commit as-is 
Git will not perform any conversions when checking out or committing text files. 
Choosing this option is not recommended for cross-platform projects("core. autocrif"is set to "false").
按原样签出，按原样提交
Git 在签出或提交文本文件时不会执行任何转换。
不建议跨平台项目选择此选项（"core.autocrif" 设置为 "false"）
```

这三种选择分别是：
`签出 Windows 样式，提交 Unix 样式的行结尾。`
`按原样签出，提交Unix样式的行结尾。`
`按原样签出，按原样提交。`


那 Windows 样式和 Unix 样式到底有什么区别呢？

引用 《[GitHub入门与实践](https://book.douban.com/subject/26462816/)》 第 50 页内容[2](https://blog.csdn.net/mukes/article/details/115693833?ops_request_misc=%7B%22request%5Fid%22%3A%22169405414316800180640748%22%2C%22scm%22%3A%2220140713.130102334..%22%7D&request_id=169405414316800180640748&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-115693833-null-null.142^v93^control&utm_term=安装Git&spm=1018.2226.3001.4187#fn2)

> GitHub 中公开的代码大部分都是以 Mac 或 Linux 中的 LF（Line Feed）换行。然而，由于 Windows 中是以 CRLF（Carriage Return+ Line Feed）换行的，所以在非对应的编辑器中将不能正常显示。
>  
> Git 可以通过设置自动转换这些换行符。使用 Windows 环境的各位，请选择推荐的 “Checkout Windows-style，commit Unix-style line endings” 选项。换行符在签出时会自动转换为 CRLF，在提交时则会自动转换为 LF .

上面说 Mac 、Linux、Unix 的 Line Feed ，翻译过来就是换行符，用 “\n” 表示，换行符 “\n” 的 ASCII 值为10；
Windows 的是 Carriage Return+ Line Feed（回车+换行），用 “\r\n” 表示，回车符 “\r” 的 ASCII 值为13；

这上下两者是不一样的。
所以这就需要转换了，至于为什么选第一项？
这还用问吗？`我们现在的教程就是介绍怎么安装 Windows 版 Git，肯定选第一项啦。`

至于 “回车”（carriage return）和 “换行”（line feed）这两个概念的来历和区别？
引用一下 [阮一峰老师博客的部分内容](http://www.ruanyifeng.com/blog/2006/04/post_213.html)

> 在计算机还没有出现之前，有一种叫做电传打字机（Teletype Model 33）的玩意，每秒钟可以打10个字符。但是它有一个问题，就是打字机打完一行换行的时候，要用去0.2秒，正好可以打两个字符。要是在这0.2秒里面，又有新的字符传过来，那么这个字符将丢失。
> 于是，研制人员想了个办法解决这个问题，就是在每行后面加两个表示结束的字符。一个叫做"回车"，告诉打字机把打印头定位在左边界；另一个叫做"换行"，告诉打字机把纸向下移一行。

更多资料参考：

1. [腾讯云 - 换行符 ‘\n’ 和 回车符 ‘\r’ 的区别？](https://cloud.tencent.com/developer/article/1353286)
2. [知乎 - 为什么会用\r\n两个字符表示换行？](https://www.zhihu.com/question/29326647)
3. [Stackoverflow - What are carriage return, linefeed, and form feed?](https://stackoverflow.com/questions/3091524/what-are-carriage-return-linefeed-and-form-feed)

点击 [next] 到第十一步。

### Step11.配置终端模拟器以与 Git Bash 一起使用

![在这里插入图片描述](.\Pictures\git环境配置\01177039343f447ab1c873d59269db94.png)

```
Use MinTTY(the default terminal of MSYS2) 
Git Bash will use MinTTY as terminal emulator, which sports a resizable window
non-rectangular selections and a Unicode font.Windows console programs(such
as interactive Python) must be launched via 'winpty' to work in MinTTY.
使用 MinTTY（MSYS2的默认终端）
Git Bash 将使用 MinTTY 作为终端仿真器，该仿真器具有可调整大小的窗口非矩形选择和 Unicode 字体。
Windows 控制台程序（例如交互式 Python）必须通过 "winpty" 启动才能在 MinTTY 中运行。

Use Windows' default console 
window Git will use the default console window of Windows("cmd.exe"), which works v
with Win32 console programs such as interactive Python or node. js, but has a
very limited default scroll-back,needs to be configured to use a Unicode font in 
order to display non-ASCII characters correctly, and prior to Windows 10 its 
window was not freely resizable and it only allowed rectangular text selections.<br>
使用 Windows 的默认控制台窗口
Git 将使用 Windows 的默认控制台窗口（"cmd.exe"），该窗口可与 Win32 控制台程序（例如交互式Python 或 
node.js）一起使用，但默认回滚非常有限，需要将其配置为使用 Unicode 字体才能正确显示非 ASCII 字符，并且在 
Windows 10 之前，其窗口不可随意调整大小，并且仅允许选择矩形文本。
```

建议选择第一种，MinTTY [3](https://blog.csdn.net/mukes/article/details/115693833?ops_request_misc=%7B%22request%5Fid%22%3A%22169405414316800180640748%22%2C%22scm%22%3A%2220140713.130102334..%22%7D&request_id=169405414316800180640748&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-115693833-null-null.142^v93^control&utm_term=安装Git&spm=1018.2226.3001.4187#fn3)功能比 cmd 多，cmd 只不过 比 MinTTY 更适合处理 Windows 的一些接口问题，这个对 Git 用处不大，除此之外 Windows 的默认控制台窗口（`cmd`）有很多劣势，比如 cmd 具有非常有限的默认历史记录回滚堆栈和糟糕的字体编码等等。
相比之下，MinTTY 具有可调整大小的窗口和其他有用的可配置选项，可以通过右键单击的工具栏来打开它们 git-bash 。点击 [next] 到第十二步。

### Step12.选择默认的 “git pull” 行为

![在这里插入图片描述](.\Pictures\git环境配置\0d65965260c74054983f04c8fb330b49.png)

```“git pull” 是什么意思呢？
ODefault(fast-forward or merge)
This is the standard behavior ofgit pull": fast-forward the current branch to 
the fetched branch when possible, otherwise create a merge commit.
默认（快进或合并）
这是 "git pull" 的标准行为：在可能的情况下将 当前分支 快进到 获取的分支，否则创建合并提交。

ORebase Rebase the current branch onto the fetched branch. If there are no local 
commits to rebase, this is equivalent to a fast-forward.
变基将当前分支变基到获取的分支上。如果没有本地提交要变基，则等同于快进。

Oonly ever fast-forward 
Fast-forward to the fetched branch. Fail if that is not possible.
只能快进快进到获取的分支。如果不可能，则失败。
```

“git pull” 是什么意思呢？
git pull 就是获取最新的远程仓库分支到本地，并与本地分支合并

上面给了三个 “git pull” 的行为：
第一个是 `merge`
第二个是 `rebase`
第三个是 `直接获取`

第一种 `git pull = git fetch + git merge`
第二种 `git pull = git fetch + git rebase`
第三种 `git pull = git fetch` ？(这个没试过，纯属猜测

一般默认选择第一项，`git rebase` 绝大部分程序员都用不好或者不懂，而且风险很大，但是很多会用的人也很推崇，但是用不好就是灾难。

git pull 只是拉取远程分支并与本地分支合并，而 git fetch 只是拉取远程分支，怎么合并，选择 merge 还是 rebase ，可以再做选择。

更多参考资料：

1. [知乎 - git pull 和 git fetch的区别？](https://www.zhihu.com/question/38305012) 
2. [知乎 - 在开发过程中使用 git rebase 还是 git merge，优缺点分别是什么？](https://www.zhihu.com/question/36509119) 
3. [Stackoverflow - Why does git perform fast-forward merges by default?](https://stackoverflow.com/questions/2850369) 
4. [Stackoverflow - In git how is fetch different than pull and how is merge different than rebase?](https://stackoverflow.com/questions/14894768/) 
5. [Stackoverflow - Difference between git pull and git pull --rebase](https://stackoverflow.com/questions/18930527)

### Step13.选择一个凭证帮助程序

![在这里插入图片描述](.\Pictures\git环境配置\fe3c254258b84f9a84c0ed40d7a1602d.png)

翻译如下：

```
Git Credential Manager 
Use the cross-platform Git Credential Manager.
See more information about the future of Git Credential Manager here.
Git 凭证管理
使用跨平台的 Git  凭证管理。
在此处查看有关 Git 凭证管理未来的更多信息。

None 
Do not use a credential helper.
不使用凭证助手。
```

一共两个选项：
`Git 凭证管理`
`不使用凭证助手`

第一个选项是提供`登录凭证`帮助的，Git 有时需要用户的凭据才能执行操作；例如，可能需要输入`用户名`和`密码`才能通过 HTTP 访问远程存储库（GitHub，GItLab 等等）。

登录图如下(属于第一个选项的，老图了)，来自https://segmentfault.com/q/1010000011171685
![在这里插入图片描述](.\Pictures\git环境配置\watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L211a2Vz,size_16,color_FFFFFF,t_70-169405553481018.png)

更多参考资料：

1. [Stackoverflow - Is Control Panel’s Credential Manager same as git’s credential helpers Credential Manager and Credential manager Core?](https://stackoverflow.com/questions/66795897) 
2. [GitHub - Git Credential Manager Core](https://github.com/microsoft/Git-Credential-Manager-Core) 
3. [GitHub - Git Credential Manager Core Core FAQ](https://github.com/microsoft/Git-Credential-Manager-Core/blob/master/docs/faq.md#about-the-project) 
4. Git官网文档 - [git-scm.com/docs/gitcredentials](https://git-scm.com/docs/gitcredentials)

注：资料链接建议不用看，这个本来三个选项的，不知因何回退成两个选项了，原本的那个新选项被取消了

点击 [next] 进到十四步。

### Step14.配置额外的选项

![在这里插入图片描述](.\Pictures\git环境配置\f9aee091420b4807b961f48a00afd6f8.png)

翻译如下：

```
Enable file system caching 
File system data will be read in bulk and cached in memory for certain operations("core.fscache" is set to "true"). 
This provides a significant performance boost.
启用文件系统缓存
将批量读取文件系统数据并将其缓存在内存中以进行某些操作（"core.fscache” 设置为 "true"）。
这可以显着提高性能。

Enable symbolic links 
Enable symbolic links(requires the SeCreateSymbolicLink permission).
Please note that existing repositories are unaffected by this setting.
启用符号链接
启用符号链接（需要SeCreateSymbolicLink权限）。
请注意，现有存储库不受此设置的影响。
```

有两个选项：
`启用文件系统缓存`
`启用符号链接`

`启用文件系统缓存`就是将批量读取文件系统数据并将其缓存在内存中以进行某些操作，可以显著提升性能。这个选项默认开启。
`启用符号链接` ，符号链接是一类特殊的文件， 其包含有一条以绝对路径或者相对路径的形式指向其它文件或者目录的`引用`，类似于 Windows 的快捷方式，不完全等同 类Unix（如 Linux） 下的 符号链接。因为该功能的支持需要一些[条件](https://github.com/git-for-windows/git/wiki/Symbolic-Links)，所以默认不开启。


更多关于 “符号链接” 参考资料：

1. [GitHub - Symbolic-Links](https://github.com/git-for-windows/git/wiki/Symbolic-Links)
2. [Stackoverflow - How does Git handle symbolic links?](https://stackoverflow.com/questions/954560/) 
3. [Stackoverflow - What is the difference between NTFS Junction Points and Symbolic Links?](https://stackoverflow.com/questions/9042542/) 

点击 [next] 到第十五步。

### Step15.配置实验性选项

![在这里插入图片描述](.\Pictures\git环境配置\88422790b5e3410c92eb2bafef00e7fc.png)

翻译如下：

```
Enable experimental support for pseudo consoles.
(NEW!) This allows running native console programs like Node or Python in a Git Bash window without using winpty, 
but it still has known bugs.
启用对伪控制台的实验性支持。
(新功能!) 这允许在不使用 winpty 的情况下在 Git Bash 窗口中运行诸如 Node 或 Python 之类的本机控制台程序，
但是它仍然存在已知的 bug。

Enable experimental built-in file system monitor
(NEW!) Automatically run a built-in file system watcher, to speed up common operations such as ' git status', ' git add', ' git commit', etc in worktrees containing many files.
启用实验性内置文件系统监视器
（新！）自动运行内置文件系统监视器，以加快包含许多文件的工作树中的常见操作，例如 'git status'、'git add'、'git commit' 等.
1234567891011
```

这是实验性功能，可能会有一些小错误之类的，建议不用开启。
点击 [install] 进行安装即可。

# git的使用

若没有安装git bash需要安装git，参考git bash安装。

由于vscode配置git会特别方便，所以这里配置和使用都是以vscode来演示。

## 如何上传代码到gitee(第一次使用)

如果需要git bash的使用和配置方法详见参考链接：

[如何把项目上传到Gitee（全网最细）_如何把項目上傳到自己的gitee_小胖java攻城狮的博客-CSDN博客](https://blog.csdn.net/qq_45798556/article/details/114632849)

[Gitee上传代码保姆级教程（亲测有效）_沫 北的博客-CSDN博客](https://blog.csdn.net/qq_51618777/article/details/124420589)

### Step1. 新建一个仓库

![image-20230907113300200](.\Pictures\git环境配置\image-20230907113300200.png)

点击创建后会显示如下页面：

![image-20230907113610211](.\Pictures\git环境配置\image-20230907113610211.png)

### Step2. 配置本地git

在本地工程目录打开vscode，点击初始化仓库：

![image-20230907114304048](.\Pictures\git环境配置\image-20230907114304048.png)

点击1打开源代码管理，然后点击初始化仓库

![image-20230907114506715](.\Pictures\git环境配置\image-20230907114506715.png)

随后在1处输入备注，2处点击`+`后会把更改放入暂存区，3处点击提交。

至此，本地git配置完成。

### Step3. 配置远程仓库

随后点击底部栏，终端，+，git bash。

![image-20230919165430824](.\Pictures\git环境配置\image-20230919165430824.png)

输入:

```c
 git config --global user.name "Your Name"
 git config --global user.email "youremail@yourdomain.com"
```

账号为用户名，邮箱为设置-邮箱管理中的上传邮箱，对git上传账号进行配置。

如果提示失败首先检查gitee设置，取消箭头勾选：

![image-20230919165857181](.\Pictures\git环境配置\image-20230919165857181.png)

随后，复制仓库HTTPS连接：

![image-20230919170245103](.\Pictures\git环境配置\image-20230919170245103.png)

打开侧边栏-源代码管理-...-远程-添加远程存储库

![image-20230919170350899](.\Pictures\git环境配置\image-20230919170350899.png)

输入连接

![image-20230919170516687](.\Pictures\git环境配置\image-20230919170516687.png)

输入在该本地的名称 通常为origin

![image-20230919170638943](.\Pictures\git环境配置\image-20230919170638943.png)

单击发布branch即可。

![image-20230919170726350](.\Pictures\git环境配置\image-20230919170726350.png)

## 从远程拉取仓库

该部分是指从云端拉取仓库，打开要拉去的仓库连接，点击克隆/下载，复制https连接。

![image-20230919171231699](.\Pictures\git环境配置\image-20230919171231699.png)

随后打开vscode，打开欢迎界面：

![image-20230919171450884](.\Pictures\git环境配置\image-20230919171450884.png)

点击克隆git仓库

![image-20230919171517083](.\Pictures\git环境配置\image-20230919171517083.png)

将刚刚复制到的连接粘贴进去

![image-20230919171551972](.\Pictures\git环境配置\image-20230919171551972.png)

随后选择保存位置，等待下载完成即可。

## 配置子模块

子模块是git的一个功能，它允许当前工程中包含其他的工程，以实现复用。

子模块配置十分简单，在拉取完远程仓库后输入：![image-20230919165430824](.\Pictures\git环境配置\image-20230919165430824.png)

```
git submodule add -f https连接
```

