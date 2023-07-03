#ifndef COMPONENT_MANAGER_HEADER
#define COMPONENT_MANAGER_HEADER

#include "component_array_wrapper.hpp"
#include "data_pool_interface.hpp"
#include "ext_uid_object_pool.hpp"
#include "signature_manager.hpp"

#include <iostream>
#include <memory>
#include <vector>

namespace trecs
{
   // Components are arranged into pools of data based on component type.
   // Two components with different types can be referenced by the same
   // component UID. Two components with the same type cannot be referenced by
   // the same component UID.
   // Component UIDs are entity UIDs. A component's UID indicates an entity
   // that partially comprises that component.
   //
   //    E.g. Entity UID '1' might have a rigid body component and a shape
   //    component. The rigid body component and the shape component would both
   //    have component UID '1'.
   //
   // The ComponentManager allocates data but does not automatically free that
   // data on destruction. 
   class ComponentManager
   {
      public:
         ComponentManager(size_t max_size);

         ComponentManager & operator=(const ComponentManager & other);

         ComponentManager & operator=(ComponentManager & other);
 
         // Resets all of the data pools back to zero elements. Does not
         // release any data.
         void clear(void);

         // Releases the ownership of the pool allocators. This should only be
         // called if the source object has been assigned to some other
         // destination object.
         // This does not free any of the allocated memory.
         void release(void);

         // Indicates that a component of a particular type can be added to
         // this component manager.
         template <typename Component_T>
         void registerComponent(void)
         {
            signature_t old_sig = signatures_.getSignature<Component_T>();
            if (old_sig != error_signature)
            {
               std::cout << "Component with signature " << old_sig << " has already been registered\n";
               return;
            }

            signature_t new_sig = signatures_.registerComponent<Component_T>();
            if (new_sig == error_signature)
            {
               std::cout << "Couldn't register signature for new component!\n";
               return;
            }

            allocators_[new_sig].reset(
               new ExternalUidObjectPool<Component_T>(max_size_)
            );
         }

         template <typename Component_T>
         ComponentArrayWrapper<Component_T> getComponents(void)
         {
            ComponentArrayWrapper<Component_T> wrapper(
               retrievePoolByType<Component_T>()
            );
            return wrapper;
         }

         // Attempts to add a component with a particular component UID.
         // Returns 'new_component_uid' if successful, returns -1 otherwise.
         template <typename Component_T>
         uid_t addComponent(
            uid_t new_component_uid,
            const Component_T & component
         )
         {
            ExternalUidObjectPool<Component_T> * pool = \
               retrievePoolByType<Component_T>();

            if (pool == nullptr)
            {
               std::cout << "Couldn't add component\n";
               return -1;
            }

            uid_t result = pool->addComponent(new_component_uid, component);

            return result;
         }

         // Removes a particular component type at a particular component UID.
         template <typename Component_T>
         void removeComponent(uid_t removed_component_uid)
         {
            ExternalUidObjectPool<Component_T> * pool = \
               retrievePoolByType<Component_T>();

            if (pool == nullptr)
            {
               return;
            }

            pool->removeComponent(removed_component_uid);
         }

         // Removes all of the components related to a particular component
         // UID. Component UIDs are entity UIDs, so this should be called when
         // an entity is removed.
         void removeComponents(uid_t removed_components_uid);

         template <typename Component_T>
         Component_T * getComponent(uid_t component_uid)
         {
            return retrieveComponentByUid<Component_T>(component_uid);
         }

         template <typename Component_T>
         const Component_T * getComponent(uid_t component_uid) const
         {
            return retrieveComponentByUid<Component_T>(component_uid);
         }

         template <typename Component_T>
         size_t getNumComponents(void) const
         {
            const auto pool = retrievePoolByType<Component_T>();
            if (pool == nullptr)
            {
               return 0;
            }

            return pool->size();
         }

         template <typename Component_T>
         signature_t getSignature(void) const
         {
            return signatures_.getSignature<Component_T>();
         }

         size_t getNumSignatures(void) const;

         template <typename Component_T>
         std::vector<uid_t> getComponentEntities(void) const
         {
            const auto derived_pool = retrievePoolByType<Component_T>();
            if (derived_pool == nullptr)
            {
               std::cout << "Couldn't find component pool\n";
               return std::vector<uid_t>();
            }

            return derived_pool->getUids();
         }

      private:

         size_t max_size_;

         // This is a mapping of integer signature types to allocators.
         std::vector<std::unique_ptr<IDataPool> > allocators_;

         SignatureManager<signature_t> signatures_;

         template <typename Component_T>
         Component_T * retrieveComponentByUid(uid_t component_uid)
         {
            ExternalUidObjectPool<Component_T> * pool_derived = \
               retrievePoolByType<Component_T>();

            if (pool_derived == nullptr)
            {
               return nullptr;
            }

            return pool_derived->getComponent(component_uid);
         }

         template <typename Component_T>
         const Component_T * retrieveComponentByUid(uid_t component_uid) const
         {
            ExternalUidObjectPool<Component_T> * pool_derived = \
               retrievePoolByType<Component_T>();

            if (pool_derived == nullptr)
            {
               return nullptr;
            }

            return pool_derived->getComponent(component_uid);
         }

         template <typename Component_T>
         ExternalUidObjectPool<Component_T> * retrievePoolByType(void)
         {
            signature_t signature = getSignature<Component_T>();

            if (signature >= allocators_.size())
            {
               std::cout << "Couldn't find pool for signature " << static_cast<int>(signature) << "\n";
               return nullptr;
            }

            IDataPool * pool_base = allocators_.at(signature).get();

            if (pool_base == nullptr)
            {
               std::cout << "Pool signature hasn't been created for component signature " << signature << "\n";
            }

            ExternalUidObjectPool<Component_T> * pool_derived = \
               static_cast<ExternalUidObjectPool<Component_T> *>(pool_base);

            return pool_derived;
         }

         template <typename Component_T>
         const ExternalUidObjectPool<Component_T> * retrievePoolByType(void) const
         {
            signature_t signature = getSignature<Component_T>();

            if (signature >= allocators_.size())
            {
               std::cout << "Couldn't find pool for signature " << signature << "\n";
               return nullptr;
            }

            IDataPool * pool_base = allocators_.at(signature).get();

            if (pool_base == nullptr)
            {
               std::cout << "Pool signature hasn't been created for component signature " << signature << "\n";
            }

            ExternalUidObjectPool<Component_T> * pool_derived = \
               static_cast<ExternalUidObjectPool<Component_T> *>(pool_base);

            return pool_derived;
         }
   };
}

#endif
