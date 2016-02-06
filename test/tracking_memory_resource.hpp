#pragma once

#include "pmr/memory_resource.hpp"
#include <algorithm>
#include <vector>


class tracking_memory_resource : public pmr::memory_resource
{
  public:
    std::vector<std::size_t> allocations{};
    std::vector<std::size_t> deallocations{};

    tracking_memory_resource(pmr::memory_resource* delegate)
        : m_delegate{delegate}
    {
    }

    void* do_allocate(std::size_t bytes, std::size_t align) override
    {
        allocations.push_back(bytes);
        return m_delegate->allocate(bytes, align);
    }


    void do_deallocate(void* ptr, std::size_t bytes, std::size_t align) override
    {
        deallocations.push_back(bytes);
        m_delegate->deallocate(ptr, bytes, align);
    }


    bool do_is_equal(const memory_resource& other) const noexcept override
    {
        return m_delegate->is_equal(other);
    }

    bool all_memory_deallocated()
    {
        return std::accumulate(begin(allocations), end(allocations), 0) ==
            std::accumulate(begin(deallocations), end(deallocations), 0);
    }

  private:
    pmr::memory_resource* m_delegate;
};


struct use_tracking_default
{
    use_tracking_default()
        : tracked_memory{pmr::get_default_resource()}
    {
        pmr::set_default_resource(&tracked_memory);
    }

    ~use_tracking_default()
    {
        pmr::set_default_resource(nullptr);
    }

    tracking_memory_resource tracked_memory;
};

