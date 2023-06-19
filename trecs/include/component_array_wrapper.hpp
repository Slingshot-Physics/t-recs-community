#ifndef COMPONENT_ARRAY_WRAPPER_HEADER
#define COMPONENT_ARRAY_WRAPPER_HEADER

#include "ext_uid_object_pool.hpp"

namespace trecs
{
   // This is a simple wrapper around an `ExternalUidObjectPool` class that
   // only allows access to getters based on entity UIDs.
   template <typename Component_T>
   class ComponentArrayWrapper
   {
      public:
         ComponentArrayWrapper(void)
            : component_array_(nullptr)
         { }

         ComponentArrayWrapper(const ComponentArrayWrapper<Component_T> & other)
            : component_array_(other.component_array_)
         { }

         ComponentArrayWrapper(ExternalUidObjectPool<Component_T> * pool)
            : component_array_(pool)
         { }

         ComponentArrayWrapper<Component_T> & operator=(const ComponentArrayWrapper<Component_T> & other) = default;

         // Access a component by its entity UID.
         Component_T * operator[](uid_t uid)
         {
            return component_array_->getComponent(uid);
         }

         // Access a component by its entity UID.
         const Component_T * operator[](uid_t uid) const
         {
            return component_array_->getComponent(uid);
         }

         bool empty(void) const
         {
            return component_array_ == nullptr;
         }

         const std::vector<uid_t> getUids(void) const
         {
            return component_array_->getUids();
         }

         size_t size(void) const
         {
            return component_array_->size();
         }

         void print_uids(void) const
         {
            auto uids = component_array_->getUids();
            std::cout << "available uids\n";
            for (const auto uid : uids)
            {
               std::cout << "\t" << uid << "\n";
            }
         }

      private:

         ExternalUidObjectPool<Component_T> * component_array_;

   };
}

#endif
