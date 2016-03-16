#pragma once

#include <unordered_map>
#include "polymorphic_allocator.h"

namespace pmr
{
    template <typename K,
              typename V,
              typename Hash = std::hash<K>,
              typename Pred = std::equal_to<K>>
    using unordered_map =
        std::unordered_map<K, V, Hash, Pred,
            polymorphic_allocator<std::pair<const K, V>>;


    template <typename K,
              typename V,
              typename Hash = std::hash<K>,
              typename Pred = std::equal_to<K>>
    using unordered_multimap =
        std::unordered_multimap<K, V, Hash, Pred,
            polymorphic_allocator<std::pair<const K, V>>;
}
