#pragma once

#include <string>
#include "polymorphic_allocator.h"

namespace pmr
{
    template <typename CharT, typename Traits = std::char_traits<CharT>>
    using basic_string =
        std::basic_string<CharT, Traits, polymorphic_allocator<CharT>>;

    using string = basic_string<char>;
    using u16string = basic_string<char16_t>;
    using u32string = basic_string<char32_t>;
    using wstring = basic_string<wchar_t>;
}
