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

clips::error_t error_handler(const clips::pcmd_t& cmd, const clips::args_t& args, const clips::flags_t& flags)
{
    std::cout << "exec error_handler" << std::endl;
    return clips::make_error("exec failed.", cmd->stack());
}

CLIPS_INIT()
{
    auto cerror = clips::make_cmd("error", "command handler return error", 
        "command handler return error, it will be show an error message.");
    cerror->example("error");
    cerror->bind(error_handler);

    return clips::bind(cerror);
}