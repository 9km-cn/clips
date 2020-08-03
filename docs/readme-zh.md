# Clips

[English Docs](../readme.md) | [中文文档](readme-zh.md)

# 概述

`clips` 是一个基于`C++11`的命令行解析器，可方便的集成到源码工程中。

# 特点

- 提供少量的、简单的接口；
- 方便创建基于子命令和嵌套命令的应用程序；
- 可在单独的文件中定义命令处理函数和绑定命令，使用 `CLIPS_INIT()`；
- 仅有头文件；
- 没有外部依赖；
- 直接绑定变量（`&varname`），或者使用`'cast<typename>'`函数获取`flag`值；
- 支持枚举值；
- 支持简单的自定义类型；
- 清晰的帮助信息；
- 友好的错误信息；
- 支持在`Windows`、`Linux`和`macOS`平台下使用；

# 命令行

示例：

```yaml
$ ./appname
$ ./appname pull --all
$ ./appname clone -v https::clips.repo.git clips
$ ./appname remote rename old_name new_name
$ ./appname remote set-url --delete dev https::clips.repo.git
```

# 帮助

可以查看子命令的详细帮助，参考下面：

```yaml
# 帮助
$ ./appname -h
$ ./appname --help

# 嵌套命令的帮助
$ ./appname [cmds...] -h
$ ./appname [cmds...] --help

# 示例
$ ./appname a b c -h
```

帮助信息，示例：

```yaml
$ ./appname -h

desc

usage:
  appname [cmds...] [args...] [--flags...]

cmds:
  sub  sub brief

flags:
  -h, --help  <bool>          :(false)  help
      --eee   <unsigned int>   (1)      eee desc
  -f, --fff   <int>            (2)      fff desc  {0,1,2}

example:
  appname sub arg0 arg1 --name=value

for more information about a cmd:
  appname [cmds...] -h
  appname [cmds...] --help
```

`flags` 列定义:

```yaml
flags:
  -n, --name  <type>  :extend(default)  desc  {enums}
```

注：`:(false)`中的`:`表示，这个命令是`extend`的，可能是从上级命令中继承过来的。

# 工程

目录结构示例：

```yaml
+ <project_path>
  + src
    - main.cpp
    - cmds_first.cpp
    - cmds_second.cpp
    + <others...>
```

包含目录路径：

```cpp
clips/include
```

在代码中包含文件头：

```cpp
#include "clips/clips.hpp"
```

# 使用

## 主函数

```cpp
#include "clips/clips.hpp"

int main(int argc, char* argv[])
{
    clips::desc("desc");

    auto ret = clips::exec(argc, argv);
    if (clips::ok != ret)
    {
        std::cout << ret << std::endl;
        return 1;
    }
    return 0;
}
```

## 根函数

根函数实际上是内部一个预置的最顶级的根命令，这个命令不具名。

定义根函数:

```cpp
uint32_t g_ddd = 0;
clips::error_t root(const clips::pcmd_t& pcmd, const clips::args_t& args)
{
    std::cout << "exec root handler. ddd=" << ddd << std::endl;
    return clips::ok;
}
```

添加`flag`, 和绑定处理函数:

```cpp

auto ret = clips::flag<uint32_t>("aaa", "a", 2, "aaa desc");
if (ret != clips::ok)
{
    return ret;
}

ret = clips::pflag<uint32_t>(&g_ddd, "ddd", "", 5, "ddd desc", true);
if (ret != clips::ok)
{
    return ret;
}

ret = clips::bind(root);
if (ret != clips::ok)
{
    return ret;
}
```

执行命令：

```yaml
$ ./appname [args...] [--flags...]
```

注：根函数不是必须的，可以为空（默认）。

## 子命令

实际上就是嵌套命令，内部的根函数对应的根命令的嵌套命令。只是因为提供了顶级接口，就和下面的嵌套命令做了区分。

定义和绑定子命令：

```cpp
// 创建
auto sub = clips::make_cmd("sub");
sub->brief("sub 简洁描述");
sub->desc("sub 详细描述");

// 添加 flags
int fff = 0;
sub->pflag<int>(&fff, "fff", "f", 2, { 0, 1, 2 }, "fff desc");
sub->flag<uint32_t>("eee", "", 1u, "eee desc", true);

// 示例
sub->example("sub --eee 0 -f 0");

// 绑定函数
sub->bind([](const clips::pcmd_t& pcmd,
    const clips::args_t& args) -> clips::error_t
    {
        std::cout << "exec sub." << std::endl;
        return clips::ok;
    }
);

// 绑定命令
auto ret = clips::bind(sub);
if (ret != clips::ok)
{
    return ret;
}
```

执行命令：

```yaml
$ ./appname sub [args...] [--flags...]
```

## 嵌套命令

嵌套命令是指命令中包含命令。

当需要在 `sub` 命令下包含子命令 `nested`, 则需要如下方法实现：

```cpp
auto nested = clips::make_cmd("nested");
nested->brief("nested 简洁描述");
nested->desc("nested 详细描述");
// ...
auto ret = sub->bind(nested);
if (ret != clips::ok)
{
    return ret;
}
```

执行命令：

```yaml
$ ./appname sub nested [args...] [--flags...]
```

## 在单独文件中定义和绑定命令

当包含子命令和`flag`很多时，在 `main.cpp` 实现所有交互逻辑，会显得臃肿不清晰。将各命令在单独的逻辑中实现，既条理清晰，又增加了程序的可测试性。

```cpp
auto your_func(const clips::pcmd_t& pcmd, const clips::args_t& args) -> clips::error_t
{
    // ...
    return clips::ok;
}

CLIPS_INIT()
{
    // 这个函数将会在解析命令行参数之前被调用。

    auto pcmd = clips::make_cmd("name");
    // ...
    pcmd->bind(your_func); // 也可以使用lambda函数
    return clips::bind(pcmd);
}
```

`CLIPS_INIT()`定义的逻辑会在 `clips::exec()` 的一开始执行。当在多个文件中定义了 `CLIPS_INIT()` 时，其执行顺序和编译器有关，通常是文件名称的字典序。其中，根函数的绑定并不受该顺序的影响，可在任何位置定义，而其他命令因为是具名的，所以会受先后绑定的影响，后绑定的同名命令会导致函数返回重复定义的错误信息。

# 应用信息

## 应用名称

如果指定了应用名称，则在`clips::exec()`中解析时不会被重写；如果不指定应用名称，则在`clips::exec()`中解析时会使用`argv[0]`代替；

```cpp
clips::name("name");
auto name = clips::name();
```

## 应用描述

```cpp
clips::desc("desc");
auto desc = clips::desc();
```

## 原始命令参数

```cpp
auto& argv = clips::argv();
```

# `Flag`

`flag`一般只能通过命令接口添加。

## 添加

```cpp
// 需要用 cast() 转换
auto err = pcmd->flag<int>("name", "n", 0, "desc");
auto value = flags["--name"]->cast<int>();
// 或
auto value = pcmd->cast<int>("--name");

// 绑定变量
int varname = 0; // 也可用 cast() 转换
auto err = pcmd->pflag<int>(&varname, "name", "n", 0, "desc");
```

## 名称

`flag` 的名称为 `"name"` 时，对应的命令行是 `--name`。名称不能为空。

## 快捷名称

`flag` 的快捷名称为 `"n"` 时，对应的命令行是 `-n`。快捷名称可以为空或一个字符。

## 类型名称

```cpp
auto type_name = flags["--name"]->type_name();
```

## 继承

当设置 `flag` 可继承时，其分支之后的嵌套命令都可以访问。

`flag` 或 `pflag` 函数的最后一个参数为 `true` 时，表示可继承，默认是不继承（不给出对应参数时），如下：

```cpp
auto err = pcmd->flag<int>("name", "n", 0, "desc", true);
```

内部解析`flag`的时候，其查找顺序是先在当前命令中查找这个`flag`，找不到时，再从根命令自顶向下查找，排除非`extend`的`flag`，直到找到或到当前命令为止。

## 可选项（枚举值）

当提供可选项（枚举值）时，会检查输入参数是否为枚举值之一。如果不合法，则`clips::exec()`会返回错误。

```cpp
auto err = pcmd->flag<int>("name", "n", 0, {0, 1, 2}, "desc");
auto err = pcmd->pflag<int>(&varname, "name", "n", 0, {0, 1, 2}, "desc");
```

## 转换

```cpp
// 是否可以转换为目标类型
if (flags["--name"]->castable<std::string>())
{
    // 可以转换
}

// 转换为目标类型值
auto value = flags["--name"]->cast<std::string>();
// 或
auto value = pcmd->cast<std::string>("--name");

// 不能转换为目标类型时，会抛出异常
try
{
    auto value = flags["--name"]->cast<std::string>();
    // 或
    auto value = pcmd->cast<std::string>("--abc");
}
catch (std::exception& e)
{
    std::cout << e.what() << std::endl;
}
```

使用 `error_t`:

```cpp
error_t err;
auto value = pcmd->cast<std::string>("--abc", &err);
if (err != clips::ok)
{
    return err;
}
```

## 堆栈信息

您可能需要堆栈信息(也可从当前命令中获取)，以在出错时帮助定位。

```cpp
auto stack = flags["--name"]->stack();
```

## 默认值字符串

可查看默认值对应的字符串。

```cpp
auto stack = flags["--name"]->default_value();
```

## 输入的字符串值

在`clips::exec()`中的执行解析逻辑后，才可以得到用户输入的字符串值，否则为空。

```cpp
auto text = flags["--name"]->text();
```

## 输入形式

```yaml
--name         # bool 型
-n             # bool 型
--name true    # bool 型
-n false       # bool 型
--name 1       # bool 型
-n 0           # bool 型
--name value   # 空格区隔名称和值
-n value       # 空格区隔名称和值
--name=value   # 等号区隔名称和值
-n=value       # 等号区隔名称和值
-n '-1'        # 单引号防止命令行解析规则导致的转义
```

## 自定义类型

需要实现流处理操作符 `<<` 和 `>>` 的重载.

定义:

```cpp
class custom_t
{
public:
    custom_t()
    {
    }

    ~custom_t()
    {
    }

    custom_t(const custom_t& cpy)
        : num_(cpy.num_)
        , msg_(cpy.msg_)
    {
    }

    custom_t(custom_t&& mv) noexcept
        : num_(mv.num_)
        , msg_(std::move(mv.msg_))
    {
    }

    custom_t& operator=(const custom_t& rhs)
    {
        num_ = rhs.num_;
        msg_ = rhs.msg_;
        return *this;
    }

    int num_{ 0 };
    std::string msg_;
};

std::ostream& operator<<(std::ostream& os, const custom_t& obj)
{
    os << "{" << obj.num_ << "," << obj.msg_ << "}";
    return os;
}

std::istream& operator>>(std::istream& is, custom_t& obj)
{
    unsigned char c;
    std::string tmp;
    is >> obj.num_ >> c >> obj.msg_;
    return is;
}
```

Flag:

```cpp
err = pcmd->flag<custom_t>("custom", "", {}, "custom_t type");
```

转换:

```cpp
std::cout << pcmd->cast<custom_t>("--custom") << std::endl;
```

执行:

```yaml
$ ./appname sub --custom=1,msg
```

# 命令

## 创建命令

建议的三种创建方式：

```cpp
// 不带参数
auto pcmd = clips::make_cmd();

// 给定名称
auto pcmd = clips::make_cmd("name");
```

## 基本信息

```cpp
/// name 名称
pcmd->name("name");
auto name = pcmd->name();

/// brief 简短描述
pcmd->brief("brief");
auto brief = pcmd->brief();

/// desc 完整描述
pcmd->desc("desc");
auto desc = pcmd->desc();
```

## 提供示例

示例只是一段文本，通常需要写如何执行这个命令。

```cpp
pcmd->example("example");
auto example = pcmd->example();
```

## 添加`flag`

```cpp
// 需要用 cast() 转换
auto err = pcmd->flag<int>("name", "n", 0, "desc");
auto value = flags["--name"]->cast<int>();
auto value = pcmd->cast<int>("--name");

// 绑定变量
int varname = 0; // 也可用 cast() 转换
auto err = pcmd->pflag<int>(&varname, "name", "n", 0, "desc");
```

注意变量的生命周期。

## 绑定函数

```cpp
auto your_func(const clips::pcmd_t& cmd, const clips::args_t& args) -> clips::error_t
{
    // ...
    return clips::ok;
}
auto err = pcmd->bind(your_func); // 或者使用lambda函数
```

注：命令函数不是必须的，可以为空（默认）。

## 嵌套命令

```cpp
auto psub = clips::make_cmd("sub");
// ...
auto err = pcmd->bind(psub);
```

## 命令参数

定义如下：

```yaml
$ ./appname sub nested [args...] [--flags...]
```

示例如下：

```yaml
$ ./appname sub adfa # ok, adfa 为参数 args[0]
$ ./appname sub      # ok, args 大小为 0
$ ./appname sub nested adfa # ok, args[0] 为 adfa
$ ./appname sub nested      # ok, args 大小为 0
```

# 错误信息

## 创建

```cpp
auto err = clips::make_error();
auto err = clips::make_error("msg");
auto err = clips::make_error("msg", "stack");
```

## 接口

错误信息：

```cpp
err.msg("parse failed.");
auto msg = err.msg();
```

堆栈信息：

```cpp
err.stack("appname sub");
auto stack = err.stack();
```

流式打印：

```cpp
std::cout << err << std::endl;
```

## 比较

只关注错误信息是否相同，不关心堆栈信息。

- `err == clips::ok` 成功；
- `err != clips::ok` 失败；

# 不支持

- 非标准`flag`, 如 `-long` 或 `--h`；

# TODO

- 帮助信息 `help`
    - 文本定制
    - 多语言国际化 `i18n --lang {default, en-us, zh-cn}`
- 类型安全 `type safety`
    - 更多类型检查 `type checking`
- 兼容 `POSIX flags`
    - 组合`flag` 支持, 如 `-abc`
- 智能推荐

# 测试

使用 `Catch2` 框架对 `clips` 进行单元测试, 推荐前往下面的网址，了解这个优秀的单元测试框架:

[https://github.com/catchorg/Catch2](https://github.com/catchorg/Catch2)