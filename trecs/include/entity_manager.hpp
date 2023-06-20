#ifndef NODE_ENTITY_MANAGER_HEADER
#define NODE_ENTITY_MANAGER_HEADER

#include "archetype.hpp"
#include "ecs_types.hpp"

#include <cstddef>
#include <vector>

namespace trecs
{

   // The entity manager is essentially a thin wrapper around a deque. It
   // tracks all of the UIDs that have been given out, and all of the UIDs that
   // are still available. It also includes a signature system where signatures
   // indicate unique component types. Component types are positive powers of
   // two, and a signature for a given entity can be specified externally.
   class EntityManager
   {
      public:
         // Initialize an entity manager with a maximum entity UID.
         EntityManager(uid_t max_entity_uid);

         void clear(void);

         // Adds an active entity. Returns the entity UID if successful, or
         // returns -1 if unsuccessful.
         uid_t addEntity(void);

         // Removes an active entity.
         void removeEntity(uid_t entity_uid);

         // Returns true if the given entity UID is active in this manager.
         // Returns false if the given entity UID is not active.
         bool entityActive(uid_t entity_uid) const;

         // Returns all active entity UIDs.
         const std::vector<uid_t> & getUids(void) const;

         // Allows external designation of the archetype of a given entity.
         bool setArchetype(
            uid_t entity_uid, const DefaultArchetype & archetype
         );

         void addComponentSignature(
            uid_t entity_uid, signature_t component_sig
         );

         void removeComponentSignature(
            uid_t entity_uid, signature_t component_sig
         );

         // Returns the given entity's archetype
         DefaultArchetype getArchetype(uid_t entity_uid) const;

         std::size_t size(void) const;

      private:

         // Hard-coded meta-max entity UID. This is implicitly determined by
         // the bit-level encodings in the edge entity UID.
         const uid_t meta_max_entity_uid_;

         uid_t max_entity_uid_;

         std::vector<uid_t> uid_pool_;

         std::vector<uid_t> uids_;

         std::vector<DefaultArchetype> archetypes_;

   };

}

#endif
