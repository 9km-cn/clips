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

#include "catch.hpp"

TEST_CASE("utils", "utils")
{
    SECTION("starts_with")
    {
        REQUIRE(clips::utils::starts_with("/abc", "/"));
        REQUIRE(!clips::utils::starts_with("abc", "/"));
        REQUIRE(clips::utils::starts_with("/abc", "/a"));
        REQUIRE(!clips::utils::starts_with("", "/a"));
        REQUIRE(clips::utils::starts_with("/abc", "/abc"));
        REQUIRE(clips::utils::starts_with("/abc", ""));
    }

    SECTION("ends_with")
    {
        REQUIRE(clips::utils::ends_with("abc/", "/"));
        REQUIRE(!clips::utils::ends_with("abc", "/"));
        REQUIRE(clips::utils::ends_with("abc/", "c/"));
        REQUIRE(!clips::utils::ends_with("", "/a"));
        REQUIRE(clips::utils::ends_with("/abc", "/abc"));
        REQUIRE(clips::utils::ends_with("/abc", ""));
    }

    SECTION("trim")
    {
        REQUIRE(clips::utils::trim("abc/", "/") == "abc");
        REQUIRE(clips::utils::trim("abc", "/") == "abc");
        REQUIRE(clips::utils::trim("/abc/", "/") == "abc");
        REQUIRE(clips::utils::trim("", "/") == "");
        REQUIRE(clips::utils::trim("    abc    ", " ") == "abc");
    }

    SECTION("trim_left")
    {
        REQUIRE(clips::utils::trim_left("abc/", "/") == "abc/");
        REQUIRE(clips::utils::trim_left("abc", "/") == "abc");
        REQUIRE(clips::utils::trim_left("/abc/", "/") == "abc/");
        REQUIRE(clips::utils::trim_left("", "/") == "");
        REQUIRE(clips::utils::trim_left("    abc", " ") == "abc");
    }

    SECTION("split")
    {
        std::vector<std::string> list;
        clips::utils::split(list, "a b c");
        REQUIRE(list.size() == 3);
        REQUIRE(list[0] == "a");
        REQUIRE(list[1] == "b");
        REQUIRE(list[2] == "c");
    }

    SECTION("filename")
    {
        REQUIRE(clips::utils::filename("/abc") == "abc");
        REQUIRE(clips::utils::filename("abc/") == "");
        REQUIRE(clips::utils::filename("/abc/") == "");
        REQUIRE(clips::utils::filename("/abc/def") == "def");
    }

    SECTION("pading_left")
    {
        REQUIRE(clips::utils::pading_left("abc", 6, " ") == "   abc");
        REQUIRE(clips::utils::pading_left("", 6, " ") == "      ");
        REQUIRE(clips::utils::pading_left("abc", 2, " ") == "abc");
        REQUIRE(clips::utils::pading_left("abc", 3, " ") == "abc");
    }

    SECTION("pading_right")
    {
        REQUIRE(clips::utils::pading_right("abc", 6, " ") == "abc   ");
        REQUIRE(clips::utils::pading_right("", 6, " ") == "      ");
        REQUIRE(clips::utils::pading_right("abc", 2, " ") == "abc");
        REQUIRE(clips::utils::pading_right("abc", 3, " ") == "abc");
    }
}