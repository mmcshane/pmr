#pragma once

#include <cstdint>

namespace pmr
{
    //! Defines hints for sizing the pools in pool-based memory_resource
    //! instances.
    //!
    //! \sa pmr::synchronized_pool_resource
    //! \sa pmr::unsynchronized_pool_resource
    struct pool_options
    {
        std::size_t max_blocks_per_chunk = 0;
        std::size_t largest_required_pool_block = 0;
    };
}
