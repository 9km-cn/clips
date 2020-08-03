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

#include "clips/clips.hpp"

clips::error_t test_handler(const clips::pcmd_t& pcmd, const clips::args_t& args)
{
    std::cout << "exec test handler" << std::endl;

    std::cout << " args{";
    for (auto& item : args)
    {
        std::cout << item << ", ";
    }
    std::cout << "}" << std::endl;

    std::cout << " flags{extend=" << pcmd->flags().count("--extend")
        << "}" << std::endl;

    return clips::ok;
}

clips::error_t base_handler(const clips::pcmd_t& pcmd, const clips::args_t& args)
{
    std::cout << "exec test base handler" << std::endl;

    std::cout << " args{";
    for (auto& item : args)
    {
        std::cout << item << ", ";
    }
    std::cout << "}" << std::endl;

    std::cout << " flags{" << std::endl
        << "    extend =" << pcmd->flags().at("--extend")->cast<uint32_t>() << std::endl
        << "    bool   =" << pcmd->flags().at("--bool")->cast<bool>() << std::endl
        << "    char   =" << pcmd->flags().at("--char")->cast<char>() << std::endl
        << "    int8   =" << pcmd->flags().at("--int8")->cast<int8_t>() << std::endl
        << "    uint8  =" << pcmd->flags().at("--uint8")->cast<uint8_t>() << std::endl
        << "    int16  =" << pcmd->flags().at("--int16")->cast<int16_t>() << std::endl
        << "    uint16 =" << pcmd->flags().at("--uint16")->cast<uint16_t>() << std::endl
        << "    int32  =" << pcmd->flags().at("--int32")->cast<int32_t>() << std::endl
        << "    uint32 =" << pcmd->flags().at("--uint32")->cast<uint32_t>() << std::endl
        << "    int64  =" << pcmd->flags().at("--int64")->cast<int64_t>() << std::endl
        << "    uint64 =" << pcmd->flags().at("--uint64")->cast<uint64_t>() << std::endl
        << "    float  =" << pcmd->flags().at("--float")->cast<float>() << std::endl
        << "    double =" << pcmd->flags().at("--double")->cast<double>() << std::endl
        << "    string =" << pcmd->flags().at("--string")->cast<std::string>() << std::endl
        << "}" << std::endl;

    return clips::ok;
}

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

clips::error_t custom_handler(const clips::pcmd_t& pcmd, const clips::args_t& args)
{
    std::cout << "exec test custom handler" << std::endl;

    std::cout << " args{";
    for (auto& item : args)
    {
        std::cout << item << ", ";
    }
    std::cout << "}" << std::endl;

    std::cout << " flags{" << std::endl
        << "    extend=" << pcmd->cast<uint32_t>("--extend") << std::endl
        << "    custom=" << pcmd->cast<custom_t>("--custom") << std::endl
        << "}" << std::endl;

    return clips::ok;
}

CLIPS_INIT()
{
    auto test = clips::make_cmd("test");
    test->brief("test clips");
    test->desc("test clips for some type flags.");
    test->example("test --name=value");
    test->bind(test_handler);

    auto base = clips::make_cmd("base");
    base->brief("test base types");
    base->desc("test base types.");
    base->example("test base --name=value");

    auto err = base->flag<bool>("bool", "", false, "bool type");
    if (err != clips::ok)
    {
        return err;
    }
    err = base->flag<char>("char", "", 0u, "char type");
    if (err != clips::ok)
    {
        return err;
    }
    err = base->flag<int8_t>("int8", "", 0, "int8_t type");
    if (err != clips::ok)
    {
        return err;
    }
    err = base->flag<uint8_t>("uint8", "", 0u, "uint8_t type");
    if (err != clips::ok)
    {
        return err;
    }
    err = base->flag<int16_t>("int16", "", 0, "int16_t type");
    if (err != clips::ok)
    {
        return err;
    }
    err = base->flag<uint16_t>("uint16", "", 0u, "uint16_t type");
    if (err != clips::ok)
    {
        return err;
    }
    err = base->flag<int32_t>("int32", "", 0, "int32_t type");
    if (err != clips::ok)
    {
        return err;
    }
    err = base->flag<uint32_t>("uint32", "", 0u, "uint32_t type");
    if (err != clips::ok)
    {
        return err;
    }
    err = base->flag<int64_t>("int64", "", 0, "int64_t type");
    if (err != clips::ok)
    {
        return err;
    }
    err = base->flag<uint64_t>("uint64", "", 0u, "uint64_t type");
    if (err != clips::ok)
    {
        return err;
    }
    err = base->flag<float>("float", "", 0.0f, "float type");
    if (err != clips::ok)
    {
        return err;
    }
    err = base->flag<double>("double", "", 0.0, "double type");
    if (err != clips::ok)
    {
        return err;
    }
    err = base->flag<std::string>("string", "", "", "string type");
    if (err != clips::ok)
    {
        return err;
    }
    base->bind(base_handler);

    auto custom = clips::make_cmd("custom");
    custom->brief("test custom types");
    custom->desc("test custom types.");
    err = custom->flag<custom_t>("custom", "", {}, "custom_t type");
    if (err != clips::ok)
    {
        return err;
    }
    custom->example("test custom --name=value");
    custom->bind(custom_handler);

    err = test->bind(base);
    if (err != clips::ok)
    {
        return err;
    }
    err = test->bind(custom);
    if (err != clips::ok)
    {
        return err;
    }

    return clips::bind(test);
}