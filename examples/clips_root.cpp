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

static uint32_t g_global = 0;

clips::error_t root(const clips::pcmd_t& pcmd, const clips::args_t& args)
{
    std::cout << "exec root handler" << std::endl;

    std::cout << " args{";
    for (auto& item : args)
    {
        std::cout << item << ", ";
    }
    std::cout << "}" << std::endl;

    std::cout << " flags{ global=" << g_global 
        << ", extend=" << pcmd->cast<uint32_t>("-e")
        << "}" << std::endl;

    return clips::ok;
}

CLIPS_INIT()
{
    clips::pflag<uint32_t>(&g_global, "global", "g", 0, "bind a global var.");
    clips::flag<uint32_t>("extend", "e", 1, "extend flag from root", true);

    return clips::bind(root);
}