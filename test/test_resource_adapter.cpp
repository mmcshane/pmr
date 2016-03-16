#include "pmr/resource_adapter.h"
#include "pmr/polymorphic_allocator.h"
#include "pmr/monotonic_buffer_resource.h"
#include "tracking_memory_resource.h"
#include <catch.hpp>


TEST_CASE_METHOD(use_tracking_default, "allocate larger than alignment", "[pmr]")
{
    pmr::resource_adapter<pmr::polymorphic_allocator<char>> resource;
    REQUIRE(16 == alignof(std::max_align_t));
    for(std::size_t align : {2, 4, 8, 16})
    {
        void* mem = resource.allocate(128, align);
        REQUIRE(tracked_memory.allocations.size());
        CHECK(tracked_memory.allocations.back() >= 128);
        CHECK(tracked_memory.allocations.back() <= 128 + align);
        std::uintptr_t ptrval = reinterpret_cast<std::uintptr_t>(mem);
        CHECK(0 == ptrval % align);
        resource.deallocate(mem, 128, align);
    }
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE_METHOD(use_tracking_default, "allocate smaller than alignment", "[pmr]")
{
    pmr::resource_adapter<pmr::polymorphic_allocator<char>> resource;
    REQUIRE(16 == alignof(std::max_align_t));
    for(std::size_t align : {2, 4, 8, 16})
    {
        void* mem = resource.allocate(1, align);
        REQUIRE(tracked_memory.allocations.size());
        CHECK(tracked_memory.allocations.back() >= 1);
        CHECK(tracked_memory.allocations.back() <= 1 + align);
        std::uintptr_t ptrval = reinterpret_cast<std::uintptr_t>(mem);
        CHECK(0 == ptrval % align);
        resource.deallocate(mem, 1, align);
    }
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE("resource_adapter equality", "[pmr]")
{
    pmr::monotonic_buffer_resource mbr;
    pmr::resource_adapter<pmr::polymorphic_allocator<char>> resource1;
    pmr::resource_adapter<pmr::polymorphic_allocator<char>> resource2;
    pmr::resource_adapter<pmr::polymorphic_allocator<char>> resource3{&mbr};

    CHECK(resource1 == resource2);
    CHECK(resource1 != resource3);
}
