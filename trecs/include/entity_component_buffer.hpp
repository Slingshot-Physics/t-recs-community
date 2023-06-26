#ifndef ENTITY_COMPONENT_BUFFER_HEADER
#define ENTITY_COMPONENT_BUFFER_HEADER

#include "ecs_types.hpp"

#include "archetype.hpp"
#include "component_array_wrapper.hpp"
#include "component_manager.hpp"
#include "entity_manager.hpp"

#include <type_traits>
#include <vector>

namespace trecs
{
   class EntityComponentBuffer
   {
      public:
         EntityComponentBuffer(size_t max_buffer_size)
            : max_buffer_size_(max_buffer_size)
            , entities_(max_buffer_size_)
            , components_(max_buffer_size_)
         { }

         // Copies the source ECB into this destination ECB without releasing
         // the source ECB's ownership of its allocators.
         EntityComponentBuffer & operator=(
            const EntityComponentBuffer & other
         );

         // Copies the source ECB into this destination ECB and releases the
         // source ECB's ownership of its allocators.
         EntityComponentBuffer & operator=(EntityComponentBuffer & other);

         uid_t addEntity(void);

         void removeEntity(uid_t entity_uid);

         size_t numEntities(void) const;

         size_t numSignatures(void) const;

         void release(void);

         // Returns true if a type list is supported by this ECB, false
         // otherwise.
         template <class...Types>
         bool supportsComponents(void) const
         {
            bool supported = true;
            supportsComponents<Types...>(supported);

            return supported;
         }

         template <typename Component_T>
         void registerComponent(void)
         {
            components_.registerComponent<Component_T>();
         }

         // Returns a copy of the vector of entities associated with a
         // particular component type.
         template <typename Component_T>
         std::vector<uid_t> getComponentEntities(void) const
         {
            return components_.getComponentEntities<Component_T>();
         }

         // Attempts to update a component on an active entity UID. If a
         // component of the same type is already associated with this entity,
         // then the new component replaces the old component. If the new
         // component type isn't associated with the entity, the the entity's
         // signature is updated and the component is assigned to the entity.
         template <typename Component_T>
         bool updateComponent(uid_t entity_uid, const Component_T & component)
         {
            if (!entities_.entityActive(entity_uid))
            {
               std::cout << "Entity uid " << entity_uid << " inactive\n";
               return false;
            }

            DefaultArchetype old_arch = entities_.getArchetype(entity_uid);
            signature_t component_sig = components_.getSignature<Component_T>();

            if (old_arch.supports(component_sig))
            {
               Component_T * old_component = components_.getComponent<Component_T>(entity_uid);
               if (old_component == nullptr)
               {
                  return false;
               }

               *old_component = component;
               return true;
            }

            DefaultArchetype new_arch = old_arch;
            new_arch.mergeSignature(component_sig);
            components_.addComponent<Component_T>(entity_uid, component);
            entities_.setArchetype(entity_uid, new_arch);

            return true;
         }

         // Attempt to retrieve a component of a particular type from an
         // entity. Returns nullptr if the requested entity is not active or if
         // an unsupported component type is used.
         template <typename Component_T>
         Component_T * getComponent(uid_t entity_uid)
         {
            if (!entities_.entityActive(entity_uid))
            {
               return nullptr;
            }

            DefaultArchetype old_arch = entities_.getArchetype(entity_uid);
            signature_t component_sig = components_.getSignature<Component_T>();

            if (!old_arch.supports(component_sig))
            {
               return nullptr;
            }

            return components_.getComponent<Component_T>(entity_uid);
         }

         template <typename Component_T>
         ComponentArrayWrapper<Component_T> getComponents(void)
         {
            return components_.getComponents<Component_T>();
         }

         // Attempts to remove a given component type from a specified entity.
         template <typename Component_T>
         void removeComponent(uid_t entity_uid)
         {
            if (!entities_.entityActive(entity_uid))
            {
               return;
            }

            signature_t component_sig = components_.getSignature<Component_T>();

            if (component_sig == error_signature)
            {
               return;
            }

            DefaultArchetype old_arch = entities_.getArchetype(entity_uid);

            if (!old_arch.supports(component_sig))
            {
               return;
            }

            components_.removeComponent<Component_T>(entity_uid);
            DefaultArchetype new_arch = old_arch;
            new_arch.removeSignature(component_sig);
            entities_.setArchetype(entity_uid, new_arch);
         }

      private:
         size_t max_buffer_size_;

         EntityManager entities_;

         ComponentManager components_;

         template <class T>
         void supportsComponents(bool & supported) const
         {
            supported &= components_.getSignature<T>() != error_signature;
         }

         template <class First, class...TheRest>
         auto supportsComponents(bool & supported) const ->
            typename std::enable_if<sizeof...(TheRest) != 0, void>::type
         {
            supported &= components_.getSignature<First>() != error_signature;
            if (!supported)
            {
               return;
            }

            supportsComponents<TheRest...>(supported);
         }

   };
}

#endif
