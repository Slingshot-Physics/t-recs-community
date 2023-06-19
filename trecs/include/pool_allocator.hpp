#ifndef POOL_ALLOCATOR_HEADER
#define POOL_ALLOCATOR_HEADER

#include "pool_allocator_interface.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <deque>
#include <map>

#include <iostream>

namespace trecs
{
   // A template class that generates a user-specified, byte-aligned pool of
   // data of fixed size. This class decouples unique identifiers from indices
   // through a mapping. Byte-boundaries are enforced by applying index offsets
   // and increments according to the alignment requirement and the size of the
   // underlying type.
   template <typename T>
   class PoolAllocator : public PoolAllocatorInterface
   {
      public:

         PoolAllocator(uint64_t num_elements, uint64_t alignment)
            : max_num_elements_(num_elements)
            , alignment_(alignment)
            , data_pool_(nullptr)
         {
            assert(max_num_elements_ > 0);
            assert(alignment > 0);

            // Number of bytes between data type globs.
            index_increment_ = (
               (sizeof(T) / alignment_) * alignment + alignment_ * (sizeof(T) % alignment_ > 0)
            );

            max_num_bytes_ = index_increment_ * max_num_elements_;

            data_pool_ = new unsigned char[max_num_bytes_];

            index_offset_ = (
               alignment_ - reinterpret_cast<size_t>(
                  &(data_pool_[0])
               ) % alignment_
            ) % alignment_;
            last_free_index_ = index_offset_;

            std::memset(data_pool_, 0, max_num_bytes_);

            for (uid_t i = 0; i < static_cast<uid_t>(max_num_elements_); ++i)
            {
               uid_pool_.push_back(i);
            }
         }

         virtual ~PoolAllocator(void)
         {
            if (data_pool_ != nullptr)
            {
               delete [] data_pool_;
               data_pool_ = nullptr;
            }
         }

         // Zeroes out the underlying data buffer. Clears the current list of
         // UIDs and resets the UID pool to contain all possible UIDs.
         void clear(void)
         {
            last_free_index_ = index_offset_;
            uid_to_index_.clear();

            std::memset(data_pool_, 0, max_num_bytes_);

            uids_.clear();
            uid_pool_.clear();

            for (uid_t i = 0; i < static_cast<uid_t>(max_num_elements_); ++i)
            {
               uid_pool_.push_back(i);
            }
         }

         int addComponent(const T & component)
         {
            if (size() >= static_cast<size_t>(max_num_elements_))
            {
               std::cout << "Too many items added\n";
               return -1;
            }

            uid_t new_component_uid = uid_pool_.front();
            assert(uid_to_index_.find(new_component_uid) == uid_to_index_.end());

            T & last_component = getComponentFromIndex(last_free_index_);
            last_component = component;

            uid_pool_.pop_front();
            uids_.push_back(new_component_uid);

            uid_to_index_[new_component_uid] = last_free_index_;

            last_free_index_ += index_increment_;

            return new_component_uid;
         }

         T & getComponent(uid_t uid)
         {
            return getComponentFromIndex(uid_to_index_.at(uid));
         }

         const T & getComponent(uid_t uid) const
         {
            return getComponentFromIndex(uid_to_index_.at(uid));
         }

         // Move the last item in the component array to the removed slot.
         // Update the ID map to map the ID of the last-index component to the
         // removed index.
         void removeComponent(uid_t uid_to_remove)
         {
            if (uid_to_index_.find(uid_to_remove) == uid_to_index_.end())
            {
               return;
            }

            // Find the UID of the component that maps to the last free index
            // in the data array.
            uid_t uid_to_update = 0;
            for (
               uid_mp_cit_t uid_mp_it = uid_to_index_.begin();
               uid_mp_it != uid_to_index_.end();
               ++uid_mp_it
            )
            {
               if (uid_mp_it->second == last_free_index_ - index_increment_)
               {
                  uid_to_update = uid_mp_it->first;
                  break;
               }
            }

            // Move last component into the removed component's location.
            size_t removed_index = uid_to_index_[uid_to_remove];
            T & removed_component = getComponentFromIndex(removed_index);
            T & last_component = getComponentFromIndex(last_free_index_ - index_increment_);
            removed_component = last_component;

            // Update the index that the last component's UID maps to.
            uid_to_index_[uid_to_update] = removed_index;

            // Remove the UID entry in the UID-index mapping and add the
            // removed UID back to the UID pool. Remove the UID entry from the
            // deque of UIDs that are in use.
            uid_to_index_.erase(uid_to_remove);
            uid_pool_.push_back(uid_to_remove);
            uids_.erase(std::find(uids_.begin(), uids_.end(), uid_to_remove));

            // Decrement the last free index.
            last_free_index_ -= index_increment_;
         }

         // Returns a deque of all of the active UIDs in the pool.
         const std::deque<uid_t> & getUids(void) const
         {
            return uids_;
         }

         // Returns the number of active components in the pool.
         size_t size(void) const
         {
            return uids_.size();
         }

         // Returns the total number of bytes in use by active components.
         size_t sizeBytes(void) const
         {
            return uids_.size() * index_increment_;
         }

         // Returns the maximum number of components that can be held in this
         // allocator.
         size_t capacity(void) const
         {
            return max_num_elements_;
         }

      private:

         // The maximum number of elements of type T in the data pool.
         int64_t max_num_elements_;

         // Size of the data pool in bytes. This is equal to the size of the
         // underlying data structure.
         size_t max_num_bytes_;

         // The size of the byte-level alignment of data elements.
         size_t alignment_;

         // An optional offset applied to the underlying data pool to ensure
         // that all elements remain on the user-specified byte boundaries.
         size_t index_offset_;

         // The number of bytes to increment the free index counter by. This is
         // determined by the size of the template type and the alignment.
         size_t index_increment_;

         // An index of the data array where the next element T should be
         // inserted.
         size_t last_free_index_;

         // The underlying data structure. Alignment across byte boundaries is
         // enforced by casting subsets of bytes of this array to the type T.
         unsigned char * data_pool_;

         // The pool of remaining available unique IDs to new elements in this
         // allocator.
         std::deque<uid_t> uid_pool_;

         // The UIDs currently being used by elements in this allocator.
         std::deque<uid_t> uids_;

         // A conversion from UID to index.
         std::map<uid_t, size_t> uid_to_index_;

         typedef std::map<uid_t, size_t>::const_iterator uid_mp_cit_t;

         // Given a byte-level index, retrieves the element of the underlying
         // data structure at that index, and casts the pointer to that element
         // to a pointer to the desired type.
         T & getComponentFromIndex(size_t index)
         {
            // Just in case something terrible has happened in the
            // implementation....
            assert((index - index_offset_) % alignment_ == 0);

            T * component = reinterpret_cast<T * >(&data_pool_[index]);
            return *component;
         }

         const T & getComponentFromIndex(size_t index) const
         {
            // Just in case something terrible has happened in the
            // implementation....
            assert((index - index_offset_) % alignment_ == 0);

            T * component = reinterpret_cast<T * >(&data_pool_[index]);
            return *component;
         }

         PoolAllocator(void);

         PoolAllocator(const PoolAllocator &);

         PoolAllocator(PoolAllocator &);

   };

}

#endif
