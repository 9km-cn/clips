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

clips::error_t sub_handler(const clips::pcmd_t& cmd, const clips::args_t& args, const clips::flags_t& flags)
{
    std::cout << "exec sub handler" << std::endl;

    std::cout << " args{";
    for (auto& item : args)
    {
        std::cout << item << ", ";
    }
    std::cout << "}" << std::endl;

    std::cout << " flags{extend=" << clips::cast<uint32_t>("-e")
        << "}" << std::endl;

    return clips::ok;
}

clips::error_t nested_handler(const clips::pcmd_t& cmd, const clips::args_t& args, const clips::flags_t& flags)
{
    std::cout << "exec sub nested handler" << std::endl;

    std::cout << " args{";
    for (auto& item : args)
    {
        std::cout << item << ", ";
    }
    std::cout << "}" << std::endl;

    std::cout << " flags{extend=" << clips::cast<uint32_t>("-e")
        << "}" << std::endl;

    return clips::ok;
}

clips::error_t leaf_handler(const clips::pcmd_t& cmd, const clips::args_t& args, const clips::flags_t& flags)
{
    std::cout << "exec sub nested leaf handler" << std::endl;

    std::cout << " args{";
    for (auto& item : args)
    {
        std::cout << item << ", ";
    }
    std::cout << "}" << std::endl;

    std::cout << " flags{extend=" << clips::cast<uint32_t>("-e")
        << ", num=" << clips::cast<int>("--num")
        << ", enum=" << clips::cast<int>("--enum")
        << "}" << std::endl;

    return clips::ok;
}

CLIPS_INIT()
{
    // sub
    auto sub = clips::make_cmd("sub", "subcommand", "subcommand and have nested command");
    sub->example("sub");
    sub->bind(sub_handler);

    // nested
    auto nested = clips::make_cmd("nested", "a nested command", "a nested command");
    nested->example("sub nested");
    nested->bind(nested_handler);

    // leaf
    auto leaf = clips::make_cmd("leaf", "a leaf command", "a leaf command");

    auto err = leaf->flag<int>("num", "n", 1, "num desc");
    if (err != clips::ok)
    {
        return err;
    }
    err = leaf->flag<int>("enum", "", 2, { 0, 1, 2 }, "enum desc", false);
    if (err != clips::ok)
    {
        return err;
    }

    leaf->example("sub nested leaf example");
    leaf->bind(leaf_handler);

    err = nested->bind(leaf);
    if (err != clips::ok)
    {
        return err;
    }
    err = sub->bind(nested);
    if (err != clips::ok)
    {
        return err;
    }

    return clips::bind(sub);
}