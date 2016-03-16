#pragma once

#include <regex>
#include <string>
#include "polymorphic_allocator.h"

namespace pmr
{
    template <typename Iter>
    using match_results =
        std::match_results<Iter, polymorphic_allocator<std::sub_match<Iter>>;


    using cmatch = match_results<const char*>;
    using wcmatch = match_results<const wchar_t*>;
    using smatch = match_results<std::string::const_iterator>;
    using wsmatch = match_results<std::wstring::const_iterator>;
}
