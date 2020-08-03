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

CLIPS_INIT()
{
    auto pcmd = clips::make_cmd("nonested");
    pcmd->brief("nonested subcommand");
    pcmd->desc("subcommand and has no nested command");
    pcmd->example("nonested");
    pcmd->bind([](const clips::pcmd_t& cmd, const clips::args_t& args) -> clips::error_t
        {
            std::cout << "exec nonested handler" << std::endl;

            std::cout << " args{";
            for (auto& item : args)
            {
                std::cout << item << ", ";
            }
            std::cout << "}" << std::endl;

            return clips::ok;
        }
    );

    return clips::bind(pcmd);
}