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
            , num_entities_(0)
            , components_(max_buffer_size_)
            , registration_locked_(false)
         { }
      
         EntityComponentBuffer(size_t max_buffer_size)
            : max_buffer_size_(max_buffer_size)
            , num_entities_(0)
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
            num_entities_ = other.num_entities_;
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
            num_entities_ = other.num_entities_;
            components_ = other.components_;
            registration_locked_ = other.registration_locked_;

            return *this;
         }

         // Resets all of the entities and components in the ECB.
         void clear(void)
         {
            num_entities_ = 0;
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
            if (static_cast<size_t>(num_entities_) >= max_buffer_size_)
            {
               return -1;
            }

            uid_t new_entity = num_entities_;
            ++num_entities_;
            return new_entity;
         }

         uid_t numEntities(void) const
         {
            return num_entities_;
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
            if (entity_uid >= num_entities_ || entity_uid < 0)
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

            return components_.addComponent(entity_uid, component) >= 0;
         }

         // Attempt to retrieve a component of a particular type from an entity
         // Returns nullptr if the requested entity is not active or if an
         // unsupported component type is used.
         template <typename Component_T>
         Component_T * getComponent(uid_t entity_uid)
         {
            if (entity_uid >= num_entities_ || entity_uid < 0)
            {
               std::cout << "ECB entity uid " << entity_uid << " inactive\n";
               return nullptr;
            }

            return components_.getComponent<Component_T>(entity_uid);
         }

         template <typename Component_T>
         ComponentArrayWrapper<Component_T> getComponents(void)
         {
            return components_.getComponents<Component_T>();
         }

      private:
         size_t max_buffer_size_;

         uid_t num_entities_;

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
