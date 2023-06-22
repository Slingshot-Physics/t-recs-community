#ifndef EXT_UID_POOL_ALLOCATOR_HEADER
#define EXT_UID_POOL_ALLOCATOR_HEADER

#include "pool_allocator_interface.hpp"

#include "ecs_types.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <map>
#include <vector>

#include <iostream>

namespace trecs
{
   // A template class that generates a user-specified, byte-aligned pool of
   // data of fixed size. This class decouples unique identifiers from indices
   // through a mapping. Byte-boundaries are enforced by applying index offsets
   // and increments according to the alignment requirement and the size of the
   // underlying type.
   template <typename T>
   class ExternalUidPoolAllocator : public PoolAllocatorInterface
   {
      public:

         ExternalUidPoolAllocator(size_t num_elements, size_t alignment)
            : max_num_elements_(num_elements)
            , alignment_(alignment)
            , data_pool_(nullptr)
         {
            assert(max_num_elements_ > 0);
            assert(alignment > 0);

            // Number of bytes between data type globs.
            index_increment_ = (
               (sizeof(T) / alignment_) * alignment + alignment_ * ((sizeof(T) % alignment_) > 0)
            );

            max_num_bytes_ = index_increment_ * (max_num_elements_ + 1);

            data_pool_ = new unsigned char[max_num_bytes_];

            // The starting index in the byte buffer for the first component.
            index_offset_ = (
               alignment_ - reinterpret_cast<size_t>(
                  &(data_pool_[0])
               ) % alignment_
            ) % alignment_;

            last_free_index_ = index_offset_;

            for (size_t i = 0; i < max_num_bytes_; ++i)
            {
               data_pool_[i] = 0;
            }
         }

         ~ExternalUidPoolAllocator(void) override
         {
            delete [] data_pool_;
            data_pool_ = nullptr;
         }

         // Clears out the entire byte array and resets the last free index to
         // the initial byte offset. Deletes all of the data in the allocator.
         void clear(void)
         {
            for (size_t i = 0; i < max_num_bytes_; ++i)
            {
               data_pool_[i] = 0;
            }

            last_free_index_ = index_offset_;
            uid_to_index_.clear();
         }

         int addComponent(uid_t new_component_uid, const T & component)
         {
            if (size() >= max_num_elements_)
            {
               // std::cout << "Too many items added\n";
               return -1;
            }

            if (uid_to_index_.find(new_component_uid) != uid_to_index_.end())
            {
               std::cout << "Couldn't add UID " << new_component_uid << " because it already exists\n";
               return -1;
            }

            T * last_component = getComponentFromIndex(last_free_index_);
            *last_component = component;

            uid_to_index_[new_component_uid] = last_free_index_;

            last_free_index_ += index_increment_;

            return new_component_uid;
         }

         T * getComponent(uid_t uid)
         {
            auto uid_index = uid_to_index_.find(uid);
            if (uid_index == uid_to_index_.end())
            {
               return nullptr;
            }

            return getComponentFromIndex(uid_index->second);
         }

         const T * getComponent(uid_t uid) const
         {
            auto uid_index = uid_to_index_.find(uid);
            if (uid_index == uid_to_index_.end())
            {
               return nullptr;
            }

            return getComponentFromIndex(uid_index->second);
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
            for (const auto uid_to_index : uid_to_index_)
            {
               if (uid_to_index.second == last_free_index_ - index_increment_)
               {
                  uid_to_update = uid_to_index.first;
                  break;
               }
            }

            // Move last component into the removed component's location.
            size_t removed_index = uid_to_index_.at(uid_to_remove);
            T * removed_component = getComponentFromIndex(removed_index);
            T * last_component = getComponentFromIndex(last_free_index_ - index_increment_);
            *removed_component = *last_component;

            // Update the index that the last component's UID maps to.
            uid_to_index_[uid_to_update] = removed_index;

            // Remove the UID entry in the UID-index mapping.
            uid_to_index_.erase(uid_to_remove);

            // Decrement the last free index.
            last_free_index_ -= index_increment_;
         }

         // Using this method is not recommended because it's slow. But it's
         // good for tests.
         std::vector<uid_t> getUids(void) const
         {
            std::vector<uid_t> uids;
            for (const auto uid_to_index: uid_to_index_)
            {
               uids.push_back(uid_to_index.first);
            }

            return uids;
         }

         // Returns the number of active components in the pool.
         size_t size(void) const
         {
            return uid_to_index_.size();
         }

         // Returns the total number of bytes in use by active components.
         size_t sizeBytes(void) const
         {
            return uid_to_index_.size() * index_increment_;
         }

         // Returns the maximum number of components that can be held in this
         // allocator.
         size_t capacity(void) const
         {
            return max_num_elements_;
         }

      private:

         // The maximum number of elements of type T in the data pool.
         size_t max_num_elements_;

         // Size of the data pool in bytes.
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

         // A conversion from UID to index.
         std::map<uid_t, size_t> uid_to_index_;

         // Given a byte-level index, retrieves the element of the underlying
         // data structure at that index, and casts the pointer to that element
         // to a pointer to the desired type.
         T * getComponentFromIndex(size_t index)
         {
            // Just in case something terrible has happened in the
            // implementation....
            assert((index - index_offset_) % alignment_ == 0);

            T * component = reinterpret_cast<T * >(&data_pool_[index]);
            return component;
         }

         // Given a byte-level index, retrieves the element of the underlying
         // data structure at that index, and casts the pointer to that element
         // to a pointer to the desired type.
         const T * getComponentFromIndex(size_t index) const
         {
            // Just in case something terrible has happened in the
            // implementation....
            assert((index - index_offset_) % alignment_ == 0);

            T * component = reinterpret_cast<T * >(&data_pool_[index]);
            return component;
         }

         ExternalUidPoolAllocator(void);

         ExternalUidPoolAllocator(const ExternalUidPoolAllocator &);

         ExternalUidPoolAllocator(ExternalUidPoolAllocator &);
   };
}

#endif
