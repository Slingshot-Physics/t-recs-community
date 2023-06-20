#ifndef QUERY_MANAGER_HEADER
#define QUERY_MANAGER_HEADER

#include "archetype.hpp"
#include "ecs_types.hpp"

#include <iostream>
#include <map>
#include <unordered_set>
#include <vector>

namespace trecs
{
   class QueryManager
   {
      public:

         bool tryAddArchetype(const DefaultArchetype & arch)
         {
            for (const auto & typed_entities : archetypes_to_entities_)
            {
               if (typed_entities.first == arch)
               {
                  return false;
               }
            }

            archetypes_to_entities_[arch];
            archetypes_.push_back(arch);
            return true;
         }

         query_t addArchetypeQuery(const DefaultArchetype & arch)
         {
            if (archetypes_to_entities_.find(arch) == archetypes_to_entities_.end())
            {
               archetypes_to_entities_[arch];
               archetypes_.push_back(arch);
               return archetypes_.size() - 1;
            }

            for (query_t i = 0; i < archetypes_.size(); ++i)
            {
               if (archetypes_[i] == arch)
               {
                  return i;
               }
            }

            return error_query;
         }

         void moveEntity(
            trecs::uid_t entity,
            const DefaultArchetype & old_arch,
            const DefaultArchetype & new_arch
         )
         {
            // Any incoming entity's archetype must have *at least* all of the
            // bits in one of the stored archetypes in order to be added to the
            // mapping.
            // An incoming entity whose new archetype is the union of any two
            // existing archetypes will be added to the mappings for both
            // existing archetypes.

            // An entity should be placed in every archetype slot whose
            // archetype is a subset of the entity's archetype.

            // New entities might have archetypes that are a subset of one or
            // more existing archetypes. All existing archetypes are checked
            // for the new entity's potential membership.
            auto typed_entities_iter = archetypes_to_entities_.begin();

            for (
               typed_entities_iter = archetypes_to_entities_.begin();
               typed_entities_iter != archetypes_to_entities_.end();
               ++typed_entities_iter
            )
            {
               if (typed_entities_iter->first.supports(old_arch))
               {
                  typed_entities_iter->second.erase(entity);
               }
            }

            for (
               typed_entities_iter = archetypes_to_entities_.begin();
               typed_entities_iter != archetypes_to_entities_.end();
               ++typed_entities_iter
            )
            {
               if (typed_entities_iter->first.supports(new_arch))
               {
                  typed_entities_iter->second.insert(entity);
               }
            }
         }

         void removeEntity(trecs::uid_t entity)
         {
            for (auto & typed_entities : archetypes_to_entities_)
            {
               typed_entities.second.erase(entity);
            }
         }

         bool supportsArchetype(const DefaultArchetype & arch) const
         {
            for (const auto & typed_entities : archetypes_to_entities_)
            {
               if (typed_entities.first.supports(arch))
               {
                  return true;
               }
            }
            return false;
         }

         auto getArchetypeEntities(
            const DefaultArchetype & arch
         ) const -> const std::unordered_set<trecs::uid_t> &
         {
            return archetypes_to_entities_.at(arch);
         }

         auto getArchetypeEntities(
            const DefaultArchetype & arch
         ) -> std::unordered_set<trecs::uid_t > &
         {
            return archetypes_to_entities_.at(arch);
         }

         auto getArchetypeEntities(
            const query_t query_id
         ) const -> const std::unordered_set<trecs::uid_t> &
         {
            return archetypes_to_entities_.at(archetypes_.at(query_id));
         }

      private:

         std::map<DefaultArchetype, std::unordered_set<trecs::uid_t> > archetypes_to_entities_;

         std::vector<DefaultArchetype> archetypes_;

   };

}

#endif
