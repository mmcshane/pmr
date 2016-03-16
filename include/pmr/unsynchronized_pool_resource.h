#pragma once

#include "pmr/memory_resource.h"
#include "pmr/polymorphic_allocator.h"
#include "pmr/pool_options.h"
#include "pmr/detail/pool.h"
#include <cstdint>
#include <vector>

namespace pmr
{

    //! A memory_resource backed by a series of memory pools, structured
    //! by size. This class is not threadsafe.
    //!
    //! \sa pmr::synchronized_pool_resource
    class unsynchronized_pool_resource : public memory_resource
    {
      public:
        //! Instantiate using a default-constructed pool_options and
        //! the memory_resource returned by pmr::get_default_resource()
        unsynchronized_pool_resource();

        unsynchronized_pool_resource(
                const unsynchronized_pool_resource&) = delete;

        //! Instantiate using a default-constructed pool_options and the
        //! provided memory_resource
        //!
        //! \param upstream The memory_resource to use as an upstream memory
        //!                 provider
        explicit unsynchronized_pool_resource(memory_resource* upstream);

        //! Instantiate  using the supplied pool_options and memory_resource
        //!
        //! \param opts Specifies pool sizing hints
        //! \param upstream The memory_resource to use as an upstream memory
        //!                 provider
        explicit unsynchronized_pool_resource(const pool_options& opts,
                memory_resource* upstream);

        //! \sa release()
        ~unsynchronized_pool_resource();

        unsynchronized_pool_resource& operator=(
                const unsynchronized_pool_resource&) = delete;

        //! Frees all memory allocated via this object, even if that memory
        //! has not been deallocated.
        void release();

        //! Access the upstream memory resource used by this instance
        //!
        //! \returns A pointer to the upstream memory_resource
        memory_resource* upstream_resource() const;

        //! Access the pool_options used by this instance. Note that while the
        //! pool_options instance passed to the constructor is a hint, the
        //! value returned from this function reflects the actual pool sizing
        //! values chosen by the implementation
        //!
        //! \returns The pool_options used to size the internal memory pools
        pool_options options() const;

      protected:
        void* do_allocate(std::size_t bytes, std::size_t align) override;

        void do_deallocate(
                void * ptr, std::size_t bytes, std::size_t align) override;

        bool do_is_equal(const memory_resource& other) const override;

        void init_pools();

      private:
        void adjust_pool_options();
        detail::pool& which_pool(std::size_t bytes, std::size_t align);

        pool_options m_opts;
        memory_resource& m_upstream;
        std::vector<detail::pool, polymorphic_allocator<detail::pool>> m_pools;
        detail::memblocks m_oversized;
    };
}
