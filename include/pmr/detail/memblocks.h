#pragma once

#include <cstdint>

namespace pmr
{
    class memory_resource;

    namespace detail
    {
        struct header
        {
            std::size_t size = 0; //sizeof(header) + block size
            header* next = nullptr;
        };


        class memblocks
        {
          public:
            void* extend(std::size_t bytes, memory_resource& upstream);
            void release(memory_resource& upstream);

          private:
            header m_slist;
            header* m_tail = &m_slist;
        };
    }
}
