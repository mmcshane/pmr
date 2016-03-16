#pragma once

#include <set>
#include "polymorphic_allocator.h"

namespace pmr
{
    template <typename K, typename Comp = std::less<K>>
    using set = std::set<K, Comp, polymorphic_allocator<K>>;


    template <typename K, typename Comp = std::less<K>>
    using multiset = std::set<K, Comp, polymorphic_allocator<K>>;
}
