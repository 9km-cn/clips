# Clips

[English Docs](readme.md) | [中文文档](docs/readme-zh.md)

# Overview

`clips` is a command line parser for modern C++, easy inclusion in project.

# Features

- provides a few and simple interfaces;
- esay to create subcommand-based and subcommand-nested CLIs application;
- command handlers can be defined and binded in separate files, using `CLIPS_INIT()`;
- header only;
- no external dependencies;
- directly to bind variables (`&varname`), or using `'cast<typename>'` to get result of flag;
- support options of flag;
- support simple custom type;
- clear help messages;
- friendly error messages;
- work on Windows, Linux, and macOS.

# Command Line

Examples:

```yaml
$ ./appname
$ ./appname pull --all
$ ./appname clone -v https::clips.repo.git clips
$ ./appname remote rename old_name new_name
$ ./appname remote set-url --delete dev https::clips.repo.git
```

# Help

Run like this:

```yaml
# help
$ ./appname -h
$ ./appname --help

# nested command help
$ ./appname [cmds...] -h
$ ./appname [cmds...] --help

# example
$ ./appname a b c -h
```

Help message, example:

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

Flags column definition:

```yaml
flags:
  -n, --name  <type>  :extend(default)  desc  {enums}
```

Note: in `:(false)`, `:` represents extend, may be extend from top command.

# Project

Directory structure example:

```yaml
+ <project_path>
  + src
    - main.cpp
    - cmds_first.cpp
    - cmds_second.cpp
    + <others...>
```

Include path in your project: 

```cpp
clips/include
```

And include header in your code:

```cpp
#include "clips/clips.hpp"
```

# Using

## main

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

## root

In fact, the root function is an internal preset top-level root command that is not named.

Define your root function:

```cpp
uint32_t g_ddd = 0;
clips::error_t root(const clips::pcmd_t& pcmd, const clips::args_t& args)
{
    std::cout << "exec root handler. ddd=" << ddd << std::endl;
    return clips::ok;
}
```

Add flag, and bind handler:

```cpp

auto err = clips::flag<uint32_t>("aaa", "a", 2, "aaa desc");
if (err != clips::ok)
{
    return err;
}

err = clips::pflag<uint32_t>(&g_ddd, "ddd", "", 5, "ddd desc", true);
if (err != clips::ok)
{
    return err;
}

err = clips::bind(root);
if (err != clips::ok)
{
    return err;
}
```

Run like this：

```yaml
$ ./appname [args...] [--flags...]
```

Note: the root handler is not required and can be null (default).

## subcommand

It's really just a nested command, a nested command of the internal root command, distinguished from the following because it provides a top-level interface.

Defined:

```cpp
// create
auto sub = clips::make_cmd("sub");
sub->brief("sub brief");
sub->desc("sub desc");

// flags
int fff = 0;
sub->pflag<int>(&fff, "fff", "f", 2, { 0, 1, 2 }, "fff desc");
sub->flag<uint32_t>("eee", "", 1u, "eee desc", true);

// example
sub->example("sub --eee 0 -f 0");

// bind handler
sub->bind([](const clips::pcmd_t& pcmd,
    const clips::args_t& args) -> clips::error_t
    {
        std::cout << "exec sub" << std::endl;
        return clips::ok;
    }
);

// bind cmd
auto ret = clips::bind(sub);
if (ret != clips::ok)
{
    return ret;
}
```

Run like this：

```yaml
$ ./appname sub [args...] [--flags...]
```

## nested subcommand

A nested command is one that contains commands.

Defined:

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

Run like this：

```yaml
$ ./appname sub nested [args...] [--flags...]
```

## defined and binded command in separate file

You don't need to implement all the commands in main.cpp, you can implement them separately. This is both clear and testable.

```cpp
auto your_func(const clips::pcmd_t& pcmd, const clips::args_t& args) -> clips::error_t
{
    // ...
    return clips::ok;
}

CLIPS_INIT()
{
    // this function will be calling before parsing command line.

    auto pcmd = clips::make_cmd("name");
    // ...
    pcmd->bind(your_func); // also using lambda
    return clips::bind(pcmd);
}
```

'CLIPS_INIT()' is executed at the beginning of 'clips::exec()'. When 'CLIPS_INIT()' is defined in multiple files, the order of execution is compiler-dependent, usually in lexicographic order of the file name. The binding of the root function is not affected by the order and can be defined anywhere. Because the other commands are named, they are affected, and the postbound command with the same name causes the function to return an error message that is repeatedly defined.

# Application Info

## Name

If the application name is specified, it will not be overridden when parsing in clips::exec(), otherwise it will be replaced by argv[0] when  parsing in clips::exec().

```cpp
clips::name("name");
auto name = clips::name();
```

## Description

```cpp
clips::desc("desc");
auto desc = clips::desc();
```

## Origin argv

It's a std::vector<std::string> from argv[].

```cpp
auto& argv = clips::argv();
```

# Flag

You can only add flag by command interfaces.

## Add

```cpp
// need cast()
auto err = pcmd->flag<int>("name", "n", 0, "desc");
auto value = flags["--name"]->cast<int>();
// or
auto value = pcmd->cast<int>("--name");

// bind variables
int varname = 0; // also using cast()
auto err = pcmd->pflag<int>(&varname, "name", "n", 0, "desc");
```

## Name

When the name of `flag` is `"name"`, the corresponding command line is `--name`. The name cannot be empty.

## Fast Name

When the fast name of `flag` is `"n"`, the corresponding command line is `-n`. The fast name can be empty or one character.

## Type Name

```cpp
auto type_name = flags["--name"]->type_name();
```

## Extend

When `flag` is `extend`, all nested commands following its branches are accessible.

When the last parameter of `flag()` or `pflag()` function is `true`, it means extend. The default is not extend (when the last parameter are not given), as follows:

```cpp
auto err = pcmd->flag<int>("name", "n", 0, "desc", true);
```

The search rule is to find the `flag` in the current command first, When it cannot be found, look down from the root command and exclude non-extend `flag`, until it is found before the current command.

## Options

When options (enumeration values) is provided, the input parameter is checked to see if it is one of the options (enumeration values). If not, the call `clips::exec()` returns an error.

```cpp
auto err = pcmd->flag<int>("name", "n", 0, {0, 1, 2}, "desc");
auto err = pcmd->pflag<int>(&varname, "name", "n", 0, {0, 1, 2}, "desc");
```

## Cast

Whether it can be converted to the target type:

```cpp
if (flags["--name"]->castable<int>())
{
    // can be converted
}
```

Converted:

```cpp
auto value = flags["--name"]->cast<std::string>();
// or
auto value = pcmd->cast<std::string>("--name");
```

An exception is thrown when the target type cannot be converted:

```cpp
try
{
    auto value = flags["--name"]->cast<std::string>();
    // or
    auto value = pcmd->cast<std::string>("--abc");
}
catch (std::exception& e)
{
    std::cout << e.what() << std::endl;
}
```

Using error_t:

```cpp
error_t err;
auto value = pcmd->cast<std::string>("--abc", &err);
if (err != clips::ok)
{
    return err;
}
```

## Stack

You may need stack information (also available from the current command) to help locate an error.

```cpp
auto stack = flags["--name"]->stack();
```

## Default Value

```cpp
auto stack = flags["--name"]->default_value();
```

## Input Text

This is available only after the parsing logic in 'clips::exec()' is executed.

```cpp
auto text = flags["--name"]->text();
```

## Input Form

```yaml
--name         # bool
-n             # bool
--name true    # bool
-n false       # bool
--name 1       # bool
-n 0           # bool
--name value   # Space separates the name and value
-n value       # Space separates the name and value
--name=value   # The equals sign separates the name and the value
-n=value       # The equals sign separates the name and the value
-n '-1'        # Single quotes prevent escape from command line parsing rules
```

## custom type

Need to implement stream operator `<<` and `>>` overloading.

Defined:

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

Cast:

```cpp
std::cout << pcmd->cast<custom_t>("--custom") << std::endl;
```

Run like this:

```yaml
$ ./appname sub --custom=1,msg
```

# Command

## Create

Three suggested ways to create:

```cpp
// without arguments
auto pcmd = clips::make_cmd();

// given name
auto pcmd = clips::make_cmd("name");
```

## Base Info

```cpp
pcmd->name("name");
auto name = pcmd->name();

pcmd->brief("brief");
auto brief = pcmd->brief();

pcmd->desc("desc");
auto desc = pcmd->desc();
```

## Example

The example is just a piece of text, and you usually need to write how to execute this command.

```cpp
pcmd->example("example");
auto example = pcmd->example();
```

## Add Flag

```cpp
// need cast()
auto err = pcmd->flag<int>("name", "n", 0, "desc");
auto value = flags["--name"]->cast<int>();
auto value = pcmd->cast<int>("--name");

// bind variables
int varname = 0; // also using cast()
auto err = pcmd->pflag<int>(&varname, "name", "n", 0, "desc");
```

Notice the life cycle of the variable.

## Bind Handler

```cpp
auto your_func(const clips::pcmd_t& cmd, const clips::args_t& args) -> clips::error_t
{
    // ...
    return clips::ok;
}
auto err = pcmd->bind(your_func); // also using lambda
```

Note: the command handler is not required and can be null (default).

## Nested Command

```cpp
auto psub = clips::make_cmd("sub");
// ...
auto err = pcmd->bind(psub);
```

## Arguments

Defined:

```yaml
$ ./appname sub nested [args...] [--flags...]
```

As follows:

```yaml
$ ./appname sub adfa # ok, adfa is args[0]
$ ./appname sub      # ok, args size is 0
$ ./appname sub nested adfa # ok, args[0] is adfa
$ ./appname sub nested      # ok, args size is 0
```

# Error Message

## Create

```cpp
auto err = clips::make_error();
auto err = clips::make_error("msg");
auto err = clips::make_error("msg", "stack");
```

## Interfaces

Msg:

```cpp
err.msg("parse failed.");
auto msg = err.msg();
```

Stack:

```cpp
err.stack("appname sub");
auto stack = err.stack();
```

Prints:

```cpp
std::cout << err << std::endl;
```

## Comparison

The comparison only focuses on whether the error message is the same, not the stack message.

- `err == clips::ok` successful;
- `err != clips::ok` failed;

# Nonsupport

- non-standard flag, like -long or --h;

# TODO

- help
    - custom text;
    - i18n --lang { default, en-us, zh-cn};
- type safety
    - type checking;
- compliant POSIX flags
    - combination flag, like -abc;
- intelligent suggestions

# TEST

Test clips with `Catch2`, recommend going to the following url to learn about this excellent unit testing framework:

[https://github.com/catchorg/Catch2](https://github.com/catchorg/Catch2)