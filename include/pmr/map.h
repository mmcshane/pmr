#pragma once

#include <map>
#include "polymorphic_allocator.h"

namespace pmr
{
    template <typename K, typename V, typename Comp = std::less<K>>
    using map = std::map<K, V, Comp,
          polymorphic_allocator<std::pair<const K, V>>;


    template <typename K, typename V, typename Comp = std::less<K>>
    using multimap = std::multimap<K, V, Comp,
          polymorphic_allocator<std::pair<const K, V>>;
}
