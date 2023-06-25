#include "entity_component_buffer.hpp"

namespace trecs
{
   uid_t EntityComponentBuffer::addEntity(void)
   {
      return entities_.addEntity();
   }

   void EntityComponentBuffer::removeEntity(uid_t entity_uid)
   {
      entities_.removeEntity(entity_uid);
      components_.removeComponents(entity_uid);
   }

   size_t EntityComponentBuffer::numEntities(void) const
   {
      return entities_.size();
   }
}
