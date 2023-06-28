#ifndef EXTERNAL_UID_OBJECT_POOL_HEADER
#define EXTERNAL_UID_OBJECT_POOL_HEADER

#include "data_pool_interface.hpp"

#include "ecs_types.hpp"

#include <cassert>
#include <map>
#include <vector>

#include <iostream>

namespace trecs
{
   template <typename Object_T>
   class ExternalUidObjectPool : public IDataPool
   {
      public:

         ExternalUidObjectPool(size_t num_elements)
            : max_num_elements_(num_elements)
            , data_pool_(nullptr)
         {
            assert(max_num_elements_ > 0);

            initialize();
         }

         ~ExternalUidObjectPool(void) override
         {
            delete [] data_pool_;
            data_pool_ = nullptr;
         }

         ExternalUidObjectPool<Object_T> & operator=(
            const ExternalUidObjectPool<Object_T> & other
         )
         {
            if (this == &other)
            {
               return *this;
            }

            clear();

            if (data_pool_ != nullptr)
            {
               delete [] data_pool_;
            }

            max_num_elements_ = other.max_num_elements_;

            initialize();

            for (const auto & uid_to_index : other.uid_to_index_)
            {
               addComponent(
                  uid_to_index.first,
                  *other.getComponent(uid_to_index.first)
               );
            }

            return *this;
         }

         IDataPool & operator=(const IDataPool & other) override
         {
            if (this == &other)
            {
               return *this;
            }

            const ExternalUidObjectPool<Object_T> * other_derived_ptr = \
               dynamic_cast<const ExternalUidObjectPool<Object_T> *>(&other);

            if (other_derived_ptr == nullptr)
            {
               std::cout << "Couldn't downcast base pool class to derived pool class\n";
               return *this;
            }

            *this = *other_derived_ptr;

            return *this;
         }

         // Resets the last free index to the 0. Deletes the uid-index mapping.
         void clear(void) override
         {
            last_free_index_ = 0;
            uid_to_index_.clear();
         }

         uid_t addComponent(uid_t new_component_uid, const Object_T & component)
         {
            if (size() >= max_num_elements_)
            {
               return -1;
            }

            if (uid_to_index_.find(new_component_uid) != uid_to_index_.end())
            {
               std::cout << "Couldn't add UID " << new_component_uid << " because it already exists\n";
               return -1;
            }

            data_pool_[last_free_index_] = component;
            uid_to_index_[new_component_uid] = last_free_index_;
            ++last_free_index_;

            return new_component_uid;
         }

         Object_T * getComponent(uid_t uid)
         {
            auto uid_index = uid_to_index_.find(uid);
            if (uid_index == uid_to_index_.end())
            {
               return nullptr;
            }

            return &(data_pool_[uid_index->second]);
         }

         const Object_T * getComponent(uid_t uid) const
         {
            auto uid_index = uid_to_index_.find(uid);
            if (uid_index == uid_to_index_.end())
            {
               return nullptr;
            }

            return &(data_pool_[uid_index->second]);
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
            uid_t uid_to_update = -1;
            for (const auto & uid_index : uid_to_index_)
            {
               if (uid_index.second == (last_free_index_ - 1))
               {
                  uid_to_update = uid_index.first;
                  break;
               }
            }

            // Move last component into the removed component's location.
            size_t removed_index = uid_to_index_.at(uid_to_remove);
            Object_T & removed_component = data_pool_[removed_index];
            Object_T & last_component = data_pool_[last_free_index_ - 1];
            removed_component = last_component;

            // Update the index that the last component's UID maps to.
            uid_to_index_[uid_to_update] = removed_index;

            // Remove the UID entry in the UID-index mapping.
            uid_to_index_.erase(uid_to_remove);

            // Decrement the last free index.
            last_free_index_ -= 1;
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
         size_t size(void) const override
         {
            return uid_to_index_.size();
         }

         // Returns the maximum number of components that can be held in this
         // allocator.
         size_t capacity(void) const override
         {
            return max_num_elements_;
         }

      private:

         // The maximum number of elements of type T in the data pool.
         size_t max_num_elements_;

         // An index of the data array where the next element T should be
         // inserted.
         size_t last_free_index_;

         // The underlying data structure. Alignment across byte boundaries is
         // enforced by casting subsets of bytes of this array to the type T.
         Object_T * data_pool_;

         // A conversion from UID to index.
         std::map<uid_t, size_t> uid_to_index_;

         void initialize(void)
         {
            data_pool_ = new Object_T[max_num_elements_];
            last_free_index_ = 0;
         }

   };
}

#endif
