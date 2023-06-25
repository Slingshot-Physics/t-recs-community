#include "component_manager.hpp"

namespace trecs
{
   ComponentManager::ComponentManager(size_t max_size)
      : max_size_(max_size)
      , allocators_(max_num_signatures)
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

      for (unsigned int i = 0; i < other.allocators_.size(); ++i)
      {
         if (other.allocators_[i].get() != nullptr)
         {
            allocators_[i].reset(other.allocators_[i].release());
            std::cout << "other allocator pointer is null? " << (other.allocators_[i].get() == nullptr) << "\n";
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

      for (unsigned int i = 0; i < other.allocators_.size(); ++i)
      {
         if (other.allocators_[i].get() != nullptr)
         {
            allocators_[i].reset(other.allocators_[i].get());
            std::cout << "other allocator pointer is null? " << (other.allocators_[i].get() == nullptr) << "\n";
         }
      }

      return *this;
   }

   void ComponentManager::release(void)
   {
      for (auto & allocator : allocators_)
      {
         if (allocator != nullptr)
         {
            allocator.release();
         }
      }
   }

   void ComponentManager::removeComponents(uid_t removed_components_uid)
   {
      for (auto & alloc : allocators_)
      {
         if (alloc != nullptr)
         {
            alloc->removeComponent(removed_components_uid);
         }
      }
   }

   size_t ComponentManager::getNumSignatures(void) const
   {
      return signatures_.size();
   }
}
