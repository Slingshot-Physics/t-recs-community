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

   const std::vector<trecs::uid_t> uids = em.getUids();
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

TEST_CASE( "assignment operator", "[EntityManager]" )
{
   size_t max_size = 468;
   trecs::EntityManager em1(max_size);
   trecs::EntityManager em2(max_size);

   std::vector<trecs::uid_t> entities;

   // Add a bunch of entities with a bunch of non-zero archetypes
   for (int i = 0; i < 128; ++i)
   {
      entities.push_back(em1.addEntity());
      for (int j = (i / 2); j < 127; j += (2 * ((i % 2) == 0) + ((i % 2) != 0)))
      {
         em1.addComponentSignature(entities.back(), j);
      }

      REQUIRE( !em1.getArchetype(entities.back()).empty() );
   }

   em2 = em1;

   // Verify that the archetypes of both entity sets in the copied entity
   // manager have the same archetypes, and that both entity sets are active.
   for (const auto entity : entities)
   {
      REQUIRE( em2.entityActive(entity) );
      REQUIRE( em1.getArchetype(entity) == em2.getArchetype(entity) );
   }

   REQUIRE(em1.size() == em2.size());

   // Remove an entity, verify that the two EM's change.
   em1.removeEntity(entities.back());

   REQUIRE( em1.size() != em2.size() );
}

TEST_CASE( "assignment operator with empty entity manager", "[EntityManager]" )
{
   size_t max_size = 468;
   trecs::EntityManager em1(max_size);
   trecs::EntityManager em2(max_size);

   em2 = em1;

   REQUIRE( em1.size() == 0 );
   REQUIRE( em2.size() == 0 );
   REQUIRE( em1.size() == em2.size() );
}

TEST_CASE( "assignment operator with un-archetyped entities", "[EntityManager]" )
{
   size_t max_size = 468;
   trecs::EntityManager em1(max_size);
   trecs::EntityManager em2(max_size);

   for (int i = 0; i < max_size; ++i)
   {
      em1.addEntity();
   }

   em2 = em1;

   REQUIRE( em1.size() == em2.size() );

   for (const auto entity : em1.getUids())
   {
      REQUIRE( em1.entityActive(entity) );
      REQUIRE( em2.entityActive(entity) );

      REQUIRE( em1.getArchetype(entity).empty() );
      REQUIRE( em2.getArchetype(entity).empty() );
   }
}
