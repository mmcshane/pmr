#include "pmr/memory_resource.h"
#include "tracking_memory_resource.h"
#include <catch.hpp>
#include <new>


TEST_CASE("null resource", "[pmr]")
{
    const char* undeletable = "xyz";
    pmr::memory_resource* mr = pmr::null_memory_resource();

    CHECK_THROWS_AS(mr->allocate(1), std::bad_alloc);
    CHECK_NOTHROW(mr->deallocate(const_cast<char*>(undeletable), 4));
    CHECK(*mr == *pmr::null_memory_resource());
}


TEST_CASE("new delete resource", "[pmr]")
{
    pmr::memory_resource* mr = pmr::get_default_resource();
    CHECK(nullptr == mr->allocate(0));
    char* bytes = (char*) mr->allocate(3);
    bytes[0] = 'a';
    bytes[1] = 'b';
    bytes[2] = 'c';
    CHECK_NOTHROW(mr->deallocate(bytes, 3));
    CHECK(*mr == *pmr::get_default_resource());
    CHECK(mr == pmr::get_default_resource());
}


TEST_CASE("replace default resource", "[pmr]")
{
    tracking_memory_resource test_mr{pmr::get_default_resource()};

    pmr::memory_resource* before = pmr::set_default_resource(&test_mr);
    pmr::memory_resource* after = pmr::get_default_resource();

    CHECK(before != after);
    CHECK(after == &test_mr);

    void* ptr = pmr::get_default_resource()->allocate(10);
    pmr::get_default_resource()->deallocate(ptr, 10);

    REQUIRE(1 == test_mr.allocations.size());
    CHECK(10 == test_mr.allocations[0]);

    REQUIRE(1 == test_mr.deallocations.size());
    CHECK(10 == test_mr.deallocations[0]);

    pmr::set_default_resource(nullptr);
    CHECK(before == pmr::get_default_resource());
}
