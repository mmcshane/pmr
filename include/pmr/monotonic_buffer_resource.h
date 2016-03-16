#pragma once

#include "pmr/memory_resource.h"
#include "pmr/detail/memblocks.h"

namespace pmr
{
    //! A memory_resource implementation that allocates from a buffer
    //! and doesn't really deallocate the memory until it is destructed.
    //! This concept is sometimes know as a linear allocator.
    //! This class can be constructed with or without an initial buffer
    //! and will always have and "upstream" memory_resource instance from
    //! which instances will request additional memory when the current
    //! buffer is exhausted.
    //!
    //! Allocations from objects of this class are not threadsafe but are
    //! very fast when they don't have to pull from upstream. Deallocations
    //! using this class are no-ops. Requests for memory from the upstream
    //! memory_resource grow geometrically.
    class monotonic_buffer_resource : public memory_resource
    {
      public:

        //! Create a monotonic_buffer_resource with no initial buffer that will
        //! use the result of pmr::get_default_resource() as its upstream
        //! memory_resource
        monotonic_buffer_resource() noexcept;

        //! Creates a monotonic_buffer_resource that uses the supplied
        //! memory_resource as its upstream resource.
        //!
        //! \param upstream the upstream memory_resource
        explicit monotonic_buffer_resource(memory_resource* upstream) noexcept;

        //! Creates a monotonic_buffer_resource that will use the larger of an
        //! internal initial buffer size and the buffer size specified here
        //! as the size of the initial buffer requested from the upstream
        //! resource. In this case since upstream is unspecified, upstream will
        //! be the result of calling pmr::get_default_resource()
        explicit monotonic_buffer_resource(std::size_t initial_size) noexcept;

        //! Creates a monotonic_buffer_resource that will use the larger of
        //! an internal initial buffer size and the buffer size specified here
        //! as the size of the initial buffer requested from the provided
        //! upstream memory_resource
        monotonic_buffer_resource(std::size_t initial_size,
                memory_resource* upstream) noexcept;

        //! Creates a monotonic_buffer_resource that will used the supplied
        //! buffer as its initial buffer. Upon exhaustion of this initial
        //! buffer, further memory will be allocated from the result of calling
        //! pmr::get_upstream_resource()
        monotonic_buffer_resource(void* buffer,
                std::size_t buffer_size) noexcept;

        //! Creates a monotonic_buffer_resouce that will use the supplied
        //! buffer as its initial buffer and will allocate from the supplied
        //! upstream resource upon exhaustion of the initial buffer.
        monotonic_buffer_resource(void* buffer, std::size_t buffer_size,
                          memory_resource* upstream) noexcept;

        monotonic_buffer_resource(const monotonic_buffer_resource&) = delete;

        //! Calls release()
        ~monotonic_buffer_resource();

        monotonic_buffer_resource& operator=(
                const monotonic_buffer_resource&) = delete;

        //! Deallocate all blocks of memory allocated from upstream
        void release();

        //! Get this instance's upstream memory_resource
        memory_resource* upstream_resource() const;

      private:
        void* do_allocate(std::size_t bytes, std::size_t align) override;
        void do_deallocate(void*, std::size_t, std::size_t) override;
        bool do_is_equal(const memory_resource& other) const override;
        void recalculate_next_buffer_size();

        memory_resource& m_upstream;
        void* m_currentbuf;
        std::size_t m_currentbuf_size;
        std::size_t m_nextbuf_size;
        detail::memblocks m_blocks;
    };
}
