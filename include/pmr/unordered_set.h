#pragma once

#include <unordered_set>
#include "polymorphic_allocator.h"

namespace pmr
{
    template <typename K,
              typename Hash = std::hash<K>,
              typename Pred = std::equal_to<K>>
    using unordered_map =
        std::unordered_map<K, Hash, Pred, polymorphic_allocator<K>;


    template <typename K,
              typename Hash = std::hash<K>,
              typename Pred = std::equal_to<K>>
    using unordered_multimap =
        std::unordered_multimap<K, Hash, Pred, polymorphic_allocator<K>;
}
