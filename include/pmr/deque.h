#pragma once

#include <deque>
#include "polymorphic_allocator.h"

namespace pmr
{
    template <typename T>
    using deque = std::deque<T, polymorphic_allocator<T>>;
}
