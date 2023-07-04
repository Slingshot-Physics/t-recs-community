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

         EntityComponentBuffer(void)
            : max_buffer_size_(0)
            , entities_(max_buffer_size_)
            , components_(max_buffer_size_)
            , registration_locked_(false)
         { }
      
         EntityComponentBuffer(size_t max_buffer_size)
            : max_buffer_size_(max_buffer_size)
            , entities_(max_buffer_size_)
            , components_(max_buffer_size_)
            , registration_locked_(false)
         { }

         // Copies the source ECB into this destination ECB without releasing
         // the source ECB's ownership of its data pools.
         EntityComponentBuffer & operator=(const EntityComponentBuffer & other)
         {
            if (this == &other)
            {
               return *this;
            }

            max_buffer_size_ = other.max_buffer_size_;
            entities_ = other.entities_;
            components_ = other.components_;
            registration_locked_ = other.registration_locked_;

            return *this;
         }

         // Copies the source ECB into this destination ECB and releases the
         // source ECB's ownership of its data pools.
         EntityComponentBuffer & operator=(EntityComponentBuffer & other)
         {
            if (this == &other)
            {
               return *this;
            }

            max_buffer_size_ = other.max_buffer_size_;
            entities_ = other.entities_;
            components_ = other.components_;
            registration_locked_ = other.registration_locked_;

            return *this;
         }

         // Resets all of the entities and components in the ECB.
         void clear(void)
         {
            entities_.clear();
            components_.clear();
         }

         // Component types will not be registerable with the ECB after this
         // method is called.
         void lockRegistration(void)
         {
            registration_locked_ = true;
         }

         uid_t addEntity(void)
         {
            return entities_.addEntity();
         }

         void removeEntity(uid_t entity_uid)
         {
            entities_.removeEntity(entity_uid);
            components_.removeComponents(entity_uid);
         }

         // Returns the numebr of active entities in the ECB.
         size_t numEntities(void) const
         {
            return entities_.size();
         }

         // Returns the number of components that have been registered with
         // the ECB.
         size_t numSignatures(void) const
         {
            return components_.getNumSignatures();
         }

         // Releases the ownership of any existing components but does not
         // free them.
         void release(void)
         {
            components_.release();
         }

         // Returns true if a type list is supported by this ECB, false
         // otherwise.
         template <class...Types>
         bool supportsComponents(void) const
         {
            bool supported = true;
            supportsComponents<Types...>(supported);

            return supported;
         }

         // Attempts to register a component type with the ECB. The component
         // will not be registered if the ECB's registration is locked or if
         // the same component type has already been registered.
         template <typename Component_T>
         void registerComponent(void)
         {
            if (!registration_locked_)
            {
               components_.registerComponent<Component_T>();
            }
         }

         // Attempts to register one or more component types with the ECB. A
         // component will not be registered if the ECB's registration is
         // locked or if the same component type has already been registered.
         template <class...Types>
         void registerComponents(void)
         {
            if (!registration_locked_)
            {
               fancyRegisterComponents<Types...>();
            }
         }

         // Returns a const reference to a vector of all active entities in
         // the ECB.
         const std::vector<uid_t> & getEntities(void) const
         {
            return entities_.getUids();
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
               std::cout << "ECB entity uid " << entity_uid << " inactive\n";
               return false;
            }

            Component_T * old_component = components_.getComponent<Component_T>(entity_uid);

            // This is just as good as checking if an entity's current
            // archetype supports a component signature. If the component
            // attached to the entity is null, then that component hasn't been
            // added to the entity, and the entity's archetype does't support
            // the component's signature.
            if (old_component != nullptr)
            {
               *old_component = component;
               return true;
            }

            signature_t component_sig = components_.getSignature<Component_T>();
            if (component_sig == error_signature)
            {
               std::cout << "Couldn't add component to entity UID: " << entity_uid << "\n";
               std::cout << "\tComponent type " << typeid(Component_T).name() << " isn't registered\n";
               return false;
            }

            DefaultArchetype new_arch = entities_.getArchetype(entity_uid);
            new_arch.mergeSignature(component_sig);

            entities_.setArchetype(entity_uid, new_arch);
            components_.addComponent(entity_uid, component);

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

         bool registration_locked_;

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

         template <class T>
         void fancyRegisterComponents(void)
         {
            components_.registerComponent<T>();
         }

         template <class First, class...TheRest>
         auto fancyRegisterComponents(void) ->
            typename std::enable_if<sizeof...(TheRest) != 0, void>::type
         {
            components_.registerComponent<First>();
            fancyRegisterComponents<TheRest...>();
         }

   };
}

#endif
