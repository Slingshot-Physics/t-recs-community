#ifndef BYTE_POOL_HEADER
#define BYTE_POOL_HEADER

#include "pool_allocator_interface.hpp"

#include <algorithm>
#include <cassert>
#include <cstdint>
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
   class BytePool : public PoolAllocatorInterface
   {
      public:

         BytePool(uint64_t num_elements, uint64_t alignment)
            : max_num_elements_(num_elements)
            , alignment_(alignment)
            , data_pool_(nullptr)
         {
            assert(max_num_elements_ > 0);
            assert(alignment > 0);

            initialize();
         }

         ~BytePool(void) override
         {
            delete [] data_pool_;
            data_pool_ = nullptr;
         }

         BytePool<T> & operator=(const BytePool<T> & other)
         {
            if (this == &other)
            {
               return *this;
            }

            uids_.clear();
            uid_pool_.clear();
            uid_to_index_.clear();

            if (data_pool_ != nullptr)
            {
               delete [] data_pool_;
            }

            max_num_elements_ = other.max_num_elements_;
            alignment_ = other.alignment_;

            initialize();

            // All of the components on the other pool allocator are at these
            // indices in the byte buffer:
            //
            //    old_index = old_index_offset + indices_per_element * element_number
            //
            // And I want to copy these bytes into my buffer. Each component
            // will have the same 'element_number' (kind of a count-based-ID)
            // and 'indices_per_element' (which is the number of bytes per
            // component). So the new index will be:
            //
            //    new_index = new_index_offset + indices_per_element * element_number
            //
            // Aaaaand with a little algebra:
            //
            //    new_index = old_index - old_index_offset + new_index_offset
            for (const auto other_uid_to_index : other.uid_to_index_)
            {
               uid_to_index_[other_uid_to_index.first] = (
                  (other_uid_to_index.second - other.index_offset_) + index_offset_
               );
            }

            uid_pool_ = other.uid_pool_;

            for (const auto uid_to_index : uid_to_index_)
            {
               T & component = getComponentFromIndex(uid_to_index.second);
               component = other.getComponentFromIndex(other.uid_to_index_.at(uid_to_index.first));
            }

            return *this;
         }

         PoolAllocatorInterface & operator=(
            const PoolAllocatorInterface & other
         ) override
         {
            if (this == &other)
            {
               return *this;
            }

            const BytePool<T> * other_derived_ptr = \
               dynamic_cast<const BytePool<T> *>(&other);

            if (other_derived_ptr == nullptr)
            {
               std::cout << "Couldn't downcast base pool class to derived pool class\n";
               return *this;
            }

            *this = *other_derived_ptr;

            return *this;
         }

         // Zeroes out the underlying data buffer. Clears the current list of
         // UIDs and resets the UID pool to contain all possible UIDs.
         void clear(void) override
         {
            for (size_t i = 0; i < max_num_bytes_; ++i)
            {
               data_pool_[i] = 0;
            }

            last_free_index_ = index_offset_;
            uid_to_index_.clear();

            uids_.clear();
            uid_pool_.clear();

            for (uid_t i = 0; i < static_cast<uid_t>(max_num_elements_); ++i)
            {
               uid_pool_.push_back(i);
            }
         }

         uid_t addComponent(const T & component)
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

            uid_pool_.erase(uid_pool_.begin());
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
         void removeComponent(uid_t uid_to_remove) override
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

            // Zero out the bytes from the component that was moved to the
            // deleted component's index.
            for (size_t i = 0; i < index_increment_; ++i)
            {
               data_pool_[last_free_index_ + i] = 0;
            }
         }

         // Returns a container of all of the active UIDs in the pool.
         const std::vector<uid_t> & getUids(void) const
         {
            return uids_;
         }

         // Returns the number of active components in the pool.
         size_t size(void) const override
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
         size_t capacity(void) const override
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
         std::vector<uid_t> uid_pool_;

         // The UIDs currently being used by elements in this allocator.
         std::vector<uid_t> uids_;

         // A conversion from UID to index.
         std::map<uid_t, size_t> uid_to_index_;

         void initialize(void)
         {
            // Number of bytes between data type globs.
            index_increment_ = (
               (sizeof(T) / alignment_) * alignment_ + alignment_ * (sizeof(T) % alignment_ > 0)
            );

            max_num_bytes_ = index_increment_ * (max_num_elements_ + 1);

            data_pool_ = new unsigned char[max_num_bytes_];

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

            for (uid_t i = 0; i < static_cast<uid_t>(max_num_elements_); ++i)
            {
               uid_pool_.push_back(i);
            }
         }

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

         BytePool(void);

         BytePool(const BytePool &);

         BytePool(BytePool &);

   };

}

#endif
