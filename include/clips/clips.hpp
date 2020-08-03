// Copyright (c) 2020, garry.hou <garry@esdk.net>, All rights reserved.
// 
// Clips is licensed under the Mulan PSL v1.
// You can use this software according to the termsand conditions of the Mulan PSL v1.
// You may obtain a copy of Mulan PSL v1 at :
//     http://license.coscl.org.cn/MulanPSL
// 
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, 
// EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON - INFRINGEMENT, 
// MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
// 
// See the Mulan PSL v1 for more details.
// ----------------------------------------------------------------------------
// 
// api list:
// clips::name() 应用名称
// clips::desc() 应用描述
// clips::make_error() 创建错误信息
// clips::make_cmd() 创建命令
// clips::flag() 添加flag
// clips::pflag() 添加flag，绑定外部变量
// clips::bind() 绑定根函数，绑定子命令等
// clips::exec() 解析，执行
// clips::argv() 原始命令参数
// 
// clips_cmd.cpp:
// static uint32_t g_ccc = 0;
// clips::error_t your_func(const pcmd_t& pcmd, const args_t& args)
// {
//     处理
//     return clips::ok;
// }
// CLIPS_INIT()
// {
//     auto sub = clips::make_cmd();
//     sub->name("name");
//     sub->brief("brief");
//     sub->desc("desc");
//     sub->flag<int>("aaa", "a", 1, "desc");
//     sub->flag<int>("bbb", "b", 2, { 0, 1, 2 }, "desc", true);
//     sub->pflag<uint32_t>(&g_ccc, "ccc", "", 5, "desc", true);
//     sub->example("example");
//     sub->bind(your_func);
//     return clips::bind(sub);
// }
//
// main.cpp:
// static uint32_t g_fff = 0;
// clips::error_t your_root(const pcmd_t& pcmd, const args_t& args)
// {
//     处理
//     return clips::ok;
// }
// int main(int argc, char* argv[])
// {
//     clips::name(argv[0]);
//     clips::desc("desc");
//     clips::flag<uint32_t>("ccc", "c", 2, "desc");
//     clips::flag<uint32_t>("ddd", "", 3, "desc");
//     clips::flag<uint32_t>("eee", "", 4, "desc", true);
//     clips::pflag<uint32_t>(&g_fff, "fff", "", 5, "desc", true);
//     clips::bind(your_root);
// 
//     auto ret = clips::exec(argc, argv);
//     if (clips::ok != ret)
//     {
//         std::cout << ret << std::endl;
//         return 1;
//     }
//     return 0;
// }

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <functional>
#include <typeindex>
#include <typeinfo>
#include <iostream>
#include <sstream>
#include <regex>
#include <tuple>
#include <cstring>

/// CLIPS_INIT 初始化函数
#define CLIPS_INIT() INNER_CLIPS_INIT_IMPL(__FILE__, __LINE__)

// INNER_CLIPS_INIT_IMPL 实现
#define INNER_CLIPS_INIT_IMPL(file, line) \
    INNER_CLIPS_INIT_FUNC_IMPL(INNER_CLIPS_CLI_UNIQUE_NAME(__CLIPS__INIT__LINE__FUNC__, line), file, line)
#define INNER_CLIPS_INIT_FUNC_IMPL(func_name, file, line) \
    static clips::error_t func_name(); \
    namespace { \
        static const bool INNER_CLIPS_CLI_UNIQUE_NAME(__CLIPS__CMD__LINE__LOCAL__VAR__, line) \
            INNER_CLIPS_CLI_UNUSED { clips::_bind_init_func(func_name, file, line) }; \
    } \
    static clips::error_t func_name()

// generate unique name
#define INNER_CLIPS_CLI_UNIQUE_NAME(name, line) name##line##__

// disable unused warning
#ifdef __GNUC__
#define INNER_CLIPS_CLI_UNUSED __attribute__((unused))
#else
#define INNER_CLIPS_CLI_UNUSED 
#endif

// version 版本
static constexpr const char* CLIPS_VERSION_CODE = "0.1.1";
constexpr const uint32_t CLIPS_VERSION_NUMBER_FUNC(int major, int minor, int patch)
{
    return ((major) * 1000 * 1000 + (minor) * 1000 + (patch));
}
static constexpr const uint32_t CLIPS_VERSION_NUMBER = CLIPS_VERSION_NUMBER_FUNC(0, 1, 1);

namespace clips {
// ----------------------------------------------------------------------------

/// version 版本
inline const char* version()
{
    return CLIPS_VERSION_CODE;
}

/// version 版本
inline uint32_t version_number()
{
    return CLIPS_VERSION_NUMBER;
}

// ----------------------------------------------------------------------------
// forward declaration

inline void name(const std::string& name);
inline const std::string& name();
inline void desc(const std::string& desc);
inline const std::string& desc();

class cmd_t;
class flag_t;

// ----------------------------------------------------------------------------
// error_t

/// error_t 错误信息
class error_t
{
public:
    error_t()
    {
    }

    explicit error_t(const std::string& msg)
        : msg_(msg)
    {
    }

    explicit error_t(const std::string& msg, const std::string& stack)
        : msg_(msg)
        , stack_(stack)
    {
    }

    error_t(const error_t& cpy)
        : msg_(cpy.msg_)
        , stack_(cpy.stack_)
    {
    }

    error_t(error_t&& mv) noexcept
        : msg_(std::move(mv.msg_))
        , stack_(std::move(mv.stack_))
    {
    }

    error_t& operator=(const error_t& rhs)
    {
        msg_ = rhs.msg_;
        stack_ = rhs.stack_;
        return *this;
    }
 
    // == 等于
    // 只判断是否是同一种错误信息，而不关心堆栈信息是否相同
    bool operator==(const error_t& rhs) const
    {
        return (msg_ == rhs.msg_);
    }

    // != 不等于
    // 只判断是否是同一种错误信息，而不关心堆栈信息是否相同
    bool operator!=(const error_t& rhs) const
    {
        return (msg_ != rhs.msg_);
    }

    // << 流式打印
    std::ostream& operator<<(std::ostream& ss) const
    {
        ss << msg_ << " " << stack_;
        return ss;
    }

    // to_string 错误和堆栈信息
    std::string to_string() const
    {
        return std::move(msg_ + " " + stack_);
    }

    // msg 错误信息
    void msg(const std::string& msg)
    {
        msg_ = msg;
    }

    // msg 错误信息
    const std::string& msg() const
    {
        return msg_;
    }

    // statck 堆栈，可确定位置信息
    void stack(const std::string& stack)
    {
        stack_ = stack;
    }

    // statck 堆栈，可确定位置信息
    const std::string& stack() const
    {
        return stack_;
    }

private:
    std::string msg_;
    std::string stack_;
};

// ok 成功
static const error_t ok{};

/// make_error 创建错误
inline error_t make_error(const std::string& msg)
{
    return std::move(error_t(msg));
}

/// make_error 创建错误
inline error_t make_error(const std::string& msg, const std::string& stack)
{
    return std::move(error_t(msg, stack));
}

/// << 流式打印
inline std::ostream& operator<<(std::ostream& os, const error_t& err)
{
    os << err.msg() << " " << err.stack();
    return os;
}

// ----------------------------------------------------------------------------
// types

/// argv_t 原始参数列表
using argv_t = std::vector<std::string>;

/// args_t 命令参数列表
using args_t = std::vector<std::string>;

/// pcmd_t 命令指针
using pcmd_t = std::shared_ptr<cmd_t>;

/// cmds_t 命令列表
using cmds_t = std::unordered_map<std::string, pcmd_t>;

/// pflag_t 标记指针
using pflag_t = std::shared_ptr<flag_t>;

/// flags_t 标记列表
using flags_t = std::unordered_map<std::string, pflag_t>;

/// func_t 命令函数
using func_t = std::function<error_t(const pcmd_t& pcmd, const args_t& args)>;

// _init_func_t 初始化函数
using _init_func_t = std::function<error_t(void)>;

// _inits_t 初始化函数列表
using _inits_t = std::vector<std::tuple<_init_func_t, std::string, int>>;

// ----------------------------------------------------------------------------
// utils

// utils 工具类
class utils
{
public:
    // starts_with C Style 判断是否以指定字符串起始
    // @param sz char* 字符串
    // @param prefix char* 前缀
    // @return bool true = 是，false = 否
    static bool starts_with(const char* sz, const char* prefix)
    {
        return strncmp(sz, prefix, strlen(prefix)) == 0;
    }

    // ends_with C Style 判断字符串是否以指定字符串结尾
    // @param sz char* 字符串
    // @param suffix char* 后缀
    // @return bool true = 是，false = 否
    static bool ends_with(const char* sz, const char* suffix)
    {
        size_t sz_len = strlen(sz);
        size_t suffix_len = strlen(suffix);
        if (suffix_len > sz_len)
        {
            return false;
        }
        const char* substr = sz + (sz_len - suffix_len);
        return strncmp(substr, suffix, suffix_len) == 0;
    }

    // trim 去除头尾的空白字符或其他字符
    // @param str std::string 字符串
    // @param trim std::string 空白字符，默认" \t\r\n"
    // @return std::string 处理完的字符串
    static std::string trim(const std::string& str, const std::string& trim = " \t\r\n")
    {
        std::string::size_type first = str.find_first_not_of(trim.c_str());
        std::string::size_type last = str.find_last_not_of(trim.c_str());

        if (first == std::string::npos || last == std::string::npos)
        {
            return "";
        }

        return std::move(str.substr(first, last - first + 1));
    }

    // trim_left 去除头部的空白字符或其他字符
    // @param str std::string 字符串
    // @param trim std::string 空白字符，默认" \t\r\n"
    // @return std::string 处理完的字符串
    static std::string trim_left(const std::string& str, const std::string& trim = " \t\r\n")
    {
        std::string::size_type first = str.find_first_not_of(trim.c_str());
        std::string::size_type last = str.size();

        if (first == std::string::npos)
        {
            return str;
        }

        return std::move(str.substr(first, last - first + 1));
    }

    // split 切分字符串
    // @brief 是否需要支持移动语义或修改函数定义来保证性能？
    // @param str std::string 字符串
    // @param d std::string 分隔符，默认" "
    // @return std::vector<std::string> 切分结果
    static void split(std::vector<std::string>& dst, const std::string& str, const std::string& d = " ")
    {
        dst.clear();

        if ("" == str)
        {
            dst.emplace_back("");
            return;
        }

        std::string::size_type pos1{ 0 };
        std::string::size_type pos2{ 0 };
        size_t len = str.length();
        pos2 = str.find(d);
        pos1 = 0;
        while (std::string::npos != pos2)
        {
            dst.emplace_back(str.substr(pos1, pos2 - pos1));
            pos1 = pos2 + d.size();
            pos2 = str.find(d, pos1);
        }

        if (pos1 != len)
        {
            dst.emplace_back(str.substr(pos1));
        }
        else
        {
            dst.emplace_back("");
        }

        return;
    }

    // filename 文件名
    static std::string filename(const char* sz)
    {
        return std::move(filename(std::string(sz)));
    }

    // filename 文件名
    static std::string filename(const std::string& path)
    {
        std::string::size_type pos = path.find_last_of("/\\");
        if (pos == std::string::npos)
        {
            return path;
        }
        return std::move(path.substr(pos + 1, path.length() - pos - 1));
    }

    // pading_left 使用 pad 填充字符串
    static std::string pading_left(const std::string& src, size_t max, const char* pad)
    {
        if (max <= src.length())
        {
            return src;
        }
        if (nullptr == pad || strcmp("", pad) == 0)
        {
            return std::string(max - src.length(), ' ') + src;
        }
        return std::string(max - src.length(), pad[0]) + src;
    }

    // pading_right 使用 pad 填充字符串
    static std::string pading_right(const std::string& src, size_t max, const char* pad)
    {
        if (max <= src.length())
        {
            return src;
        }
        if (nullptr == pad || strcmp("", pad) == 0)
        {
            return src + std::string(max - src.length(), ' ');
        }
        return src + std::string(max - src.length(), pad[0]);
    }

private:
    utils() {}
};

// ----------------------------------------------------------------------------
// flag

// flag_cast_exception cast exception
class flag_cast_exception : public std::exception
{
public:
    flag_cast_exception()
    {
    }

    virtual ~flag_cast_exception()
    {
    }

    explicit flag_cast_exception(const char* msg)
        : msg_(msg)
    {
    }

    explicit flag_cast_exception(const std::string& msg)
        : msg_(msg)
    {
    }

    flag_cast_exception(const flag_cast_exception& cpy)
        : msg_(cpy.msg_)
    {
    }

    flag_cast_exception(flag_cast_exception&& mv) noexcept
        : msg_(std::move(mv.msg_))
    {
    }

    flag_cast_exception& operator=(const flag_cast_exception& rhs)
    {
        msg_ = rhs.msg_;
        return *this;
    }

    void msg(const std::string& msg)
    {
        msg_ = msg;
    }

    virtual const char* what()
    {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

// flag_t 
class flag_t
{
public:
    flag_t()
    {
    }

    flag_t(const flag_t& cpy)
        : holder_(cpy.clone())
        , extend_(cpy.extend_)
        , exist_(cpy.exist_)
        , required_(cpy.required_)
        , name_(cpy.name_)
        , fast_(cpy.fast_)
        , desc_(cpy.desc_)
        , stack_(cpy.stack_)
        , default_(cpy.default_)
        , text_(cpy.text_)
        , oneof_(cpy.oneof_)
    {
    }

    flag_t(flag_t&& mv) noexcept
        : holder_(std::move(mv.holder_))
        , extend_(mv.extend_)
        , exist_(mv.exist_)
        , required_(mv.required_)
        , name_(std::move(mv.name_))
        , fast_(std::move(mv.fast_))
        , desc_(std::move(mv.desc_))
        , stack_(std::move(mv.stack_))
        , default_(std::move(mv.default_))
        , text_(std::move(mv.text_))
        , oneof_(std::move(mv.oneof_))
    {
    }

    flag_t& operator=(const flag_t& rhs)
    {
        if (holder_ == rhs.holder_)
        {
            return *this;
        }
        holder_ = rhs.clone();
        extend_ = rhs.extend_;
        exist_ = rhs.exist_;
        required_ = rhs.required_;
        name_ = rhs.name_;
        fast_ = rhs.fast_;
        desc_ = rhs.desc_;
        stack_ = rhs.stack_;
        default_ = rhs.default_;
        text_ = rhs.text_;
        oneof_ = rhs.oneof_;
        return *this;
    }

    /// is_null 是否为空
    bool is_null()
    {
        return !bool(holder_);
    }

    /// type_name 类型名称
    std::string type_name()
    {
        if (!bool(holder_))
        {
            return "null";
        }
        if (holder_->type_index() == typeid(std::string))
        {
            return "std::string";
        }
        return holder_->type_index().name();
    }

    /// castable 是否可以转换为指定类型
    template<class T>
    bool castable()
    {
        if (!bool(holder_))
        {
            return false;
        }
        return holder_->type_index() == std::type_index(typeid(T));
    }

    /// cast 转换，类型不一致时会抛出异常
    template<class T>
    T& cast()
    {
        if (!castable<T>())
        {
            if (!bool(holder_))
            {
                throw flag_cast_exception(
                    std::string("can not cast null")
                    + " to " + typeid(T).name()
                );
            }
            else
            {
                throw flag_cast_exception(
                    std::string("can not cast ") + holder_->type_index().name()
                    + " to " + typeid(T).name()
                );
            }
        }
        auto ptr = dynamic_cast<value_holder<T>*>(holder_.get());
        return ptr->value_;
    }

    /// is_oneof 是否为合法枚举值
    bool is_oneof(const std::string& text)
    {
        if (oneof_.size() == 0)
        {
            return true; // 不是枚举类型，就都合法
        }
        auto it = oneof_.begin();
        while (it != oneof_.end())
        {
            if (*it == text)
            {
                break;
            }
        }
        return (it != oneof_.end());
    }

    /// parse 解析
    error_t parse(const std::string& text)
    {
        if (!bool(holder_))
        {
            return make_error("flag is null", stack_);
        }
        if (oneof_.size() != 0)
        {
            auto it = oneof_.begin();
            for (; it != oneof_.end(); ++it)
            {
                if (*it == text)
                {
                    break;
                }
            }
            if (it == oneof_.end())
            {
                return make_error("not one of flag options.", stack_);
            }
        }
        text_ = text;
        if (!holder_->parse(text))
        {
            return make_error(std::string("parse failed. type must be ") + holder_->type_index().name(), stack_);
        }
        return ok;
    }

    /// extend 继承属性
    void extend(bool extend)
    {
        extend_ = extend;
    }

    /// extend 继承属性
    bool extend()
    {
        return extend_;
    }

    /// exist 是否存在
    void exist(bool exist)
    {
        exist_ = exist;
    }

    /// exist 是否存在
    bool exist()
    {
        return exist_;
    }

    /// name 名称
    void name(const std::string& name)
    {
        name_ = name;
    }

    /// name 名称
    std::string name()
    {
        return name_;
    }

    /// fast 快捷名称
    void fast(const std::string& fast)
    {
        fast_ = fast;
    }

    /// fast 快捷名称
    std::string fast()
    {
        return fast_;
    }

    /// desc 描述
    void desc(const std::string& desc)
    {
        desc_ = desc;
    }

    /// desc 描述
    std::string desc()
    {
        return desc_;
    }

    // stack 堆栈，可确定命令分支的位置
    void stack(const std::string& stack)
    {
        stack_ = stack;
    }

    // stack 堆栈，可确定命令分支的位置
    const std::string& stack() const
    {
        return stack_;
    }

    /// default_value 默认值的字符串表示
    std::string default_value()
    {
        return default_;
    }

    /// text 输入的字符串
    std::string text()
    {
        return text_;
    }

    /// oneof 枚举值列表
    std::vector<std::string>& oneof()
    {
        return oneof_;
    }

    /// set 设置
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value 
        && !std::is_reference<T>::value
        && !std::is_same<T, bool>::value
        && !std::is_same<T, std::string>::value
        && !std::is_same<T, unsigned char>::value
        && !std::is_same<T, char>::value
        && !std::is_same<T, signed char>::value, T>::type>
    error_t set(const std::string& name, const std::string& fast, T default_v, 
        const std::string& desc, bool extend = false)
    {
        auto err = set_value<T>(name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        std::stringstream oss;
        oss << "" << default_v;
        default_ = oss.str();

        return ok;
    }

    /// set 设置
    template <class T,
        class = bool>
    error_t set(const std::string& name, const std::string& fast, 
        bool default_v, const std::string& desc, bool extend)
    {
        auto err = set_value<bool>(name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = default_v ? "true" : "false";

        return ok;
    }

    /// set 设置
    template <class T,
        class = char>
    error_t set(const std::string& name, const std::string& fast,
        char default_v, const std::string& desc, bool extend)
    {
        auto err = set_value<char>(name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = std::to_string(default_v);

        return ok;
    }

    /// set 设置
    template <class T,
        class = signed char>
    error_t set(const std::string& name, const std::string& fast,
        signed char default_v, const std::string& desc, bool extend)
    {
        auto err = set_value<signed char>(name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = std::to_string(default_v);

        return ok;
    }

    /// set 设置
    template <class T,
        class = unsigned char>
    error_t set(const std::string& name, const std::string& fast,
        unsigned char default_v, const std::string& desc, bool extend)
    {
        auto err = set_value<unsigned char>(name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = std::to_string(default_v);

        return ok;
    }

    /// set 设置
    template <class T,
        class = std::string>
    error_t set(const std::string& name, const std::string& fast, 
        const std::string& default_v, const std::string& desc, bool extend)
    {
        auto err = set_value<std::string>(name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = default_v;

        return ok;
    }

    /// set 设置
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value
        && !std::is_reference<T>::value
        && !std::is_same<T, double>::value
        && !std::is_same<T, float>::value
        && !std::is_same<T, bool>::value
        && !std::is_same<T, std::string>::value
        && !std::is_same<T, unsigned char>::value
        && !std::is_same<T, char>::value
        && !std::is_same<T, signed char>::value, T>::type>
    error_t set(const std::string& name, const std::string& fast, T default_v,
        const std::vector<T>& options, const std::string& desc, bool extend = false)
    {
        auto err = set_value<T>(name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        std::stringstream oss;
        oss << default_v;
        default_ = oss.str();

        oneof_.clear();
        for (auto& item : options)
        {
            std::stringstream oss;
            oss << item;
            oneof_.push_back(oss.str());
        }

        return ok;
    }

    /// set 设置
    template <class T,
        class = char>
    error_t set(const std::string& name, const std::string& fast, char default_v,
        const std::vector<char>& options, const std::string& desc, bool extend = false)
    {
        auto err = set_value<char>(name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = std::to_string(default_v);

        oneof_.clear();
        for (auto& item : options)
        {
            oneof_.push_back(std::to_string(item));
        }

        return ok;
    }

    /// set 设置
    template <class T,
        class = signed char>
    error_t set(const std::string& name, const std::string& fast, signed char default_v,
        const std::vector<signed char>& options, const std::string& desc, bool extend = false)
    {
        auto err = set_value<signed char>(name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = std::to_string(default_v);

        oneof_.clear();
        for (auto& item : options)
        {
            oneof_.push_back(std::to_string(item));
        }

        return ok;
    }

    /// set 设置
    template <class T,
        class = unsigned char>
    error_t set(const std::string& name, const std::string& fast, unsigned char default_v,
        const std::vector<unsigned char>& options, const std::string& desc, bool extend = false)
    {
        auto err = set_value<unsigned char>(name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = std::to_string(default_v);

        oneof_.clear();
        for (auto& item : options)
        {
            oneof_.push_back(std::to_string(item));
        }

        return ok;
    }

    /// set 设置
    template <class T,
        class = std::string>
    error_t set(const std::string& name, const std::string& fast, const std::string& default_v,
        const std::vector<std::string>& options, const std::string& desc, bool extend = false)
    {
        auto err = set_value<std::string>(name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = default_v;

        oneof_.clear();
        for (auto& item : options)
        {
            oneof_.push_back(item);
        }

        return ok;
    }

    /// set 设置
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value
        && !std::is_reference<T>::value
        && !std::is_same<T, bool>::value
        && !std::is_same<T, std::string>::value
        && !std::is_same<T, unsigned char>::value
        && !std::is_same<T, char>::value
        && !std::is_same<T, signed char>::value, T>::type>
    error_t set(T* ptr, const std::string& name, const std::string& fast, T default_v,
        const std::string& desc, bool extend = false)
    {
        auto err = set_value<T>(ptr, name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        std::stringstream oss;
        oss << default_v;
        default_ = oss.str();

        return ok;
    }

    /// set 设置
    template <class T,
        class = bool>
    error_t set(bool* ptr, const std::string& name, const std::string& fast, 
        bool default_v, const std::string& desc, bool extend = false)
    {
        auto err = set_value<bool>(ptr, name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = default_v ? "true" : "false";

        return ok;
    }

    /// set 设置
    template <class T,
        class = char>
    error_t set(char* ptr, const std::string& name, const std::string& fast,
        char default_v, const std::string& desc, bool extend = false)
    {
        auto err = set_value<char>(ptr, name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = std::to_string(default_v);

        return ok;
    }

    /// set 设置
    template <class T,
        class = signed char>
    error_t set(signed char* ptr, const std::string& name, const std::string& fast,
        signed char default_v, const std::string& desc, bool extend = false)
    {
        auto err = set_value<signed char>(ptr, name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = std::to_string(default_v);

        return ok;
    }

    /// set 设置
    template <class T,
        class = unsigned char>
    error_t set(unsigned char* ptr, const std::string& name, const std::string& fast,
        unsigned char default_v, const std::string& desc, bool extend = false)
    {
        auto err = set_value<unsigned char>(ptr, name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = std::to_string(default_v);

        return ok;
    }

    /// set 设置
    template <class T,
        class = std::string>
    error_t set(std::string* ptr, const std::string& name, const std::string& fast, 
        const std::string& default_v, const std::string& desc, bool extend = false)
    {
        auto err = set_value<std::string>(ptr, name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = default_v;

        return ok;
    }

    /// set 设置
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value
        && !std::is_reference<T>::value
        && !std::is_same<T, double>::value
        && !std::is_same<T, float>::value
        && !std::is_same<T, bool>::value
        && !std::is_same<T, std::string>::value
        && !std::is_same<T, unsigned char>::value
        && !std::is_same<T, char>::value
        && !std::is_same<T, signed char>::value, T>::type>
    error_t set(T* ptr, const std::string& name, const std::string& fast, T default_v,
        const std::vector<T>& options, const std::string& desc, bool extend = false)
    {
        auto err = set_value<T>(ptr, name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        std::stringstream oss;
        oss << default_v;
        default_ = oss.str();

        oneof_.clear();
        for (auto& item : options)
        {
            std::stringstream oss;
            oss << item;
            oneof_.push_back(oss.str());
        }

        return ok;
    }

    /// set 设置
    template <class T,
        class = char>
    error_t set(char* ptr, const std::string& name, const std::string& fast, char default_v,
        const std::vector<char>& options, const std::string& desc, bool extend = false)
    {
        auto err = set_value<char>(ptr, name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = std::to_string(default_v);

        oneof_.clear();
        for (auto& item : options)
        {
            oneof_.push_back(std::to_string(item));
        }

        return ok;
    }

    /// set 设置
    template <class T,
        class = signed char>
    error_t set(signed char* ptr, const std::string& name, const std::string& fast, signed char default_v,
        const std::vector<signed char>& options, const std::string& desc, bool extend = false)
    {
        auto err = set_value<signed char>(ptr, name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = std::to_string(default_v);

        oneof_.clear();
        for (auto& item : options)
        {
            oneof_.push_back(std::to_string(item));
        }

        return ok;
    }

    /// set 设置
    template <class T,
        class = unsigned char>
    error_t set(unsigned char* ptr, const std::string& name, const std::string& fast, unsigned char default_v,
        const std::vector<unsigned char>& options, const std::string& desc, bool extend = false)
    {
        auto err = set_value<unsigned char>(ptr, name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = std::to_string(default_v);

        oneof_.clear();
        for (auto& item : options)
        {
            oneof_.push_back(std::to_string(item));
        }

        return ok;
    }

    /// set 设置
    template <class T,
        class = std::string>
    error_t set(std::string* ptr, const std::string& name, const std::string& fast, const std::string& default_v,
        const std::vector<std::string>& options, const std::string& desc, bool extend = false)
    {
        auto err = set_value<std::string>(ptr, name, fast, default_v, desc, extend);
        if (err != ok)
        {
            return err;
        }

        default_ = default_v;

        oneof_.clear();
        for (auto& item : options)
        {
            oneof_.push_back(item);
        }

        return ok;
    }

private:
    // ctor [禁用] 隐式构造
    template<class T, 
        class = typename std::enable_if<std::is_same<T, flag_t>::value, T>::type>
    flag_t(T&& v)
        : holder_(new value_holder<T>(std::forward<T>(v)))
    {
    }

    // = [禁用] 隐式赋值
    template<class T,
        class = typename std::enable_if<std::is_same<T, flag_t>::value, T>::type>
    flag_t& operator=(T& rhs)
    {
        holder_.reset(new value_holder<T>(std::forward<T>(rhs)));
        return *this;
    }

    error_t check_name(const std::string& name, const std::string& fast)
    {
        if (name.empty())
        {
            return make_error("flag name must be not null.");
        }
        if (name.length() <= 1)
        {
            return make_error("flag name length too short, len > 1.");
        }
        if (fast.length() > 1)
        {
            return make_error("flag fast length too long, len <= 1.");
        }
        return ok;
    }

    /// set_value 设置
    /// todo: 标准名称的检查
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value
        && !std::is_reference<T>::value, T>::type>
    error_t set_value(const std::string& name, const std::string& fast,
        T default_v, const std::string& desc, bool extend = false)
    {
        auto err = check_name(name, fast);
        if (err != ok)
        {
            return err;
        }

        name_ = name;
        fast_ = fast;
        desc_ = desc;
        extend_ = extend;

        holder_.reset(new value_holder<T>(std::forward<T>(default_v)));

        return ok;
    }

    /// set_value 设置
    /// todo: 标准名称的检查
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value
        && !std::is_reference<T>::value, T>::type>
    error_t set_value(T* ptr, const std::string& name, const std::string& fast,
        T default_v, const std::string& desc, bool extend = false)
    {
        auto err = check_name(name, fast);
        if (err != ok)
        {
            return err;
        }

        name_ = name;
        fast_ = fast;
        desc_ = desc;
        extend_ = extend;

        holder_.reset(new value_holder<T>(ptr, std::forward<T>(default_v)));

        return ok;
    }

    struct holder;
    using holder_ptr = std::unique_ptr<holder>;

    struct holder
    {
        virtual ~holder() {}
        virtual holder_ptr clone() const = 0;
        virtual bool parse(const std::string& text) = 0;
        virtual const std::type_index type_index() = 0;
    };

    template<typename T>
    struct value_holder : public holder
    {
        using type = T;

        value_holder()
        {
            memset(&value_, 0, sizeof(value_));
        }

        value_holder(const value_holder& cpy)
            : ptr_(cpy.ptr_)
            , value_(cpy.value_)
        {
        }

        value_holder(value_holder&& cpy)
            : ptr_(std::move(cpy.ptr_))
            , value_(std::move(cpy.value_))
        {
        }

        value_holder& operator=(const value_holder& rhs)
        {
            ptr_ = rhs.ptr_;
            value_ = rhs.value_;
        }

        value_holder(const T& v)
            : value_(v)
        {
        }

        value_holder(T* ptr, const T& v)
            : ptr_(ptr)
            , value_(v)
        {
            if (nullptr != ptr_)
            {
                *ptr_ = value_;
            }
        }

        virtual holder_ptr clone() const
        {
            return holder_ptr(new value_holder(ptr_, value_));
        }

        virtual bool parse(const std::string& text)
        {
            std::stringstream iss(text);
            iss >> value_;
            if (!iss)
            {
                return false;
            }
            if (nullptr != ptr_)
            {
                *ptr_ = value_;
            }
            return true;
        }

        virtual const std::type_index type_index()
        {
            return std::move(std::type_index(typeid(value_)));
        }

        T* ptr_{ nullptr };
        T value_{ 0 }; // note: 一些类型在特殊的编译器版本上会有性能问题，应该避免大内存和复杂类型做为flag。
    };

    holder_ptr clone() const
    {
        if (holder_)
        {
            return holder_->clone();
        }
        return nullptr;
    }

    // 数据
    holder_ptr      holder_;

    // extend_ 是否可继承
    bool extend_{ false };

    // exist_ 是否存在
    bool exist_{ false };

    // required_ 是否必须
    // todo: no implements
    bool required_{ false };

    // name_ 名称
    std::string name_;

    // fast_ 快捷名称
    std::string fast_;

    // desc_ 描述
    std::string desc_;

    // stack_ 堆栈
    std::string stack_;

    // default_ 默认值对应的字符串表示
    std::string default_;

    // text_ 输入的字符串
    std::string text_;

    // oneof 可选项，枚举值
    std::vector<std::string> oneof_;
};

// ----------------------------------------------------------------------------
// cmd_t

// cmd_t 命令
class cmd_t
{
public:
    cmd_t()
    {
    }

    ~cmd_t()
    {
    }

    explicit cmd_t(const std::string& name)
        : name_(name)
    {
    }

    cmd_t(const cmd_t& cpy)
        : name_(cpy.name_)
        , brief_(cpy.brief_)
        , desc_(cpy.desc_)
        , example_(cpy.example_)
        , stack_(cpy.stack_)
        , subs_(cpy.subs_)
        , flags_(cpy.flags_)
        , on_func_(cpy.on_func_)
        , parent_ptr_(cpy.parent_ptr_)
    {
    }

    cmd_t(cmd_t&& mv) noexcept
        : name_(std::move(mv.name_))
        , brief_(std::move(mv.brief_))
        , desc_(std::move(mv.desc_))
        , example_(std::move(mv.example_))
        , stack_(std::move(mv.stack_))
        , subs_(std::move(mv.subs_))
        , flags_(std::move(mv.flags_))
        , on_func_(std::move(mv.on_func_))
        , parent_ptr_(std::move(mv.parent_ptr_))
    {
    }

    cmd_t& operator=(const cmd_t& rhs)
    {
        name_ = rhs.name_;
        brief_ = rhs.brief_;
        desc_ = rhs.desc_;
        example_ = rhs.example_;
        stack_ = rhs.stack_;
        subs_ = rhs.subs_;
        flags_ = rhs.flags_;
        on_func_ = rhs.on_func_;
        parent_ptr_ = rhs.parent_ptr_;
        return *this;
    }

    cmd_t& operator=(cmd_t&& mv) noexcept
    {
        name_ = std::move(mv.name_);
        brief_ = std::move(mv.brief_);
        desc_ = std::move(mv.desc_);
        example_ = std::move(mv.example_);
        stack_ = std::move(mv.stack_);
        subs_ = std::move(mv.subs_);
        flags_ = std::move(mv.flags_);
        on_func_ = std::move(mv.on_func_);
        parent_ptr_ = mv.parent_ptr_;
        return *this;
    }

    // name 命令名称
    void name(const std::string& name)
    {
        name_ = name;
    }

    // name 命令名称
    const std::string& name() const
    {
        return name_;
    }

    // brief 简短描述
    void brief(const std::string& brief)
    {
        brief_ = brief;
    }

    // brief 简短描述
    const std::string& brief() const
    {
        return brief_;
    }

    // desc 命令描述
    void desc(const std::string& desc)
    {
        desc_ = desc;
    }

    // desc 命令描述
    const std::string& desc() const
    {
        return desc_;
    }

    // example 示例
    void example(const std::string& example)
    {
        example_ = example;
    }

    // example 示例
    const std::string& example() const
    {
        return example_;
    }

    // stack 堆栈，可确定当前命令在分支中的位置
    // 堆栈并不是在绑定命令时确定的，而是在解析时确定的。
    // 因为pcmd_t可以复用，存在于多条命令分支中。
    void stack(const std::string& stack)
    {
        stack_ = stack;
    }

    // stack 堆栈，可确定当前命令在分支中的位置
    // 堆栈并不是在绑定命令时确定的，而是在解析时确定的。
    // 因为pcmd_t可以复用，存在于多条命令分支中。
    const std::string& stack() const
    {
        return stack_;
    }

    // subs 子命令列表
    cmds_t& subs()
    {
        return subs_;
    }

    /// flags 标记列表
    flags_t& flags()
    {
        return flags_;
    }

    // bind 绑定函数
    error_t bind(func_t func)
    {
        on_func_ = func;
        return ok;
    }

    // bind 绑定子命令
    // todo: 标准的命令名称的检查
    error_t bind(cmd_t* cmd_ptr)
    {
        if (nullptr == cmd_ptr)
        {
            return make_error("error: cmd_t ptr is null");
        }
        if (cmd_ptr->name().empty())
        {
            return make_error("error: cmd_t name is null");
        }
        auto it = subs_.find(cmd_ptr->name());
        if (it != subs_.end())
        {
            return make_error("error: double defined.");
        }
        subs_[cmd_ptr->name()].reset(cmd_ptr);
        return ok;
    }

    // bind 绑定子命令
    // todo: 标准的命令名称的检查
    error_t bind(pcmd_t& cmd_ptr)
    {
        if (nullptr == cmd_ptr)
        {
            return make_error("error: cmd_t ptr is null");
        }
        if (cmd_ptr->name().empty())
        {
            return make_error("error: cmd_t name is null");
        }
        auto it = subs_.find(cmd_ptr->name());
        if (it != subs_.end())
        {
            return make_error("error: double defined.");
        }
        subs_[cmd_ptr->name()] = cmd_ptr;
        return ok;
    }

    // flag 标记
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value
        && !std::is_reference<T>::value, T>::type>
        error_t flag(const std::string& name, const std::string& fast, T default_value,
            const std::string& desc, bool extend = false)
    {
        auto it = flags_.find("--" + name);
        if (it != flags_.end())
        {
            return make_error("error: double defined flag. name=" + name);
        }
        auto flag_ptr = std::make_shared<flag_t>();
        auto ret = flag_ptr->set<T>(name, fast, default_value, desc, extend);
        if (ok != ret)
        {
            return ret;
        }
        flags_["--" + name] = flag_ptr;
        if (fast == "")
        {
            return ok;
        }
        auto fast_it = flags_.find("-" + fast);
        if (fast_it != flags_.end())
        {
            return make_error("error: double defined flag. fast=" + fast + " name=" + name);
        }
        flags_["-" + fast] = flag_ptr;
        return ok;
    }

    // flag
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value
        && !std::is_reference<T>::value
        && !std::is_same<T, double>::value
        && !std::is_same<T, float>::value
        && !std::is_same<T, bool>::value, T>::type>
        error_t flag(const std::string& name, const std::string& fast, T default_value,
            const std::vector<T>& options, const std::string& desc, bool extend = false)
    {
        auto it = flags_.find("--" + name);
        if (it != flags_.end())
        {
            return make_error("error: double defined flag. name=" + name);
        }
        auto flag_ptr = std::make_shared<flag_t>();
        auto ret = flag_ptr->set<T>(name, fast, default_value, options, desc, extend);
        if (ok != ret)
        {
            return ret;
        }
        flags_["--" + name] = flag_ptr;
        if (fast == "")
        {
            return ok;
        }
        auto fast_it = flags_.find("-" + fast);
        if (fast_it != flags_.end())
        {
            return make_error("error: double defined flag. fast=" + fast + " name=" + name);
        }
        flags_["-" + fast] = flag_ptr;
        return ok;
    }

    // pflag 标记
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value
        && !std::is_reference<T>::value, T>::type>
        error_t pflag(T* ptr, const std::string& name, const std::string& fast, T default_value,
            const std::string& desc, bool extend = false)
    {
        auto it = flags_.find("--" + name);
        if (it != flags_.end())
        {
            return make_error("error: double defined flag. name=" + name);
        }
        auto flag_ptr = std::make_shared<flag_t>();
        auto ret = flag_ptr->set<T>(ptr, name, fast, default_value, desc, extend);
        if (ok != ret)
        {
            return ret;
        }
        flags_["--" + name] = flag_ptr;
        if (fast == "")
        {
            return ok;
        }
        auto fast_it = flags_.find("-" + fast);
        if (fast_it != flags_.end())
        {
            return make_error("error: double defined flag. fast=" + fast + " name=" + name);
        }
        flags_["-" + fast] = flag_ptr;
        return ok;
    }

    // pflag
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value
        && !std::is_reference<T>::value
        && !std::is_same<T, double>::value
        && !std::is_same<T, float>::value
        && !std::is_same<T, bool>::value, T>::type>
        error_t pflag(T* ptr, const std::string& name, const std::string& fast, T default_value,
            const std::vector<T>& options, const std::string& desc, bool extend = false)
    {
        auto it = flags_.find("--" + name);
        if (it != flags_.end())
        {
            return make_error("error: double defined flag. name=" + name);
        }
        auto flag_ptr = std::make_shared<flag_t>();
        auto ret = flag_ptr->set<T>(ptr, name, fast, default_value, options, desc, extend);
        if (ok != ret)
        {
            return ret;
        }
        flags_["--" + name] = flag_ptr;
        if (fast == "")
        {
            return ok;
        }
        auto fast_it = flags_.find("-" + fast);
        if (fast_it != flags_.end())
        {
            return make_error("error: double defined flag. fast=" + fast + " name=" + name);
        }
        flags_["-" + fast] = flag_ptr;
        return ok;
    }

    // cast 转换，不存在或类型不一致时会抛出异常
    template<class T>
    bool castable(const std::string& name)
    {
        auto it = flags_.find(name);
        if (it == flags_.end())
        {
            throw flag_cast_exception("not found.");
        }
        return flags_[name]->castable<T>();
    }

    // cast 转换，不存在或类型不一致时会抛出异常
    template<class T>
    T& cast(const std::string& name)
    {
        auto it = flags_.find(name);
        if (it == flags_.end())
        {
            throw flag_cast_exception("not found.");
        }
        return flags_[name]->cast<T>();
    }

    // parent 上级命令
    // 上级命令并不是在绑定命令时确定的，而是在解析时确定的。
    // 因为pcmd_t可以复用，存在于多条命令分支中。
    void parent(pcmd_t& cmd_ptr)
    {
        parent_ptr_ = cmd_ptr;
    }

    // parent 上级命令
    // 上级命令并不是在绑定命令时确定的，而是在解析时确定的。
    // 因为pcmd_t可以复用，存在于多条命令分支中。
    pcmd_t& parent()
    {
        return parent_ptr_;
    }

    // on_exec 执行命令
    error_t on_exec(const pcmd_t& pcmd, const args_t& args)
    {
        auto fit = flags_.find("--help");
        if (fit != flags_.end() && fit->second->cast<bool>())
        {
            return on_help();
        }
        if (nullptr == on_func_)
        {
            return make_error(" warn: nothing to do");
        }
        return on_func_(pcmd, args);
    }

private:

    // on_help 输出帮助信息
    error_t on_help()
    {
        if (!name_.empty())
        {
            std::cout << std::endl << desc_ << std::endl << std::endl;
        }
        else
        {
            std::cout << std::endl << clips::desc() << std::endl << std::endl;
        }

        std::cout << "usage:" << std::endl
            << "  " << stack_;
        if (subs_.size() != 0)
        {
            std::cout << " [cmds...]";
        }
        std::cout << " [args...] [--flags...] " << std::endl << std::endl;

        if (subs_.size() != 0)
        {
            std::cout << "cmds:" << std::endl;
            size_t cmd_max_len = 0;
            for (auto& item : subs_)
            {
                if (cmd_max_len < item.second->name().length())
                {
                    cmd_max_len = item.second->name().length();
                }
            }
            for (auto& item : subs_)
            {
                std::cout << "  " << utils::pading_right(item.second->name(), cmd_max_len, " ") 
                    << "  " << item.second->brief() << std::endl;
            }
            std::cout << std::endl;
        }

        flags_t flags_tmp;
        for (auto& item : flags_)
        {
            flags_tmp["--" + item.second->name()] = item.second;
        }
        std::cout << "flags:" << std::endl;
        size_t fast_max_len = 0;
        size_t name_max_len = 0;
        size_t type_max_len = 0;
        size_t default_max_len = 0;
        for (auto& item : flags_tmp)
        {
            if (fast_max_len < item.second->fast().length())
            {
                fast_max_len = item.second->fast().length();
            }
            if (name_max_len < item.second->name().length())
            {
                name_max_len = item.second->name().length();
            }
            if (type_max_len < item.second->type_name().length())
            {
                type_max_len = item.second->type_name().length();
            }
            if (default_max_len < item.second->default_value().length())
            {
                default_max_len = item.second->default_value().length();
            }
        }
        for (auto& item : flags_tmp)
        {
            std::cout << "  "
                << utils::pading_right(
                (item.second->fast() == "" ? "" : "-" + item.second->fast() + ","),
                    fast_max_len + 2, " ")
                << " "
                << utils::pading_right("--" + item.second->name(), name_max_len + 2, " ")
                << "  "
                << utils::pading_right("<" + item.second->type_name() + ">", type_max_len + 2, " ")
                << "  ";

            if (item.second->extend())
            {
                std::cout << ":";
            }
            else
            {
                std::cout << " ";
            }
            std::cout << utils::pading_right("(" + item.second->default_value() + ")", default_max_len + 2, " ")
                << "  "
                << item.second->desc();

            auto& oneof = item.second->oneof();
            if (oneof.size() != 0)
            {
                std::cout << "  {";
                for (auto eit = oneof.begin(); eit != oneof.end(); ++eit)
                {
                    if (eit != oneof.begin())
                    {
                        std::cout << ",";
                    }
                    std::cout << *eit;
                }
                std::cout << "}";
            }

            std::cout << std::endl;
        }
        std::cout << std::endl;

        if (!example_.empty())
        {
            std::cout << "example:" << std::endl
                << "  " << example_ << std::endl << std::endl;
        }

        std::cout << "for more information about a command:" << std::endl
            << "  " << clips::name() << " [cmds...] -h" << std::endl
            << "  " << clips::name() << " [cmds...] --help" << std::endl;

        return ok;
    }

    // name_ 命令名称，单行，合法的命令行标识符号
    std::string name_;

    // brief_ 简短描述
    std::string brief_;

    // desc_ 完整描述
    std::string desc_;

    // example 示例
    std::string example_;

    // stack_ 堆栈
    std::string stack_;

    // subs_ 子命令
    cmds_t subs_;

    // flags_ 局部标记列表, 会被子命令继承的标记列表
    flags_t flags_;

    // func_ 执行函数
    func_t on_func_{ nullptr };

    // parent_ptr_ 父级命令
    pcmd_t parent_ptr_{ nullptr };
};

/// make_cmd 创建指令
inline pcmd_t make_cmd()
{
    return std::move(std::make_shared<cmd_t>());
}

/// make_cmd 创建指令
inline pcmd_t make_cmd(const std::string& name)
{
    return std::move(std::make_shared<cmd_t>(name));
}

// ----------------------------------------------------------------------------
// inner

// inner 内部使用
class inner
{
public:
    // get 获取实例
    static inner& get()
    {
        static std::unique_ptr<inner> ins_{ nullptr };
        static std::once_flag flagone;
        std::call_once(flagone, []() {
            ins_.reset(new inner());
        });
        return *ins_;
    }

    // path 模块路径
    const std::string& path() const
    {
        return path_;
    }

    // name 应用名称
    void name(const std::string& name)
    {
        name_ = utils::filename(name);
    }

    // name 应用名称
    const std::string& name() const
    {
        return name_;
    }

    // desc 应用描述
    void desc(const std::string& desc)
    {
        desc_ = desc;
    }

    // desc 应用描述
    const std::string& desc() const
    {
        return desc_;
    }

    // version 版本号
    void version(const std::string& version)
    {
        version_ = version;
    }

    // version 版本号
    const std::string& version() const
    {
        return version_;
    }

    // argv 返回所有原始参数
    const argv_t& argv(void)
    {
        return argv_;
    }

    // bind 绑定根函数
    error_t bind(func_t func)
    {
        return root_->bind(func);
    }

    // bind 绑定子指令
    error_t bind(pcmd_t& ptr)
    {
        return root_->bind(ptr);
    }

    // flag 标记
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value
        && !std::is_reference<T>::value, T>::type>
        error_t flag(const std::string& name, const std::string& fast, T default_value,
            const std::string& desc, bool extend = false)
    {
        return root_->flag(name, fast, default_value, desc, extend);
    }

    // flag 标记
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value
        && !std::is_reference<T>::value
        && !std::is_same<T, double>::value
        && !std::is_same<T, float>::value
        && !std::is_same<T, bool>::value, T>::type>
        error_t flag(const std::string& name, const std::string& fast, T default_value,
            const std::vector<T>& options, const std::string& desc, bool extend = false)
    {
        return root_->flag(name, fast, default_value, options, desc, extend);
    }

    // pflag 标记
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value
        && !std::is_reference<T>::value, T>::type>
        error_t pflag(T* ptr, const std::string& name, const std::string& fast, T default_value,
            const std::string& desc, bool extend = false)
    {
        return root_->pflag(ptr, name, fast, default_value, desc, extend);
    }

    // pflag 标记
    template <class T,
        class = typename std::enable_if<!std::is_pointer<T>::value
        && !std::is_const<T>::value
        && !std::is_reference<T>::value
        && !std::is_same<T, double>::value
        && !std::is_same<T, float>::value
        && !std::is_same<T, bool>::value, T>::type>
        error_t pflag(T* ptr, const std::string& name, const std::string& fast, T default_value,
            const std::vector<T>& options, const std::string& desc, bool extend = false)
    {
        return root_->pflag(ptr, name, fast, default_value, options, desc, extend);
    }

    // _bind_init_func 绑定初始化函数
    bool _bind_init_func(_init_func_t func, const char* file, int line)
    {
        _inits_.push_back(std::make_tuple(func, file, line));
        return false; // 返回值只是为了初始化一个全局静态变量，没有其他作用
    }

    // exec 执行命令
    error_t exec(const std::string& argv)
    {
        utils::split(argv_, argv, " ");

        auto ret = bind_init();
        if (ret != ok)
        {
            return ret;
        }

        return parse(argv_);
    }

    // exec 执行命令
    error_t exec(int argc, char* argv[])
    {
        path_ = argv[0];

        if (name_.empty())
        {
            name_ = utils::filename(argv[0]);
        }

        argv_.clear();
        for (int i = 1; i < argc; i++)
        {
            argv_.push_back(argv[i]);
        }

        auto ret = bind_init();
        if (ret != ok)
        {
            return ret;
        }

        return parse(argv_);
    }

private:
    inner(const inner& cpy) = delete;
    inner& operator=(const inner& rhs) = delete;
    inner()
        : root_(new cmd_t())
    {
        root_->flag<bool>("help", "h", false, "help", true);
    }

    // bind_init 初始化
    error_t bind_init()
    {
        for (auto& item : _inits_)
        {
            auto ret = std::get<0>(item)();
            if (ret != ok)
            {
                ret.stack(std::get<1>(item) + ":" + std::to_string(std::get<2>(item)));
                return ret;
            }
        }
        return ok;
    }

    // parse 解析
    error_t parse(const argv_t& argv)
    {
        std::stringstream full_stack;
        std::stringstream pcmd_stack;
        full_stack << name_;
        pcmd_stack << name_;

        auto& pcmd = root_;
        pcmd->stack(pcmd_stack.str());

        args_t args;
        size_t i = 0;
        while (i < argv.size())
        {
            full_stack << " " << argv[i];

            // 命令和参数处理
            if (!utils::starts_with(argv[i].c_str(), "-"))
            {
                if (args.size() != 0)
                {
                    // 后面的都是参数了，不是子命令
                    args.push_back(utils::trim(argv[i], "\'"));
                    i++;
                    continue;
                }

                auto cit = pcmd->subs().find(argv[i]);
                if (cit == pcmd->subs().end())
                {
                    args.push_back(utils::trim(argv[i], "\'"));
                    i++;
                    continue;
                }

                for (auto& item : pcmd->flags())
                {
                    if (item.second->extend() && cit->second->flags().count(item.first) == 0)
                    {
                        item.second->stack(pcmd_stack.str());
                        cit->second->flags()[item.first] = item.second;
                    }
                }

                cit->second->parent(pcmd);
                pcmd = cit->second;
                pcmd_stack << " " << argv[i];
                pcmd->stack(pcmd_stack.str());

                i++;
                continue;
            }

            // flagt提取
            auto flag_name = argv[i];
            std::string flag_value("");

            auto flag_equal_pos = flag_name.find("=");
            if (flag_equal_pos != std::string::npos)
            {
                flag_value = flag_name.substr(flag_equal_pos + 1, flag_name.length() - flag_equal_pos - 1);
                flag_name = flag_name.substr(0, flag_equal_pos);
            }
            i++;

            auto fit = pcmd->flags().find(flag_name);
            if (fit == pcmd->flags().end())
            {
                return make_error("undefined flag.", full_stack.str());
            }

            // flag转换
            auto pflag = fit->second;
            pflag->exist(true);
            if (pflag->castable<bool>())
            {
                if (flag_equal_pos == std::string::npos)
                {
                    if (i < argv.size())
                    {
                        flag_value = utils::trim(argv[i], "\'");
                        if (flag_value.compare("True") == 0 || flag_value.compare("true") == 0 || flag_value.compare("1") == 0)
                        {
                            pflag->parse("1");
                            full_stack << " " << argv[i];
                            i++;
                        }
                        else if (flag_value.compare("False") == 0 || flag_value.compare("false") == 0 || flag_value.compare("0") == 0)
                        {
                            pflag->parse("0");
                            full_stack << " " << argv[i];
                            i++;
                        }
                        else
                        {
                            pflag->parse("1");
                        }
                    }
                    else
                    {
                        pflag->parse("1");
                    }
                }
                else
                {
                    if (flag_value.compare("True") == 0 || flag_value.compare("true") == 0 || flag_value.compare("1") == 0)
                    {
                        pflag->parse("1");
                    }
                    else if (flag_value.compare("False") == 0 || flag_value.compare("false") == 0 || flag_value.compare("0") == 0)
                    {
                        pflag->parse("0");
                    }
                    else
                    {
                        return make_error("bool value error.", full_stack.str());
                    }
                }
            }
            else
            {
                if (flag_equal_pos == std::string::npos)
                {
                    if (i < argv.size())
                    {
                        flag_value = utils::trim(argv[i], "\'");
                        if (utils::starts_with(flag_value.c_str(), "-"))
                        {
                            return make_error("no value of flag.", full_stack.str());
                        }
                        full_stack << " " << argv[i];
                        auto ret = pflag->parse(flag_value);
                        if (ret != ok)
                        {
                            return make_error(ret.msg(), full_stack.str());
                        }
                        i++;
                    }
                    else
                    {
                        return make_error("no value of flag ", full_stack.str());
                    }
                }
                else
                {
                    auto ret = pflag->parse(flag_value);
                    if (ret != ok)
                    {
                        return make_error(ret.msg(), full_stack.str());
                    }
                }
            }
        }

        error_t err;
        try
        {
            err = pcmd->on_exec(pcmd, args);
        }
        catch (flag_cast_exception& e)
        {
            return make_error(e.what());
        }
        catch (std::exception& e)
        {
            return make_error(e.what());
        }
        return err;
    }

    // path_ 模块路径argv[0]
    std::string path_;

    // name_ 应用名称
    std::string name_;

    // desc 应用描述
    std::string desc_;

    // version_ 版本
    std::string version_;

    // argv_ 原始命令
    argv_t argv_;

    // root_ 根命令
    pcmd_t root_;

    // _inits_ 初始化函数
    _inits_t _inits_;
};

// ----------------------------------------------------------------------------
// global

/// path 应用路径
/// 实际为argv[0]；
inline const std::string& path()
{
    return inner::get().path();
}

/// name 应用名称
/// 如果指定了应用名称，则在exec解析时不会被重写；
/// 如果不指定应用名称，则在exec解析时会使用argv[0]代替；
inline void name(const std::string& name)
{
    inner::get().name();
}

/// name 应用名称
/// 如果指定了应用名称，则在exec解析时不会被重写；
/// 如果不指定应用名称，则在exec解析时会使用argv[0]代替；
inline const std::string& name()
{
    return inner::get().name();
}

/// desc 应用描述
inline void desc(const std::string& desc)
{
    inner::get().desc(desc);
}

/// desc 应用描述
inline const std::string& desc()
{
    return inner::get().desc();
}

/// argv 返回所有原始参数
inline const argv_t& argv()
{
    return inner::get().argv();
}

/// bind 绑定根函数
inline error_t bind(func_t func)
{
    return inner::get().bind(func);
}

/// bind 绑定子命令
inline error_t bind(pcmd_t& ptr)
{
    return inner::get().bind(ptr);
}

/// flag 标记
template <class T,
    class = typename std::enable_if<!std::is_pointer<T>::value
    && !std::is_const<T>::value
    && !std::is_reference<T>::value, T>::type>
    inline error_t flag(const std::string& name, const std::string& fast, T default_value,
        const std::string& desc, bool extend = false)
{
    return inner::get().flag(name, fast, default_value, desc, extend);
}

/// flag
template <class T,
    class = typename std::enable_if<!std::is_pointer<T>::value
    && !std::is_const<T>::value
    && !std::is_reference<T>::value
    && !std::is_same<T, double>::value
    && !std::is_same<T, float>::value
    && !std::is_same<T, bool>::value, T>::type>
    inline error_t flag(const std::string& name, const std::string& fast, T default_value,
        const std::vector<T>& options, const std::string& desc, bool extend = false)
{
    return inner::get().flag(name, fast, default_value, options, desc, extend);
}

/// pflag 标记
template <class T,
    class = typename std::enable_if<!std::is_pointer<T>::value
    && !std::is_const<T>::value
    && !std::is_reference<T>::value, T>::type>
    inline error_t pflag(T* ptr, const std::string& name, const std::string& fast, T default_value,
        const std::string& desc, bool extend = false)
{
    return inner::get().pflag(ptr, name, fast, default_value, desc, extend);
}

/// pflag
template <class T,
    class = typename std::enable_if<!std::is_pointer<T>::value
    && !std::is_const<T>::value
    && !std::is_reference<T>::value
    && !std::is_same<T, double>::value
    && !std::is_same<T, float>::value
    && !std::is_same<T, bool>::value, T>::type>
    inline error_t pflag(T* ptr, const std::string& name, const std::string& fast, T default_value,
        const std::vector<T>& options, const std::string& desc, bool extend = false)
{
    return inner::get().pflag(ptr, name, fast, default_value, options, desc, extend);
}

/// exec 执行命令
inline error_t exec(const std::string& argv)
{
    return inner::get().exec(argv);
}

/// exec 执行命令
inline error_t exec(int argc, char* argv[])
{
    return inner::get().exec(argc, argv);
}

// _bind_init_func 绑定初始化函数, 内部使用
inline bool _bind_init_func(_init_func_t func, const char* file, int line)
{
    return inner::get()._bind_init_func(func, file, line);
}

// ----------------------------------------------------------------------------
} // namespace clips
