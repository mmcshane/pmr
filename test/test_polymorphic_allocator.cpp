#include "pmr/polymorphic_allocator.h"
#include "pmr/memory_resource.h"
#include "pmr/monotonic_buffer_resource.h"
#include "tracking_memory_resource.h"
#include <catch.hpp>
#include <memory>
#include <string>
#include <vector>
#include <list>

namespace
{
    const char* tags = "[pmr][polymorphic_allocator]";

    struct allocator_unaware
    {
        allocator_unaware(int v) : value{v} { }
        int value = 0;
    };


    struct allocator_aware_tagged
    {
        using allocator_type = pmr::polymorphic_allocator<int>;

        allocator_aware_tagged(std::allocator_arg_t, const allocator_type&, int v)
            : value{v} { }
        int value = 0;
    };


    struct allocator_aware_trailing
    {
        using allocator_type = pmr::polymorphic_allocator<int>;

        allocator_aware_trailing(int v, const allocator_type&)
            : value{v} { }
        int value = 0;
    };

    using unaware_traits = std::allocator_traits<
        pmr::polymorphic_allocator<allocator_unaware>>;
    using tagged_traits = std::allocator_traits<
        pmr::polymorphic_allocator<allocator_aware_tagged>>;
    using trailing_traits = std::allocator_traits<
        pmr::polymorphic_allocator<allocator_aware_trailing>>;
}


TEST_CASE("self-check allocator awareness done correctly")
{
    REQUIRE((std::uses_allocator<
            tagged_traits::value_type, pmr::memory_resource*>::value));
    REQUIRE((std::is_constructible<tagged_traits::value_type,
            std::allocator_arg_t, pmr::memory_resource*, int>::value));

    REQUIRE((std::uses_allocator<
            trailing_traits::value_type, pmr::memory_resource*>::value));
    REQUIRE((std::is_constructible<trailing_traits::value_type,
            int, pmr::memory_resource*>::value));
}


TEST_CASE_METHOD(use_tracking_default, "simple alloc/dealloc", tags)
{
    unaware_traits::allocator_type alloc;
    unaware_traits::pointer ints = unaware_traits::allocate(alloc, 5);
    CHECK(1 == tracked_memory.allocations.size());
    unaware_traits::deallocate(alloc, ints, 5);
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE_METHOD(use_tracking_default, "alloc unaware construct/destroy", tags)
{
    unaware_traits::allocator_type alloc;
    unaware_traits::pointer ptr = unaware_traits::allocate(alloc, 1);
    unaware_traits::construct(alloc, ptr, 1);
    CHECK(1 == ptr->value);
    unaware_traits::destroy(alloc, ptr);
    unaware_traits::deallocate(alloc, ptr, 1);
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE_METHOD(use_tracking_default, "tagged alloc construct/destroy", tags)
{
    tagged_traits::allocator_type alloc;
    tagged_traits::pointer ptr = tagged_traits::allocate(alloc, 1);
    tagged_traits::construct(alloc, ptr, 1);
    CHECK(1 == ptr->value);
    tagged_traits::destroy(alloc, ptr);
    tagged_traits::deallocate(alloc, ptr, 1);
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE_METHOD(use_tracking_default, "trailing alloc construct/destroy", tags)
{
    trailing_traits::allocator_type alloc;
    trailing_traits::pointer ptr = trailing_traits::allocate(alloc, 1);
    trailing_traits::construct(alloc, ptr, 1);
    CHECK(1 == ptr->value);
    trailing_traits::destroy(alloc, ptr);
    trailing_traits::deallocate(alloc, ptr, 1);
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE_METHOD(use_tracking_default, "alloc aware tuple construct/destroy", tags)
{
    using pair_t = std::pair<allocator_unaware, allocator_aware_tagged>;
    pair_t* ptr = reinterpret_cast<pair_t*>(
            pmr::get_default_resource()->allocate(sizeof(pair_t)));
    unaware_traits::allocator_type alloc;
    alloc.construct(ptr, std::move(std::make_pair(1, 1)));
    CHECK(1 == ptr->first.value);
    CHECK(1 == ptr->second.value);
    unaware_traits::destroy(alloc, ptr);
    pmr::get_default_resource()->deallocate(ptr, sizeof(pair_t));
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE_METHOD(use_tracking_default, "simple vector", tags)
{
    {
        std::vector<int, pmr::polymorphic_allocator<int>> vec;
        vec.push_back(1);
        CHECK(1 == tracked_memory.allocations.size());
    }
    CHECK(tracked_memory.all_memory_deallocated());
}


TEST_CASE_METHOD(use_tracking_default, "list of allocator-aware", tags)
{
    using string_t = std::basic_string<
        char, std::char_traits<char>, pmr::polymorphic_allocator<char>>;
    using list_t = std::list<string_t, pmr::polymorphic_allocator<string_t>>;

    char* stackbuf[1024] = {0};
    std::uintptr_t stackbuf_start = reinterpret_cast<std::uintptr_t>(stackbuf);
    std::uintptr_t stackbuf_end = stackbuf_start + 1024;

    pmr::monotonic_buffer_resource mbr{stackbuf, 1024};
    tracking_memory_resource tmr{&mbr};

    // string must be long enough to avoid small string optimization
    const char* chars = "All those moments will be lost in time… like tears in rain.";
    string_t str{chars, &tmr};
    list_t l{&tmr};
    l.push_back(str);

    // string inside the list allocated from stackbuf
    CHECK(chars == l.front());
    CHECK(reinterpret_cast<std::uintptr_t>(l.front().data()) >= stackbuf_start);
    CHECK(reinterpret_cast<std::uintptr_t>(l.front().data()) < stackbuf_end);

    // no allocations to default resource
    CHECK(tracked_memory.allocations.empty());

    // stackbuf allocations:
    //  #1 construct the string
    //  #2 construct listnode
    //  #3 copy string into listnode
    std::size_t allocations = tmr.allocations.size();
    CHECK(allocations == 3);

    list_t copy = l;

    // string inside the list copy allocated from stackbuf
    CHECK(chars == copy.front());
    CHECK(reinterpret_cast<std::uintptr_t>(copy.front().data()) >= stackbuf_start);
    CHECK(reinterpret_cast<std::uintptr_t>(copy.front().data()) < stackbuf_end);

    //unclear how many allocations the list copy causes, but it's non-zero
    CHECK(tmr.allocations.size() > allocations);

    // still no allocations from default resource
    CHECK(tracked_memory.allocations.empty());
}


