#include "component_manager.hpp"

namespace trecs
{
   ComponentManager::ComponentManager(size_t max_size)
      : max_size_(max_size)
      , data_pools_(max_num_signatures)
   { }

   // Moves ownership of the allocator buffers from the source ComponentManager
   // to this ComponentManager. Change of ownership requires the source to be
   // non-const.
   ComponentManager & ComponentManager::operator=(
      ComponentManager & other
   )
   {
      if (this == &other)
      {
         return *this;
      }

      max_size_ = other.max_size_;
      signatures_ = other.signatures_;

      // Free all of the underlying data.
      for (auto & allocator : data_pools_)
      {
         allocator.reset(nullptr);
      }

      // Delete all of the existing allocator pointers.
      data_pools_.clear();

      // Re-append null pointers to the allocator pointers.
      for (unsigned int i = 0; i < max_num_signatures; ++i)
      {
         data_pools_.push_back(nullptr);
      }

      for (unsigned int i = 0; i < other.data_pools_.size(); ++i)
      {
         if (other.data_pools_[i].get() != nullptr)
         {
            data_pools_[i].reset(other.data_pools_[i].release());
         }
      }

      return *this;
   }

   // Copies the raw pointers to allocator buffers from the source
   // ComponentManager but does not change their ownership. The caller must
   // ensure that the pointer ownership on the source ComponentManager is
   // released after assignment completes.
   ComponentManager & ComponentManager::operator=(
      const ComponentManager & other
   )
   {
      if (this == &other)
      {
         return *this;
      }

      max_size_ = other.max_size_;
      signatures_ = other.signatures_;

      // Free all of the underlying data.
      for (auto & allocator : data_pools_)
      {
         allocator.reset(nullptr);
      }

      // Delete all of the existing allocator pointers.
      data_pools_.clear();

      // Re-append null pointers to the allocator pointers.
      for (unsigned int i = 0; i < max_num_signatures; ++i)
      {
         data_pools_.push_back(nullptr);
      }

      for (unsigned int i = 0; i < other.data_pools_.size(); ++i)
      {
         if (other.data_pools_[i].get() != nullptr)
         {
            data_pools_[i].reset(other.data_pools_[i].get());
         }
      }

      return *this;
   }

   void ComponentManager::clear(void)
   {
      for (auto & allocator : data_pools_)
      {
         if (allocator != nullptr)
         {
            allocator->clear();
         }
      }
   }

   void ComponentManager::release(void)
   {
      for (auto & allocator : data_pools_)
      {
         if (allocator != nullptr)
         {
            allocator.release();
         }
      }
   }

   void ComponentManager::removeComponents(uid_t removed_components_uid)
   {
      for (auto & alloc : data_pools_)
      {
         if (alloc != nullptr)
         {
            alloc->removeComponent(removed_components_uid);
         }
      }
   }

   // Returns the number of registered component signatures.
   size_t ComponentManager::getNumSignatures(void) const
   {
      return signatures_.size();
   }
}
