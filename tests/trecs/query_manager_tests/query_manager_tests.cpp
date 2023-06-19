#include "query_manager.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

TEST_CASE("instantiate", "[ecs-query-manager]")
{
   trecs::QueryManager queries;
   REQUIRE( true );
}

TEST_CASE("add unique archetypes", "[ecs-query-manager]")
{
   trecs::QueryManager queries;
   bool result_a = queries.tryAddArchetype(0b001100);
   bool result_b = queries.tryAddArchetype(0b001101);
   bool result_c = queries.tryAddArchetype(0b101101);
   bool result_d = queries.tryAddArchetype(0b010010);

   REQUIRE(result_a);
   REQUIRE(result_b);
   REQUIRE(result_c);
   REQUIRE(result_d);
}

TEST_CASE("add unique archetypes with query ID", "[ecs-query-manager]")
{
   trecs::QueryManager queries;
   trecs::query_t query_a = queries.addArchetypeQuery(0b001100);
   trecs::query_t query_b = queries.addArchetypeQuery(0b001101);
   trecs::query_t query_c = queries.addArchetypeQuery(0b101101);
   trecs::query_t query_d = queries.addArchetypeQuery(0b010010);

   REQUIRE( query_a != query_b );
   REQUIRE( query_c != query_b );
   REQUIRE( query_c != query_d );
   REQUIRE( query_a != query_d );
}

TEST_CASE("add unique and non-unique archetypes", "[ecs-query-manager]")
{
   trecs::QueryManager queries;
   bool result_a = queries.tryAddArchetype(0b001100);
   bool result_b = queries.tryAddArchetype(0b001101);
   bool result_c = queries.tryAddArchetype(0b101101);
   bool result_d = queries.tryAddArchetype(0b010010);
   bool result_e = queries.tryAddArchetype(0b010010);
   bool result_f = queries.tryAddArchetype(0b101101);
   bool result_g = queries.tryAddArchetype(0b101101);

   REQUIRE( !result_e );
   REQUIRE( !result_f );
   REQUIRE( !result_g );
}

TEST_CASE("add unique and non-unique archetypes with query ID", "[ecs-query-manager]")
{
   trecs::QueryManager queries;
   trecs::query_t query_a = queries.addArchetypeQuery(0b001100);
   trecs::query_t query_b = queries.addArchetypeQuery(0b001101);
   trecs::query_t query_c = queries.addArchetypeQuery(0b101101);
   trecs::query_t query_d = queries.addArchetypeQuery(0b010010);
   trecs::query_t query_e = queries.addArchetypeQuery(0b010010);
   trecs::query_t query_f = queries.addArchetypeQuery(0b101101);
   trecs::query_t query_g = queries.addArchetypeQuery(0b101101);

   REQUIRE( query_a != query_b );
   REQUIRE( query_c != query_b );
   REQUIRE( query_c != query_d );
   REQUIRE( query_a != query_d );
   REQUIRE( query_c == query_f );
   REQUIRE( query_c == query_g );
   REQUIRE( query_d == query_e );
}

TEST_CASE("query manager crashes on invalid query ID", "[ecs-query-manager]")
{
   trecs::QueryManager queries;
   bool caught_exception = false;
   try
   {
      queries.getArchetypeEntities(100);
   }
   catch(const std::exception& e)
   {
      std::cerr << e.what() << '\n';
      caught_exception = true;
   }
   
   REQUIRE( caught_exception );
}

// Verifies that entity sorting respects the notion that:
//    an entity's archetype satisfies a query if the entity's archetype has at
//    least the same bits as the query archetype.
TEST_CASE("add entities to unique but non-mutually exclusive archetypes", "[ecs-query-manager]")
{
   trecs::QueryManager queries;
   trecs::archetype_t sig_a = 0b001100;
   trecs::archetype_t sig_b = 0b001101;
   trecs::archetype_t sig_c = 0b101101;
   trecs::archetype_t sig_d = 0b010010;
   bool result_a = queries.tryAddArchetype(sig_a);
   bool result_b = queries.tryAddArchetype(sig_b);
   bool result_c = queries.tryAddArchetype(sig_c);
   bool result_d = queries.tryAddArchetype(sig_d);

   queries.moveEntity(0, 0, sig_a);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 1);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 0);

   queries.moveEntity(0, sig_a, sig_b);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 1);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 1);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 0);

   queries.moveEntity(0, sig_b, sig_c);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 1);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 1);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 1);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 0);

   queries.moveEntity(1, 0, sig_c);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 2);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 2);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 2);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 0);

   queries.moveEntity(2, 0, sig_c);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 0);

   queries.moveEntity(3, 0, sig_d);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 1);

   queries.moveEntity(4, 0, sig_a);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 4);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 1);

   queries.moveEntity(5, 0, sig_a);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 5);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 1);

}

// Verifies that entity sorting respects the notion that:
//    an entity's archetype satisfies a query if the entity's archetype has at
//    least the same bits as the query archetype.
TEST_CASE("retrieve entities from unique archetypes via query IDs", "[ecs-query-manager]")
{
   trecs::QueryManager queries;
   trecs::archetype_t sig_a = 0b001100;
   trecs::archetype_t sig_b = 0b001101;
   trecs::archetype_t sig_c = 0b101101;
   trecs::archetype_t sig_d = 0b010010;

   trecs::query_t query_a = queries.addArchetypeQuery(sig_a);
   trecs::query_t query_b = queries.addArchetypeQuery(sig_b);
   trecs::query_t query_c = queries.addArchetypeQuery(sig_c);
   trecs::query_t query_d = queries.addArchetypeQuery(sig_d);

   queries.moveEntity(0, 0, sig_a);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 1);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 0);

   queries.moveEntity(0, sig_a, sig_b);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 1);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 1);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 0);

   queries.moveEntity(0, sig_b, sig_c);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 1);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 1);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 1);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 0);

   queries.moveEntity(1, 0, sig_c);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 2);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 2);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 2);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 0);

   queries.moveEntity(2, 0, sig_c);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 0);

   queries.moveEntity(3, 0, sig_d);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 1);

   queries.moveEntity(4, 0, sig_a);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 4);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 1);

   queries.moveEntity(5, 0, sig_a);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 5);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 1);

}

TEST_CASE("add entities to mutually exclusive archetypes", "[ecs-query-manager]")
{
   trecs::QueryManager queries;
   trecs::archetype_t sig_a = 0b001100;
   trecs::archetype_t sig_b = 0b000001;
   trecs::archetype_t sig_c = 0b100010;
   trecs::archetype_t sig_d = 0b010000;
   bool result_a = queries.tryAddArchetype(sig_a);
   bool result_b = queries.tryAddArchetype(sig_b);
   bool result_c = queries.tryAddArchetype(sig_c);
   bool result_d = queries.tryAddArchetype(sig_d);

   queries.moveEntity(0, 0, sig_a);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 1);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 0);

   queries.moveEntity(0, sig_a, sig_b);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 1);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 0);

   queries.moveEntity(0, sig_b, sig_c);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 1);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 0);

   queries.moveEntity(1, 0, sig_c);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 2);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 0);

   queries.moveEntity(2, 0, sig_c);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 0);

   queries.moveEntity(3, 0, sig_d);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 1);

   queries.moveEntity(4, 0, sig_a);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 1);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 1);

   queries.moveEntity(5, 0, sig_a);

   REQUIRE(queries.getArchetypeEntities().at(sig_a).size() == 2);
   REQUIRE(queries.getArchetypeEntities().at(sig_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities().at(sig_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities().at(sig_d).size() == 1);

}

TEST_CASE("register one component archetype", "[ecs-query-manager]" )
{
   trecs::QueryManager queries;

   queries.tryAddArchetype(0b0011);

   for (uint64_t i = 0; i < (1 << 16); ++i)
   {
      if ((0b0011 & i) == 0b0011)
      {
         REQUIRE( queries.supportsArchetype(i) );
      }
      else
      {
         REQUIRE( !queries.supportsArchetype(i) );
      }
   }
}

// Verifies that two different archetype queries can be registered, and that
// and that neither system claims responsibility for the other's archetype.
TEST_CASE( "different systems, different archetypes", "[ecs-query-manager]" )
{
   trecs::QueryManager queries;

   queries.tryAddArchetype(0b0111);
   queries.tryAddArchetype(0b1001);

   REQUIRE( queries.supportsArchetype(0b0111) );
   REQUIRE( queries.supportsArchetype(0b11110111) );
   REQUIRE( queries.supportsArchetype(0b1001) );
   REQUIRE( queries.supportsArchetype(0b10101001) );

}

TEST_CASE( "add entities with supported archetypes", "[ecs-query-manager]" )
{
   trecs::QueryManager queries;

   queries.tryAddArchetype(0b0111);
   queries.tryAddArchetype(0b1001);

   for (unsigned i = 0; i < 16; ++i)
   {
      if (i < 8)
      {
         queries.moveEntity(i, 0b1001, 0b0111);
      }
      else
      {
         queries.moveEntity(i, 0b0111, 0b1001);
      }
   }

   REQUIRE( queries.getArchetypeEntities().at(0b0111).size() == 8 );
   REQUIRE( queries.getArchetypeEntities().at(0b1001).size() == 8 );
}

TEST_CASE( "add and remove node entities", "[ecs-query-manager]" )
{
   trecs::QueryManager queries;

   REQUIRE( queries.tryAddArchetype(0b01111) );
   REQUIRE( queries.tryAddArchetype(0b01001) );

   for (unsigned i = 0; i < 16; ++i)
   {
      if (i < 8)
      {
         queries.moveEntity(i, 0, 0b1001);
      }
      else
      {
         queries.moveEntity(i, 0, 0b1111);
      }
   }

   REQUIRE( queries.getArchetypeEntities().at(0b01111).size() == 8 );
   REQUIRE( queries.getArchetypeEntities().at(0b01001).size() == 16 );

   queries.removeEntity(9);

   REQUIRE( queries.getArchetypeEntities().at(0b01111).size() == 7 );
   REQUIRE( queries.getArchetypeEntities().at(0b01001).size() == 15 );
}

// Tests the case where entities with archetypes that are unions of
// all of the registered archetype queries are added to every registered query.
// An entity should be placed in every archetype slot whose archetype is a
// subset of the entity's archetype.
TEST_CASE( "add node entities with archetype subset", "[ecs-query-manager]" )
{
   trecs::QueryManager queries;

   trecs::archetype_t ss_archetypes[2] = {0b10001, 0b01001};

   REQUIRE( queries.tryAddArchetype(ss_archetypes[0]) );
   REQUIRE( queries.tryAddArchetype(ss_archetypes[1]) );

   for (int i = 0; i < 8; ++i)
   {
      // This gets added to both registered queries
      queries.moveEntity(i, 0, 0b11001);

      // This gets added to just one of the registered queries
      queries.moveEntity(2 * i + 8, 0, 0b01001);
   }

   REQUIRE( queries.getArchetypeEntities().at(ss_archetypes[0]).size() == 8 );
   REQUIRE( queries.getArchetypeEntities().at(ss_archetypes[1]).size() == 16 );
   
}
