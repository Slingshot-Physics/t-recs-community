#include "entity_manager.hpp"

#include <algorithm>
#include <cassert>
#include <iostream>

namespace trecs
{

   EntityManager::EntityManager(uid_t max_entity_uid)
      : meta_max_entity_uid_((1 << 20) - 2)
      , max_entity_uid_(
         max_entity_uid > meta_max_entity_uid_ ? meta_max_entity_uid_ : max_entity_uid
      )
      , archetypes_(max_entity_uid_, 0)
   {
      assert(max_entity_uid_ > 0);

      if (max_entity_uid >= meta_max_entity_uid_)
      {
         std::cout << "Trying to allocate more than " << meta_max_entity_uid_ << " entities.\n";
         std::cout << "Capped max entity UID.\n";
      }

      for (uid_t i = 0; i < max_entity_uid_; ++i)
      {
         uid_pool_.push_back(i);
      }

      std::cout << "number of possible signatures: " << archetypes_.size() << "\n";
   }

   void EntityManager::clear(void)
   {
      uids_.clear();
      uid_pool_.clear();

      for (uid_t i = 0; i < max_entity_uid_; ++i)
      {
         uid_pool_.push_back(i);
      }

      for (uid_t i = 0; i < max_entity_uid_; ++i)
      {
         archetypes_[i] = 0;
      }
   }

   uid_t EntityManager::addEntity(void)
   {
      if (uid_pool_.size() == 0)
      {
         return -1;
      }

      uid_t new_entity_uid = uid_pool_.front();
      uid_pool_.pop_front();
      uids_.push_back(new_entity_uid);

      archetypes_[new_entity_uid] = 0;

      return new_entity_uid;
   }

   void EntityManager::removeEntity(uid_t removed_entity_uid)
   {
      auto uids_it = std::find(
         uids_.begin(), uids_.end(), removed_entity_uid
      );

      if (uids_it == uids_.end())
      {
         return;
      }

      uids_.erase(uids_it);
      uid_pool_.push_back(removed_entity_uid);

      archetypes_[removed_entity_uid] = 0;
   }

   bool EntityManager::entityActive(uid_t entity_uid) const
   {
      return std::find(uids_.begin(), uids_.end(), entity_uid) != uids_.end();
   }

   const std::deque<uid_t> & EntityManager::getUids(void) const
   {
      return uids_;
   }

   bool EntityManager::setArchetype(uid_t entity_uid, archetype_t archetype)
   {
      if (std::find(uids_.begin(), uids_.end(), entity_uid) == uids_.end())
      {
         std::cout << "Attempting to set the archetype of a non-existent entity\n";
         return false;
      }

      archetypes_[entity_uid] = archetype;
      return true;
   }

   archetype_t EntityManager::getArchetype(uid_t entity_uid) const
   {
      return archetypes_[entity_uid];
   }

   std::size_t EntityManager::size(void) const
   {
      return uids_.size();
   }

}
