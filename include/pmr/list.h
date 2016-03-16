#pragma once

#include <list>
#include "polymorphic_allocator.h"

namespace pmr
{
    template <typename T>
    using list = std::list<T, polymorphic_allocator<T>>;
}
