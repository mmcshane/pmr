#pragma once

#include "pmr/memory_resource.h"
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
    }


    //! Adapts any allocator type to the memory_resource interface
    //!
    //! \tparam Alloc The type of allocator to adapt
    template <typename Alloc>
    class resource_adapter_impl : public memory_resource
    {
      public:
        using allocator_type = Alloc;

        //! Instantiate and use a default-constructed instance of allocator_type
        resource_adapter_impl();

        resource_adapter_impl(const resource_adapter_impl&) = default;
        resource_adapter_impl(resource_adapter_impl&&) = default;

        //! Instantiate specifying an allocator instance
        //!
        //! \param alloc an instance of the Allocator concept.
        explicit resource_adapter_impl(const allocator_type& alloc);


        //! Instantiate specifying an allocator instance
        //!
        //! \param alloc an instance of the Allocator concept.
        explicit resource_adapter_impl(allocator_type&& alloc);

        resource_adapter_impl& operator=(
                const resource_adapter_impl&) = default;

        //! Observe the allocator instance wrapped by this adapter
        //!
        //! \returns An instance of allocator_type by-value
        allocator_type get_allocator() const;

      protected:
        //! Allocates memory using this instances instance of allocator_type
        //!
        //! \param bytes The number of bytes to allocate
        //! \param align The required alignment of the returned pointer
        //! \returns A pointer to a suitable aligned block of memory with a size
        //!          at least bytes
        void* do_allocate(std::size_t bytes, std::size_t align);

        //! Deallocates memory previously allocated by this instance
        //!
        //! \param ptr A pointer to the memory to deallocate
        //! \param bytes The number of bytes in the block to deallocatae
        //! \param align The alignement of the block indicated by ptr
        void do_deallocate(void* ptr, std::size_t bytes, std::size_t align);

        //! Test that this instance is equal to another memory_resource instance
        //!
        //! returns true iff other can by dynamic_cast to resource_adapter_impl and
        //!         the allocator wrapped by other is equal to (in the
        //!         operator== sense) the one used by this instance; false
        //!         otherwise
        bool do_is_equal(const memory_resource& other) const;

      private:
        using traits = std::allocator_traits<allocator_type>;

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
        using aligner = detail::aligned<align>;
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
        using aligner = detail::aligned<align>;
        using aligner_traits =
            typename traits::template rebind_traits<aligner>;

        std::size_t n = (bytes + align - 1) / align;
        typename aligner_traits::allocator_type alloc(m_alloc);
        return aligner_traits::deallocate(alloc,
                reinterpret_cast<typename aligner_traits::pointer>(ptr), n);
    }


    //! \class pmr::resource_adapter
    //! This alias template rebinds the Alloc to the char type such that
    //! specializations of the same allocator type always yield the same type
    //!
    //! \tparam Alloc The allocator type to adapt
    //! \sa pmr::resource_adapter_impl
    template <typename Alloc>
    using resource_adapter = resource_adapter_impl<
        typename std::allocator_traits<Alloc>::template rebind_alloc<char>>;
}
