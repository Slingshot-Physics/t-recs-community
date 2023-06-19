#include "entity_manager.hpp"

#include "complicated_types.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <iostream>
#include <set>
#include <vector>

typedef unsigned int uid_t;

TEST_CASE("entity manager respects max size")
{
   size_t max_size = 125;
   trecs::EntityManager em(max_size);

   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      em.addEntity();
      REQUIRE( em.size() <= max_size );
   }
}

TEST_CASE("all entities requested are unique")
{
   size_t max_size = 122;
   trecs::EntityManager em(max_size);

   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      em.addEntity();
   }

   std::set<uid_t> uids_verification;
   for (unsigned int i = 0; i < em.getUids().size(); ++i)
   {
      REQUIRE( uids_verification.insert(em.getUids()[i]).second );
   }
}

TEST_CASE("delete same UID multiple times")
{
   size_t max_size = 468;
   trecs::EntityManager em(max_size);

   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      em.addEntity();
   }

   for (unsigned int i = 0; i < 100; ++i)
   {
      em.removeEntity(max_size/2);
      REQUIRE(em.size() == max_size - 1);
   }
}

TEST_CASE("add til full delete til empty")
{
   size_t max_size = 468;
   trecs::EntityManager em(max_size);

   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      em.addEntity();
   }

   REQUIRE( em.size() == max_size );

   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      em.removeEntity(i / 4);
   }

   REQUIRE( em.size() == 0 );
}

// Verify that an archetype can be added to one entry. Verify that an archetype
// is reset when an entry is deleted. Verify that a entity is marked inactive
// after it's deleted.
TEST_CASE("add an archetype to one entry")
{
   size_t max_size = 468;
   trecs::EntityManager em(max_size);

   trecs::uid_t entity_uid = em.addEntity();

   REQUIRE(em.getArchetype(entity_uid) == 0);

   em.setArchetype(entity_uid, 16);

   REQUIRE(em.getArchetype(entity_uid) == 16);

   REQUIRE(em.entityActive(entity_uid));

   em.removeEntity(entity_uid);

   REQUIRE(em.getArchetype(entity_uid) == 0);
   REQUIRE(!em.entityActive(entity_uid));
}

TEST_CASE("add archetypes to existing entities")
{
   size_t max_size = 468;

   trecs::EntityManager em(max_size);

   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      trecs::uid_t entity_uid = em.addEntity();
      if (entity_uid >= 0)
      {
         REQUIRE(em.getArchetype(entity_uid) == 0);
      }

      if (entity_uid >= 0)
      {
         std::cout << "setting archetype for entity UID: " << entity_uid << "\n";
         trecs::archetype_t bogus_archetype = (max_size - entity_uid);
         REQUIRE(em.setArchetype(entity_uid, bogus_archetype));

         std::cout << "getting archetype for entity UID: " << entity_uid << "\n";
         trecs::archetype_t temp_archetype = em.getArchetype(entity_uid);

         std::cout << "entity archetype: " << temp_archetype << "\n";
         REQUIRE(temp_archetype == (max_size - entity_uid));
      }
   }

   const std::deque<trecs::uid_t> uids = em.getUids();
   REQUIRE(uids.size() == em.getUids().size());

   for (auto entity_uid : uids)
   {
      std::cout << "getting archetype for entity UID: " << entity_uid << "\n";
      REQUIRE(std::find(em.getUids().begin(), em.getUids().end(), entity_uid) != em.getUids().end());
      REQUIRE(em.getArchetype(entity_uid) == (max_size - entity_uid));
   }

   REQUIRE( em.size() == max_size );

   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      em.removeEntity(i / 4);
   }

   REQUIRE( em.size() == 0 );
}
