#pragma once

#include "pmr/memory_resource.hpp"
#include <memory>
#include <utility>

namespace pmr
{
    namespace detail
    {

        template <std::size_t align> struct aligned{};
        template <> struct alignas(2)  aligned<2> { char _[ 2]; };
        template <> struct alignas(4)  aligned<4> { char _[ 4]; };
        template <> struct alignas(8)  aligned<8> { char _[ 8]; };
        template <> struct alignas(16) aligned<16>{ char _[16]; };
        template <> struct alignas(32) aligned<32>{ char _[32]; };
        template <> struct alignas(64) aligned<64>{ char _[64]; };


        template <typename Alloc>
        class resource_adapter_impl : public memory_resource
        {
          public:
            using allocator_type = Alloc;

            resource_adapter_impl();
            resource_adapter_impl(const resource_adapter_impl&) = default;
            resource_adapter_impl(resource_adapter_impl&&) = default;
            explicit resource_adapter_impl(const allocator_type& alloc);
            explicit resource_adapter_impl(allocator_type&& alloc);

            resource_adapter_impl& operator=(
                    const resource_adapter_impl&) = default;

            allocator_type get_allocator() const;

          protected:
            void* do_allocate(std::size_t bytes, std::size_t align);
            void do_deallocate(void* ptr, std::size_t bytes, std::size_t align);
            bool do_is_equal(const memory_resource& other) const;

          private:
            using traits = std::allocator_traits<allocator_type>;
            static_assert(
                std::is_same<typename traits::value_type, char>::value,
                "resource_adapter_imp only instantiable with Allocator<char>");

            template <std::size_t align>
            void* do_allocate(std::size_t bytes);

            template <std::size_t align>
            void do_deallocate(void* ptr, std::size_t bytes);

            Alloc m_alloc;
        };


        template <typename Alloc>
        resource_adapter_impl<Alloc>::resource_adapter_impl()
        {
        }


        template <typename Alloc>
        resource_adapter_impl<Alloc>::resource_adapter_impl(
                const allocator_type& alloc)
            : m_alloc{alloc}
        {
        }


        template <typename Alloc>
        resource_adapter_impl<Alloc>::resource_adapter_impl(
                allocator_type&& alloc)
            : m_alloc{std::move(alloc)}
        {
        }


        template <typename Alloc>
        typename resource_adapter_impl<Alloc>::allocator_type
        resource_adapter_impl<Alloc>::get_allocator() const
        {
            return m_alloc;
        }


        template <typename Alloc>
        void*
        resource_adapter_impl<Alloc>::do_allocate(
                std::size_t bytes, std::size_t align)
        {
            switch(align)
            {
                case  2: return do_allocate<2>(bytes);
                case  4: return do_allocate<4>(bytes);
                case  8: return do_allocate<8>(bytes);
                case 16: return do_allocate<16>(bytes);
                case 32: return do_allocate<32>(bytes);
                case 64: return do_allocate<64>(bytes);
                default: throw std::bad_alloc();
            }
        }


        template <typename Alloc>
        void
        resource_adapter_impl<Alloc>::do_deallocate(
                void* ptr, std::size_t bytes, std::size_t align)
        {
            switch(align)
            {
                case  2: return do_deallocate<2>(ptr, bytes);
                case  4: return do_deallocate<4>(ptr, bytes);
                case  8: return do_deallocate<8>(ptr, bytes);
                case 16: return do_deallocate<16>(ptr, bytes);
                case 32: return do_deallocate<32>(ptr, bytes);
                case 64: return do_deallocate<64>(ptr, bytes);
                default: throw std::bad_alloc();
            }
        }


        template <typename Alloc>
        bool
        resource_adapter_impl<Alloc>::do_is_equal(
                const memory_resource& other) const
        {
            if(const resource_adapter_impl* p =
                    dynamic_cast<const resource_adapter_impl*>(&other))
            {
                return m_alloc == p->m_alloc;
            }
            return false;
        }


        template <typename Alloc>
        template <std::size_t align>
        void*
        resource_adapter_impl<Alloc>::do_allocate(std::size_t bytes)
        {
            using aligner = aligned<align>;
            using aligner_traits =
                typename traits::template rebind_traits<aligner>;

            std::size_t n = (bytes + align - 1) / align;
            typename aligner_traits::allocator_type alloc(m_alloc);
            return aligner_traits::allocate(alloc, n);
        }


        template <typename Alloc>
        template <std::size_t align>
        void
        resource_adapter_impl<Alloc>::do_deallocate(void* ptr, std::size_t bytes)
        {
            using aligner = aligned<align>;
            using aligner_traits =
                typename traits::template rebind_traits<aligner>;

            std::size_t n = (bytes + align - 1) / align;
            typename aligner_traits::allocator_type alloc(m_alloc);
            return aligner_traits::deallocate(alloc,
                    reinterpret_cast<typename aligner_traits::pointer>(ptr), n);
        }
    }


    template <typename Allocator>
    using resource_adapter = detail::resource_adapter_impl<
        typename std::allocator_traits<Allocator>::template rebind_alloc<char>>;
}
