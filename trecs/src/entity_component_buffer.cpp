#include "entity_component_buffer.hpp"

namespace trecs
{
   EntityComponentBuffer & EntityComponentBuffer::operator=(
      const EntityComponentBuffer & other
   )
   {
      if (this == &other)
      {
         return *this;
      }

      max_buffer_size_ = other.max_buffer_size_;
      entities_ = other.entities_;
      components_ = other.components_;

      return *this;
   }

   EntityComponentBuffer & EntityComponentBuffer::operator=(
      EntityComponentBuffer & other
   )
   {
      if (this == &other)
      {
         return *this;
      }

      max_buffer_size_ = other.max_buffer_size_;
      entities_ = other.entities_;
      components_ = other.components_;

      return *this;
   }

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

   size_t EntityComponentBuffer::numSignatures(void) const
   {
      return components_.getNumSignatures();
   }

   void EntityComponentBuffer::release(void)
   {
      components_.release();
   }
}
