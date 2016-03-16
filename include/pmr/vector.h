#pragma once

#include <vector>
#include "polymorphic_allocator.h"

namespace pmr
{
    template <typename T>
    using vector = std::vector<T, polymorphic_allocator<T>>;
}
