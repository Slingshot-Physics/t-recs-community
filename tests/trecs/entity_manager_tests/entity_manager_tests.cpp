#include "entity_manager.hpp"

#include "complicated_types.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <iostream>
#include <set>
#include <vector>

TEST_CASE( "entity manager respects max size", "[EntityManager]" )
{
   size_t max_size = 125;
   trecs::EntityManager em(max_size);

   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      em.addEntity();
      REQUIRE( em.size() <= max_size );
   }
}

TEST_CASE( "all entities requested are unique", "[EntityManager]" )
{
   size_t max_size = 122;
   trecs::EntityManager em(max_size);

   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      em.addEntity();
   }

   std::set<trecs::uid_t> uids_verification;
   for (unsigned int i = 0; i < em.getUids().size(); ++i)
   {
      REQUIRE( uids_verification.insert(em.getUids()[i]).second );
   }
}

TEST_CASE( "delete same UID multiple times", "[EntityManager]" )
{
   size_t max_size = 468;
   trecs::EntityManager em(max_size);

   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      em.addEntity();
   }

   for (unsigned int i = 0; i < 100; ++i)
   {
      em.removeEntity(max_size / 2);
      REQUIRE( em.size() == max_size - 1 );
   }
}

TEST_CASE( "add til full delete til empty", "[EntityManager]" )
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

TEST_CASE( "add one component signature", "[EntityManager]" )
{
   size_t max_size = 468;
   trecs::EntityManager em(max_size);

   auto entity = em.addEntity();
   trecs::signature_t rando_signature = 75;
   em.addComponentSignature(entity, rando_signature);

   REQUIRE( em.getArchetype(entity).supports(rando_signature));
}

TEST_CASE( "remove one component signature", "[EntityManager]" )
{
   size_t max_size = 468;
   trecs::EntityManager em(max_size);

   auto entity = em.addEntity();
   trecs::signature_t rando_signature = 75;
   em.addComponentSignature(entity, rando_signature);

   em.removeComponentSignature(entity, rando_signature);
   REQUIRE( !em.getArchetype(entity).supports(rando_signature) );
}

// Verify that a component signature can be added to one entry. Verify that an
// archetype is reset when an entry is deleted. Verify that a entity is marked
// inactive after it's deleted.
TEST_CASE( "add an archetype to one entry", "[EntityManager]" )
{
   size_t max_size = 468;
   trecs::EntityManager em(max_size);

   trecs::uid_t entity = em.addEntity();

   REQUIRE( em.getArchetype(entity).empty() );

   trecs::DefaultArchetype arch;
   arch.mergeSignature(16);

   em.setArchetype(entity, arch);

   REQUIRE( em.getArchetype(entity).supports(16) );
   REQUIRE( em.getArchetype(entity).supports(arch) );

   REQUIRE( em.entityActive(entity) );

   em.removeEntity(entity);

   REQUIRE( em.getArchetype(entity).empty() );
   REQUIRE( !em.entityActive(entity) );
}

TEST_CASE( "add archetypes to existing entities", "[EntityManager]" )
{
   size_t max_size = 468;
   trecs::EntityManager em(max_size);

   // Add many more than the maximum number of entities.
   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      trecs::uid_t entity = em.addEntity();
      if (entity >= 0)
      {
         REQUIRE( em.getArchetype(entity).empty() );
      }

      // Add an archetype to all valid entities.
      if (entity >= 0)
      {
         trecs::DefaultArchetype arch;
         arch.mergeSignature(entity % trecs::max_num_signatures);

         std::cout << "setting archetype for entity UID: " << entity << "\n";
         em.setArchetype(entity, arch);

         std::cout << "getting archetype for entity UID: " << entity << "\n";
         REQUIRE( em.getArchetype(entity).supports(entity % trecs::max_num_signatures) );
         REQUIRE( em.getArchetype(entity).supports(arch) );
      }

      // Verify that only negative entities are returned once the number of
      // entities in the manager exceeds the maximum.
      if (i >= max_size)
      {
         REQUIRE( entity < 0 );
      }
   }

   const std::deque<trecs::uid_t> uids = em.getUids();
   REQUIRE( uids.size() == em.getUids().size() );

   // Verify that all of the archetypes that were set during the big loop are
   // still correct.
   for (auto entity_uid : uids)
   {
      std::cout << "getting archetype for entity UID: " << entity_uid << "\n";
      REQUIRE( std::find(em.getUids().begin(), em.getUids().end(), entity_uid) != em.getUids().end() );
      REQUIRE( em.getArchetype(entity_uid).supports(entity_uid % trecs::max_num_signatures) );
   }

   REQUIRE( em.size() == max_size );

   // Remove all of the entities in a weird way and verify that the entity
   // manager is empty at the end.
   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      em.removeEntity(i / 4);
   }

   REQUIRE( em.size() == 0 );
}
