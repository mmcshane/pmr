#include "pmr/monotonic_buffer_resource.hpp"
#include <algorithm>
#include <memory>

namespace pmr
{
    namespace
    {
        std::size_t default_nextbuf_size = 32 * sizeof(void*);
    }


    monotonic_buffer_resource::monotonic_buffer_resource() noexcept
        : monotonic_buffer_resource(default_nextbuf_size, nullptr)
    {
    }


    monotonic_buffer_resource::monotonic_buffer_resource(
            memory_resource* upstream) noexcept
        : monotonic_buffer_resource(default_nextbuf_size, upstream)
    {
    }


    monotonic_buffer_resource::monotonic_buffer_resource(
            std::size_t initial_size) noexcept
        : monotonic_buffer_resource(
                std::max(initial_size, default_nextbuf_size), nullptr)
    {
    }


    monotonic_buffer_resource::monotonic_buffer_resource(
            std::size_t initial_size, memory_resource* upstream) noexcept
        : m_upstream{upstream ? *upstream : *get_default_resource()}
        , m_currentbuf{nullptr}
        , m_currentbuf_size{0}
        , m_nextbuf_size{std::max(initial_size, default_nextbuf_size)}
    {
    }


    monotonic_buffer_resource::monotonic_buffer_resource(
            void* buf, std::size_t bufsize) noexcept
        : monotonic_buffer_resource(buf, bufsize, nullptr)
    {
    }


    monotonic_buffer_resource::monotonic_buffer_resource(
            void* buf, std::size_t bufsize, memory_resource* upstream) noexcept
        : m_upstream{upstream ? *upstream : *get_default_resource()}
        , m_currentbuf{buf}
        , m_currentbuf_size{bufsize}
        , m_nextbuf_size{std::max(bufsize, default_nextbuf_size)}
    {
        recalculate_next_buffer_size();
    }


    monotonic_buffer_resource::~monotonic_buffer_resource()
    {
        release();
    }


    void
    monotonic_buffer_resource::release()
    {
        m_blocks.release(m_upstream);
    }


    memory_resource*
    monotonic_buffer_resource::upstream_resource() const
    {
        return &m_upstream;
    }


    void*
    monotonic_buffer_resource::do_allocate(std::size_t bytes, std::size_t align)
    {
        void* allocated = std::align(align, bytes, m_currentbuf, m_currentbuf_size);
        if(!allocated)
        {
            m_nextbuf_size = std::max(m_nextbuf_size, bytes + align);
            m_currentbuf = m_blocks.extend(m_nextbuf_size, m_upstream);
            m_currentbuf_size = m_nextbuf_size;
            recalculate_next_buffer_size();
            allocated = std::align(align, bytes, m_currentbuf, m_currentbuf_size);
        }
        if(!allocated)
        {
            throw std::bad_alloc();
        }
        m_currentbuf = reinterpret_cast<char*>(m_currentbuf) + bytes;
        m_currentbuf_size -= bytes;
        return allocated;
    }


    void
    monotonic_buffer_resource::recalculate_next_buffer_size()
    {
        m_nextbuf_size = (std::size_t(-1)/2 < m_nextbuf_size)
            ? std::size_t(-1) : m_currentbuf_size * 2;
    }


    void
    monotonic_buffer_resource::do_deallocate(void*, std::size_t, std::size_t)
    {
    }


    bool
    monotonic_buffer_resource::do_is_equal(const memory_resource& other) const
    {
        return this == &other;
    }
}
