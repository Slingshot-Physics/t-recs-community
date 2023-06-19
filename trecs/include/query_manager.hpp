#ifndef QUERY_MANAGER_HEADER
#define QUERY_MANAGER_HEADER

#include "ecs_types.hpp"

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace trecs
{
   class QueryManager
   {
      public:

         bool tryAddArchetype(trecs::archetype_t arch)
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

         query_t addArchetypeQuery(trecs::archetype_t arch)
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
            trecs::archetype_t old_arch,
            trecs::archetype_t new_arch
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
               if ((typed_entities_iter->first & old_arch) == typed_entities_iter->first)
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
               if ((typed_entities_iter->first & new_arch) == typed_entities_iter->first)
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

         bool supportsArchetype(trecs::archetype_t arch) const
         {
            for (const auto & typed_entities : archetypes_to_entities_)
            {
               if ((typed_entities.first & arch) == typed_entities.first)
               {
                  return true;
               }
            }
            return false;
         }

         auto getArchetypeEntities(void) const -> const std::unordered_map<trecs::archetype_t, std::unordered_set<trecs::uid_t> > &
         {
            return archetypes_to_entities_;
         }

         auto getArchetypeEntities(void) -> std::unordered_map<trecs::archetype_t, std::unordered_set<trecs::uid_t> > &
         {
            return archetypes_to_entities_;
         }

         auto getArchetypeEntities(const query_t query_id) const -> const std::unordered_set<trecs::uid_t> &
         {
            return archetypes_to_entities_.at(archetypes_.at(query_id));
         }

      private:

         std::unordered_map<trecs::archetype_t, std::unordered_set<trecs::uid_t> > archetypes_to_entities_;

         std::vector<trecs::archetype_t> archetypes_;

   };

}

#endif
