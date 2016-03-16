#include "pmr/detail/memblocks.h"
#include "pmr/memory_resource.h"
#include <limits>
#include <new>

namespace pmr
{
    namespace detail
    {
        void*
        memblocks::extend(std::size_t bytes, memory_resource& upstream)
        {
            if(std::numeric_limits<std::size_t>::max() - sizeof(header) < bytes)
            {
                throw std::bad_alloc();
            }
            void* ptr = upstream.allocate(bytes + sizeof(header));
            header* hdr = ::new (ptr) header();
            hdr->size = bytes + sizeof(header);
            m_tail->next = hdr;
            m_tail = hdr;
            return reinterpret_cast<char*>(ptr) + sizeof(header);
        }


        void
        memblocks::release(memory_resource& upstream)
        {
            header* hdr = m_slist.next;
            while(hdr)
            {
                std::size_t bytes = hdr->size;
                header* ptr = hdr;
                hdr = hdr->next;
                upstream.deallocate(ptr, bytes, alignof(std::max_align_t));
            }
            m_slist = header{};
        }
    }
}
