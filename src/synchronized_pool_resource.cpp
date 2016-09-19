#include "pmr/synchronized_pool_resource.h"
#include <cassert>
#include <memory>

namespace pmr
{
    synchronized_pool_resource::synchronized_pool_resource()
        : synchronized_pool_resource{get_default_resource()}
    {
    }


    synchronized_pool_resource::synchronized_pool_resource(
            memory_resource* upstream)
        : synchronized_pool_resource{pool_options{}, upstream}
    {
    }


    synchronized_pool_resource::synchronized_pool_resource(
            const pool_options& opts, memory_resource* upstream)
        : m_delegate{opts, upstream}
    {
    }


    synchronized_pool_resource::~synchronized_pool_resource()
    {
        release();
    }


    pool_options
    synchronized_pool_resource::options() const
    {
        return m_delegate.options();
    }


    memory_resource*
    synchronized_pool_resource::upstream_resource() const
    {
        return m_delegate.upstream_resource();
    }


    void
    synchronized_pool_resource::release()
    {
        std::lock_guard<std::mutex> lck{m_mutex};
        m_delegate.release();
    }


    void*
    synchronized_pool_resource::do_allocate(
            std::size_t bytes, std::size_t align)
    {
        std::lock_guard<std::mutex> lck{m_mutex};
        return m_delegate.allocate(bytes, align);
    }


    void
    synchronized_pool_resource::do_deallocate(
            void * ptr, std::size_t bytes, std::size_t align)
    {
        std::lock_guard<std::mutex> lck{m_mutex};
        return m_delegate.deallocate(ptr, bytes, align);
    }


    bool
    synchronized_pool_resource::do_is_equal(
            const memory_resource& other) const
    {
        return this == std::addressof(other);
    }
}
