#include "component_manager.hpp"

namespace trecs
{
   ComponentManager::ComponentManager(size_t max_size)
      : max_size_(max_size)
      , allocators_(max_num_signatures, nullptr)
   { }

   void ComponentManager::clear(void)
   {
      for (auto & alloc : allocators_)
      {
         if (alloc != nullptr)
         {
            alloc->clear();
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
