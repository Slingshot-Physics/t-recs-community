#ifndef ALLOCATOR_HEADER
#define ALLOCATOR_HEADER

// ECS stuff
#include "component_manager.hpp"
#include "ecs_types.hpp"
#include "entity_manager.hpp"
#include "pool_allocator.hpp"
#include "system_manager.hpp"
#include "query_manager.hpp"

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

         void clear(void);

         unsigned int maxEntities(void)
         {
            return max_num_entities_;
         }

         uid_t addEntity(void);

         uid_t addEntity(uid_t node_entity_a, uid_t node_entity_b);

         uid_t addEntity(uid_t node_entity);

         const std::deque<uid_t> & getEntities(void) const;

         void removeEntity(uid_t entity_uid);

         edge_t getEdge(uid_t edge_entity_uid);

         edge_t updateEdge(
            uid_t entity,
            uid_t node_entity_a,
            uid_t node_entity_b
         );

         edge_t updateEdge(uid_t entity, uid_t node_entity);

         template <typename Component_T>
         archetype_t getComponentArchetype(void) const
         {
            return signatureToArchetype(
               components_.getSignature<Component_T>()
            );
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

            archetype_t component_arch = getComponentArchetype<Component_T>();
            archetype_t old_arch = entities_.getArchetype(entity_uid);

            if ((old_arch & component_arch) > 0)
            {
               std::cout << "Component with signature " << component_arch << " already exists on entity " << entity_uid << "\n";
               return false;
            }

            archetype_t new_arch = old_arch | component_arch;
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

            archetype_t component_arch = getComponentArchetype<Component_T>();
            archetype_t old_arch = entities_.getArchetype(entity_uid);

            if ((old_arch & component_arch) > 0)
            {
               Component_T * old_component = components_.getComponent<Component_T>(entity_uid);
               if (old_component == nullptr)
               {
                  return false;
               }

               *old_component = component;
               return true;
            }

            archetype_t new_arch = old_arch | component_arch;
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

            archetype_t component_arch = getComponentArchetype<Component_T>();
            archetype_t entity_arch = entities_.getArchetype(entity_uid);

            if ((entity_arch & component_arch) == 0)
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

            archetype_t component_arch = getComponentArchetype<Component_T>();

            if (component_arch == 0)
            {
               return;
            }

            archetype_t old_arch = entities_.getArchetype(entity_uid);
            if ((old_arch & component_arch) == 0)
            {
               return;
            }

            components_.removeComponent<Component_T>(entity_uid);
            archetype_t new_arch = old_arch & (~component_arch);
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

         bool addArchetypeQuery(archetype_t arch)
         {
            if (arch == 0)
            {
               std::cout << "Invalid archetype " << arch << " requested for query.\n";
               return false;
            }

            return queries_.tryAddArchetype(arch);
         }

         template <class...Args>
         void addArchetypeQuery(Args...args)
         {
            archetype_t arch = 0;
            fancyAddArchetypeQuery(arch, args...);

            if (arch == 0)
            {
               std::cout << "Invalid archetype " << arch << " requested for query.\n";
               return;
            }

            queries_.tryAddArchetype(arch);
         }

         template <class...Args>
         archetype_t getArchetype(Args...args)
         {
            archetype_t arch = 0;
            fancyGetArchetype(arch, args...);
            return arch;
         }

         // Performs these operations on all registered systems:
         //    - Registers components
         //    - Registers archetype queries
         //    - Runs the initialize method
         void initializeSystems(void);

         auto getQueryEntities(archetype_t query) const -> const std::unordered_set<trecs::uid_t> &
         {
            if (!queries_.supportsArchetype(query))
            {
               return empty_set_;
            }

            return queries_.getArchetypeEntities().at(query);
         }

      private:

         unsigned int max_num_entities_;

         const std::unordered_set<uid_t> empty_set_;

         archetype_t edge_archetype_;

         EntityManager entities_;

         ComponentManager components_;

         SystemManager systems_;

         QueryManager queries_;

         // Removes `entity_uid` from all of the `trecs::edge_t` components on
         // the edge entities.
         void removeNodeEntityFromEdge(uid_t entity_uid);

         archetype_t signatureToArchetype(signature_t sig) const;

         void fancyAddArchetypeQuery(archetype_t & arch)
         {
            arch |= 0;
         }

         template <class First, class...TheRest>
         void fancyAddArchetypeQuery(archetype_t & arch, First f, TheRest...args)
         {
            (void)f;
            archetype_t component_arch = getComponentArchetype<First>();
            arch |= component_arch;
            fancyAddArchetypeQuery(arch, args...);
         }

         void fancyGetArchetype(archetype_t & arch)
         {
            arch |= 0;
         }

         template <class First, class...TheRest>
         void fancyGetArchetype(archetype_t & arch, First f, TheRest...args)
         {
            (void)f;
            archetype_t component_arch = getComponentArchetype<First>();
            arch |= component_arch;
            fancyGetArchetype(arch, args...);
         }

   };

}

#endif
