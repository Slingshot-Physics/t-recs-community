#ifndef ALLOCATOR_HEADER
#define ALLOCATOR_HEADER

#include "archetype.hpp"
#include "component_manager.hpp"
#include "ecs_types.hpp"
#include "entity_manager.hpp"
#include "pool_allocator.hpp"
#include "system_manager.hpp"
#include "query_manager.hpp"

#include <type_traits>
#include <vector>

namespace trecs
{
   // The allocator tracks all of the structs associated with each rigid body.
   // The allocator does not enforce relationships between data structures, it
   // just stores data and makes it easy to add, modify, and remove.
   class Allocator
   {
      public:
         Allocator(void);

         Allocator(unsigned int max_num_entities);

         unsigned int maxEntities(void) const
         {
            return max_num_entities_;
         }

         uid_t addEntity(void);

         uid_t addEntity(uid_t node_entity_a, uid_t node_entity_b);

         uid_t addEntity(uid_t node_entity);

         const std::vector<uid_t> & getEntities(void) const;

         void removeEntity(uid_t entity_uid);

         edge_t getEdge(uid_t edge_entity_uid);

         edge_t updateEdge(
            uid_t entity,
            uid_t node_entity_a,
            uid_t node_entity_b
         );

         edge_t updateEdge(uid_t entity, uid_t node_entity);

         template <typename Component_T>
         signature_t getComponentSignature(void) const
         {
            return components_.getSignature<Component_T>();
         }

         // Attempts to add a component to an entity UID. If the entity UID is
         // inactive or if the component type isn't supported, or if another
         // component of the same supported type is already associated with
         // this entity, return false.
         // Returns true if the component is assigned to the entity
         // successfully.
         template <typename Component_T>
         bool addComponent(uid_t entity_uid, const Component_T & component)
         {
            if (!entities_.entityActive(entity_uid))
            {
               std::cout << "Entity uid " << entity_uid << " inactive\n";
               return false;
            }

            DefaultArchetype old_arch = entities_.getArchetype(entity_uid);
            signature_t component_sig = getComponentSignature<Component_T>();

            if (old_arch.supports(component_sig))
            {
               std::cout << "Component with signature " << component_sig << " already exists on entity " << entity_uid << "\n";
               return false;
            }

            DefaultArchetype new_arch = old_arch;
            new_arch.mergeSignature(component_sig);
            components_.addComponent<Component_T>(entity_uid, component);
            entities_.setArchetype(entity_uid, new_arch);
            queries_.moveEntity(entity_uid, old_arch, new_arch);

            return true;
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
            signature_t component_sig = getComponentSignature<Component_T>();

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
            queries_.moveEntity(entity_uid, old_arch, new_arch);

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
            signature_t component_sig = getComponentSignature<Component_T>();

            if (!old_arch.supports(component_sig))
            {
               return nullptr;
            }

            return components_.getComponent<Component_T>(entity_uid);
         }

         // Attempt to retrieve a component of a particular type from an
         // entity. Returns nullptr if the requested entity is not active or if
         // an unsupported component type is used.
         template <typename Component_T>
         const Component_T * getComponent(uid_t entity_uid) const
         {
            if (!entities_.entityActive(entity_uid))
            {
               return nullptr;
            }

            DefaultArchetype old_arch = entities_.getArchetype(entity_uid);
            signature_t component_sig = getComponentSignature<Component_T>();

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

         template <typename Component_T>
         void removeComponent(uid_t entity_uid)
         {
            if (!entities_.entityActive(entity_uid))
            {
               return;
            }

            signature_t component_sig = getComponentSignature<Component_T>();

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
            queries_.moveEntity(entity_uid, old_arch, new_arch);
         }

         template <typename Component_T>
         void registerComponent(void)
         {
            components_.registerComponent<Component_T>();
         }

         template <typename System_T>
         System_T * registerSystem(void)
         {
            return systems_.registerSystem<System_T>();
         }

         // Registers an archetype query with T-RECS. Returns a query ID for
         // the requested archetype. This method is called as:
         //
         //    allocator.addArchetypeQuery<int, float, complicatedType>();
         //
         // with as many registered component types as desired.
         template <class...Args>
         query_t addArchetypeQuery(void)
         {
            DefaultArchetype arch;
            fancierGetArchetype<Args...>(arch);

            if (arch.empty())
            {
               std::cout << "Empty archetype requested for query.\n";
               return error_query;
            }

            return queries_.addArchetypeQuery(arch);
         }

         // Returns an archetype object based on the component types provided.
         // This method is called as:
         //
         //    allocator.getArchetype<int, float, complicatedType>();
         //
         // with as many registered component types as desired. This method is
         // not used to register archetype queries with T-RECS. To register
         // archetype queries, see 'addArchetypeQuery'.
         template <class...Args>
         DefaultArchetype getArchetype(void) const
         {
            DefaultArchetype arch;
            fancierGetArchetype<Args...>(arch);
            return arch;
         }

         // Performs these operations on all registered systems:
         //    - Registers components
         //    - Registers archetype queries
         //    - Runs the initialize method
         void initializeSystems(void);

         // Retrieves an unordered set of entities that match a particular
         // archetype query.
         auto getQueryEntities(const query_t arch_query) const -> const std::unordered_set<trecs::uid_t> &
         {
            return queries_.getArchetypeEntities(arch_query);
         }

      private:

         unsigned int max_num_entities_;

         const std::unordered_set<uid_t> empty_set_;

         query_t edge_query_;

         EntityManager entities_;

         ComponentManager components_;

         SystemManager systems_;

         QueryManager queries_;

         // Removes `entity_uid` from all of the `trecs::edge_t` components on
         // the edge entities.
         void removeNodeEntityFromEdge(uid_t entity_uid);

         template <class T>
         void fancierGetArchetype(DefaultArchetype & arch) const
         {
            arch.mergeSignature(getComponentSignature<T>());
         }

         template <class First, class...TheRest>
         auto fancierGetArchetype(DefaultArchetype & arch) const -> 
            typename std::enable_if<sizeof...(TheRest) != 0, void>::type
         {
            arch.mergeSignature(getComponentSignature<First>());
            fancierGetArchetype<TheRest...>(arch);
         }

   };

}

#endif
