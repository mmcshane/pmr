#pragma once

#include <cstdint>
#include <cstddef>

namespace pmr
{
    //! Virtual base class for any type needing to encapsulate access
    //! to memory.
    class memory_resource
    {
      public:
        virtual ~memory_resource();

        //! Allocates uninitialized memory from this memory_resource
        //! Behavior is undefined if the specified alignment is larger than
        //! alignof(std::max_align_t). General behavior is implementation
        //! defined by overriding do_allocate(std::size_t, std::size_t)
        //!
        //! \param bytes The number of bytes to allocate
        //! \param align The alignment of the returned pointer; default
        //!              is alignof(std::max_align_t)
        //! \return an untyped pointer to a contiguous block of memory of
        //!         at least bytes in size and with alignment as specified.
        //!         The return value is nullptr if bytes is zero
        //! \throws std::bad_alloc if unable to allocate the requested bytes
        void* allocate(std::size_t bytes,
                std::size_t align = alignof(std::max_align_t));

        //! Delegates to do_deallocate(void*, std::size_t, std::size_t) to
        //! deallocate a chunk of memory allocated by this memory_resource
        //! implementation
        //!
        //! \param ptr A pointer to the first byte of the memory to deallocate
        //! \param bytes The size in bytes of the memory pointed at by ptr
        //! \param align The alignment of the block to deallocate; default
        //!              is alignof(std::max_align_t)
        void deallocate(void* ptr, std::size_t bytes,
                std::size_t align = alignof(std::max_align_t));

        //! Delegates to do_is_equal(const memory_resource*) const to
        //! determine implementation-defined equality of this memory_resource
        //!
        //! \param other Another memory_resource instance
        //! \return the value returned by do_is_equal(other)
        bool is_equal(const memory_resource& other) const noexcept;

      protected:
        //! Implementation defined behavior for allocation
        //! This function is called by the public allocate(std::size_t,
        //! std::size_t) function.
        //!
        //! \param bytes The number of bytes to allocate
        //! \param align The alignment of the returned pointer; default
        //!              is alignof(std::max_align_t)
        //! \return an untyped pointer to a contiguous block of memory of
        //!         at least bytes in size and with alignment as specified.
        //!         The return value is nullptr if bytes is zero
        //! \throws std::bad_alloc if unable to allocate the requested bytes
        virtual void* do_allocate(std::size_t bytes, std::size_t align) = 0;


        //! Deallocates a chunk of memory allocated by this memory_resource
        //!
        //! \param ptr A pointer to the first byte of the memory to deallocate
        //! \param bytes The size in bytes of the memory pointed at by ptr
        //! \param align The alignment of the block to deallocate; default
        //!              is alignof(std::max_align_t)
        virtual void do_deallocate(
                void * ptr, std::size_t bytes, std::size_t align) = 0;

        //! Indicates implementation-defined equality of this memory_resource
        //!
        //! \param other Another memory_resource instance
        //! \return the value returned by do_is_equal(other)
        virtual bool do_is_equal(const memory_resource& other) const = 0;

    };

    //! Tests memory_resource instances for equality
    //!
    //! \return true iff the address of lhs is the same as the address of rhs
    //!         AND lhs.is_equal(rhs) returns true
    //! \relates memory_resource
    bool operator==(const memory_resource& lhs,
            const memory_resource& rhs) noexcept;

    //! Tests memory_resource instances for inequality
    //!
    //! \return !(lhs == rhs)
    //! \relates memory_resource
    bool operator!=(const memory_resource& lhs,
            const memory_resource& rhs) noexcept;

    //! Access a global, threadsafe memory_resource implementation that uses
    //! global new and delete to allocate/deallocate memory
    //!
    //! \return the same pointer for every invocation
    //! \relates memory_resource
    memory_resource* new_delete_resource() noexcept;

    //! Access a global, threadsafe memory_resource implementation that
    //! always throws std::bad_alloc when it attempts to allocate and for
    //! which deallocate is a no-op
    //!
    //! \return the same pointer for every invocation
    //! \relates memory_resource
    memory_resource* null_memory_resource() noexcept;

    //! Access a global default memory_resource. The default if there has not
    //! been a call to set_default_resource(memory_resource*) is to return
    //! the value from new_delete_resource().
    //!
    //! \return the installed default memory_resource
    //! \relates memory_resource
    memory_resource* get_default_resource() noexcept;

    //! Sets the default global memory_resource. This call is threadsafe in
    //! that accesses to the globally installed pointer will properly
    //! synchronize with each other, however it is up to the user to keep
    //! removed default memory_resource instances alive long enough to
    //! service any remaining requests.
    //!
    //! \param mr The memory_resource to install as the new global default.
    //!           A nullptr indicates that the return value from
    //!           get_default_resource() should be installed as the default
    //! \return the previously installed default memory_resource
    //! \relates memory_resource
    memory_resource* set_default_resource(memory_resource* mr) noexcept;
}
