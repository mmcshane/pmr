#include "pmr/monotonic_buffer_resource.h"
#include "pmr/memory_resource.h"
#include "tracking_memory_resource.h"
#include <catch.hpp>
#include <limits>
#include <new>

namespace
{
    const char* tags = "[pmr][monotonic_buffer_resource]";
}

TEST_CASE_METHOD(use_tracking_default, "alloc from upstream", tags)
{
    {
        pmr::monotonic_buffer_resource mbr;
        REQUIRE(mbr.upstream_resource() == &tracked_memory);

        void* ptr = mbr.allocate(1024);
        REQUIRE(tracked_memory.allocations.size());
        CHECK(tracked_memory.allocations[0] >= 1024);

        CHECK_NOTHROW(mbr.deallocate(ptr, 1024));
        //no real deallocations until mbr goes out of scope
        CHECK_FALSE(tracked_memory.all_memory_deallocated());
    }
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE_METHOD(use_tracking_default, "huge alloc throws", tags)
{
    pmr::monotonic_buffer_resource mbr;
    CHECK_THROWS_AS(mbr.allocate(
                std::numeric_limits<std::size_t>::max()), std::bad_alloc);
}


TEST_CASE_METHOD(use_tracking_default, "satisfy from provided buffer first", tags)
{
    char buf[sizeof(char)];
    pmr::monotonic_buffer_resource mbr{buf, sizeof(char)};

    void* ptr = mbr.allocate(sizeof(char), alignof(char));
    CHECK(tracked_memory.allocations.empty());
    CHECK(ptr == &buf);
    mbr.deallocate(ptr, sizeof(char));

    void* ptr2 = mbr.allocate(sizeof(char));
    CHECK(1 == tracked_memory.allocations.size());
    mbr.release();
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE_METHOD(use_tracking_default, "release deallocates upstream", tags)
{
    pmr::monotonic_buffer_resource mbr;
    void* ptr = mbr.allocate(10);
    mbr.deallocate(ptr, 10);
    CHECK(1 == tracked_memory.allocations.size());
    REQUIRE(0 == tracked_memory.deallocations.size());

    mbr.release();
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE("mbr equality", tags)
{
    pmr::monotonic_buffer_resource mbr1;
    pmr::monotonic_buffer_resource mbr2;

    CHECK(mbr1 == mbr1);
    CHECK(mbr1 != mbr2);
}


TEST_CASE_METHOD(use_tracking_default, "override tiny default sizes", tags)
{
    pmr::monotonic_buffer_resource mbr{1};
    mbr.allocate(10);
    REQUIRE(1 == tracked_memory.allocations.size());
    CHECK(tracked_memory.allocations[0] > 1);
    mbr.release();
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE_METHOD(use_tracking_default, "geometric growth", tags)
{
    pmr::monotonic_buffer_resource mbr;
    mbr.allocate(100);
    REQUIRE(1 == tracked_memory.allocations.size());
    std::size_t mbr_current_buffer_size = tracked_memory.allocations[0];
    mbr.allocate(mbr_current_buffer_size + 100);
    REQUIRE(2 == tracked_memory.allocations.size());

    //should be roughly 2x ignoring bytes used for alignment
    CHECK(tracked_memory.allocations[1] > 1.5 * tracked_memory.allocations[0]);
    mbr.release();
    INFO(tracked_memory);
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE_METHOD(use_tracking_default,
        "allocate more than next geometric step", tags)
{
    pmr::monotonic_buffer_resource mbr;
    mbr.allocate(1024 * 1024); // 1MiB is > the default alloc from upstram
    mbr.allocate(1024); // causes 2nd alloc from upstream
    REQUIRE(2 == tracked_memory.allocations.size());

    //should be roughly 2x ignoring bytes used for alignment
    CHECK(tracked_memory.allocations[1] > 1.5 * tracked_memory.allocations[0]);
    mbr.release();
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE_METHOD(use_tracking_default, "multiple allocations no upstream", tags)
{
    char initbuf[1024] = {0};
    std::uintptr_t bufstart = reinterpret_cast<std::uintptr_t>(initbuf);
    std::uintptr_t bufend = bufstart + 1024;
    pmr::monotonic_buffer_resource mbr{initbuf, 1024};
    for(int i = 0; i < 10; i++)
    {
        std::uintptr_t ptrval = reinterpret_cast<std::uintptr_t>(
                mbr.allocate(sizeof(int), alignof(int)));
        CHECK(ptrval >= bufstart);
        CHECK(ptrval < bufend);
    }
    CHECK(tracked_memory.allocations.empty());
}
