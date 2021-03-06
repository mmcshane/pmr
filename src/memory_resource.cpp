#include "pmr/memory_resource.h"
#include "pmr/resource_adapter.h"
#include <atomic>
#include <cassert>
#include <new>
#include <type_traits>

namespace pmr
{
    memory_resource::~memory_resource()
    {
    }


    void*
    memory_resource::allocate(std::size_t bytes, std::size_t align)
    {
        assert(align > 0);
        assert(!(align & (align - 1)));
        assert(align <= alignof(std::max_align_t));

        return 0 == bytes ? nullptr : do_allocate(bytes, align);
    }


    void
    memory_resource::deallocate(void* ptr, std::size_t bytes, std::size_t align)
    {
        return do_deallocate(ptr, bytes, align);
    }


    bool
    memory_resource::is_equal(const memory_resource& other) const noexcept
    {
        return do_is_equal(other);
    }


    bool operator==(const memory_resource& lhs,
            const memory_resource& rhs) noexcept
    {
        return &lhs == &rhs || lhs.is_equal(rhs);
    }


    bool operator!=(const memory_resource& lhs,
            const memory_resource& rhs) noexcept
    {
        return !(lhs == rhs);
    }


    namespace
    {
        class null_mem : public memory_resource
        {
          public:
            void* do_allocate(std::size_t, std::size_t)
            {
                throw std::bad_alloc();
            }


            void do_deallocate(void*, std::size_t, std::size_t)
            {
            }


            bool do_is_equal(const memory_resource& other) const noexcept
            {
                return this == &other;
            }
        };
    }

    memory_resource* new_delete_resource() noexcept
    {
        static resource_adapter<std::allocator<char>> mr;
        return &mr;
    }


    memory_resource* null_memory_resource() noexcept
    {
        static null_mem mr;
        return &mr;
    }


    namespace
    {
        std::atomic<memory_resource*> default_mem { new_delete_resource() };
    }


    memory_resource* get_default_resource() noexcept
    {
        return default_mem.load(std::memory_order_acquire);
    }


    memory_resource* set_default_resource(memory_resource* mr) noexcept
    {
        return default_mem.exchange(
                mr ? mr : new_delete_resource(), std::memory_order_release);
    }


}
