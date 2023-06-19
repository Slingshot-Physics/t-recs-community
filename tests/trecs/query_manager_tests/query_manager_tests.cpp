#include "query_manager.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

trecs::DefaultArchetype archFromBits(int bits)
{
   trecs::DefaultArchetype arch;
   for (int i = 0; i < 32; ++i)
   {
      if ((bits & (1 << i)) > 0)
      {
         arch.mergeSignature(i);
      }
   }

   return arch;
}

TEST_CASE("instantiate", "[QueryManager]")
{
   trecs::QueryManager queries;
   REQUIRE( true );
}

TEST_CASE("add unique archetypes with query ID", "[QueryManager]")
{
   trecs::QueryManager queries;

   // 0b001100
   trecs::DefaultArchetype arch_a;
   arch_a.mergeSignature(2);
   arch_a.mergeSignature(3);

   // 0b001101
   trecs::DefaultArchetype arch_b = arch_a;
   arch_b.mergeSignature(0);

   // 0b101101
   trecs::DefaultArchetype arch_c = arch_b;
   arch_c.mergeSignature(5);

   // 0b010010
   trecs::DefaultArchetype arch_d;
   arch_d.mergeSignature(1);
   arch_d.mergeSignature(4);

   trecs::query_t query_a = queries.addArchetypeQuery(arch_a);
   trecs::query_t query_b = queries.addArchetypeQuery(arch_b);
   trecs::query_t query_c = queries.addArchetypeQuery(arch_c);
   trecs::query_t query_d = queries.addArchetypeQuery(arch_d);

   REQUIRE( query_a != query_b );
   REQUIRE( query_c != query_b );
   REQUIRE( query_c != query_d );
   REQUIRE( query_a != query_d );
}

TEST_CASE("add unique and non-unique archetypes with query ID", "[QueryManager]")
{
   trecs::QueryManager queries;

   trecs::DefaultArchetype arch;
   // 0b001100
   arch.mergeSignature(2);
   arch.mergeSignature(3);
   trecs::query_t query_a = queries.addArchetypeQuery(arch);

   // 0b001101
   arch.mergeSignature(0);
   trecs::query_t query_b = queries.addArchetypeQuery(arch);

   // 0b101101
   arch.mergeSignature(5);
   trecs::query_t query_c = queries.addArchetypeQuery(arch);

   // 0b010010
   trecs::DefaultArchetype arch2;
   arch2.mergeSignature(1);
   arch2.mergeSignature(4);
   trecs::query_t query_d = queries.addArchetypeQuery(arch2);

   // 0b010010
   trecs::query_t query_e = queries.addArchetypeQuery(arch2);

   // 0b101101
   trecs::query_t query_f = queries.addArchetypeQuery(arch);

   // 0b101101
   trecs::query_t query_g = queries.addArchetypeQuery(arch);

   REQUIRE( query_a != query_b );
   REQUIRE( query_c != query_b );
   REQUIRE( query_c != query_d );
   REQUIRE( query_a != query_d );
   REQUIRE( query_c == query_f );
   REQUIRE( query_c == query_g );
   REQUIRE( query_d == query_e );
}

// Verifies that entity sorting respects the notion that:
//    an entity's archetype satisfies a query if the entity's archetype has at
//    least the same bits as the query archetype.
TEST_CASE("retrieve entities from unique archetypes via query IDs", "[QueryManager]")
{
   trecs::QueryManager queries;
   trecs::DefaultArchetype blank;
   trecs::DefaultArchetype sig_a = archFromBits(0b001100);
   trecs::DefaultArchetype sig_b = archFromBits(0b001101);
   trecs::DefaultArchetype sig_c = archFromBits(0b101101);
   trecs::DefaultArchetype sig_d = archFromBits(0b010010);

   trecs::query_t query_a = queries.addArchetypeQuery(sig_a);
   trecs::query_t query_b = queries.addArchetypeQuery(sig_b);
   trecs::query_t query_c = queries.addArchetypeQuery(sig_c);
   trecs::query_t query_d = queries.addArchetypeQuery(sig_d);

   queries.moveEntity(0, blank, sig_a);

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

   queries.moveEntity(1, blank, sig_c);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 2);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 2);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 2);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 0);

   queries.moveEntity(2, blank, sig_c);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 0);

   queries.moveEntity(3, blank, sig_d);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 1);

   queries.moveEntity(4, blank, sig_a);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 4);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 1);

   queries.moveEntity(5, blank, sig_a);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 5);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 1);

}

TEST_CASE("add entities to mutually exclusive archetypes", "[QueryManager]")
{
   trecs::QueryManager queries;
   trecs::DefaultArchetype blank;
   trecs::DefaultArchetype sig_a = archFromBits(0b001100);
   trecs::DefaultArchetype sig_b = archFromBits(0b000001);
   trecs::DefaultArchetype sig_c = archFromBits(0b100010);
   trecs::DefaultArchetype sig_d = archFromBits(0b010000);
   trecs::query_t query_a = queries.addArchetypeQuery(sig_a);
   trecs::query_t query_b = queries.addArchetypeQuery(sig_b);
   trecs::query_t query_c = queries.addArchetypeQuery(sig_c);
   trecs::query_t query_d = queries.addArchetypeQuery(sig_d);

   queries.moveEntity(0, blank, sig_a);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 1);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 0);

   queries.moveEntity(0, sig_a, sig_b);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 1);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 0);

   queries.moveEntity(0, sig_b, sig_c);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 1);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 0);

   queries.moveEntity(1, blank, sig_c);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 2);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 0);

   queries.moveEntity(2, blank, sig_c);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 0);

   queries.moveEntity(3, blank, sig_d);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 1);

   queries.moveEntity(4, blank, sig_a);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 1);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 1);

   queries.moveEntity(5, blank, sig_a);

   REQUIRE(queries.getArchetypeEntities(query_a).size() == 2);
   REQUIRE(queries.getArchetypeEntities(query_b).size() == 0);
   REQUIRE(queries.getArchetypeEntities(query_c).size() == 3);
   REQUIRE(queries.getArchetypeEntities(query_d).size() == 1);

}

TEST_CASE("register one component archetype", "[QueryManager]" )
{
   trecs::QueryManager queries;

   queries.addArchetypeQuery(archFromBits(0b0011));

   for (uint64_t i = 0; i < (1 << 16); ++i)
   {
      if ((0b0011 & i) == 0b0011)
      {
         REQUIRE( queries.supportsArchetype(archFromBits(i)) );
      }
      else
      {
         REQUIRE( !queries.supportsArchetype(archFromBits(i)) );
      }
   }
}

// Verifies that two different archetype queries can be registered, and that
// and that neither system claims responsibility for the other's archetype.
TEST_CASE( "different systems, different archetypes", "[QueryManager]" )
{
   trecs::QueryManager queries;

   queries.addArchetypeQuery(archFromBits(0b0111));
   queries.addArchetypeQuery(archFromBits(0b1001));

   REQUIRE( queries.supportsArchetype(archFromBits(0b0111)) );
   REQUIRE( queries.supportsArchetype(archFromBits(0b11110111)) );
   REQUIRE( queries.supportsArchetype(archFromBits(0b1001)) );
   REQUIRE( queries.supportsArchetype(archFromBits(0b10101001)) );

}

TEST_CASE( "add entities with supported archetypes", "[QueryManager]" )
{
   trecs::QueryManager queries;

   trecs::query_t query_a = queries.addArchetypeQuery(archFromBits(0b0111));
   trecs::query_t query_b = queries.addArchetypeQuery(archFromBits(0b1001));

   for (unsigned i = 0; i < 16; ++i)
   {
      if (i < 8)
      {
         queries.moveEntity(i, archFromBits(0b1001), archFromBits(0b0111));
      }
      else
      {
         queries.moveEntity(i, archFromBits(0b0111), archFromBits(0b1001));
      }
   }

   REQUIRE( queries.getArchetypeEntities(query_a).size() == 8 );
   REQUIRE( queries.getArchetypeEntities(query_b).size() == 8 );
}

TEST_CASE( "add and remove node entities", "[QueryManager]" )
{
   trecs::QueryManager queries;

   archFromBits(0b01111).print();
   trecs::query_t query_a = queries.addArchetypeQuery(archFromBits(0b01111));

   archFromBits(0b01001).print();
   trecs::query_t query_b = queries.addArchetypeQuery(archFromBits(0b01001));

   for (unsigned i = 0; i < 16; ++i)
   {
      if (i < 8)
      {
         queries.moveEntity(i, archFromBits(0), archFromBits(0b1001));
      }
      else
      {
         queries.moveEntity(i, archFromBits(0), archFromBits(0b1111));
      }
   }

   REQUIRE( queries.getArchetypeEntities(query_a).size() == 8 );
   REQUIRE( queries.getArchetypeEntities(query_b).size() == 16 );

   queries.removeEntity(9);

   REQUIRE( queries.getArchetypeEntities(query_a).size() == 7 );
   REQUIRE( queries.getArchetypeEntities(query_b).size() == 15 );
}

// Tests the case where entities with archetypes that are unions of
// all of the registered archetype queries are added to every registered query.
// An entity should be placed in every archetype slot whose archetype is a
// subset of the entity's archetype.
TEST_CASE( "add node entities with archetype subset", "[QueryManager]" )
{
   trecs::QueryManager queries;

   trecs::DefaultArchetype ss_archetypes[2] = {
      archFromBits(0b10001),
      archFromBits(0b01001)
   };

   trecs::query_t query_a = queries.addArchetypeQuery(ss_archetypes[0]);
   trecs::query_t query_b = queries.addArchetypeQuery(ss_archetypes[1]);

   for (int i = 0; i < 8; ++i)
   {
      // This gets added to both registered queries
      queries.moveEntity(i, archFromBits(0), archFromBits(0b11001));

      // This gets added to just one of the registered queries
      queries.moveEntity(2 * i + 8, archFromBits(0), archFromBits(0b01001));
   }

   REQUIRE( queries.getArchetypeEntities(query_a).size() == 8 );
   REQUIRE( queries.getArchetypeEntities(query_b).size() == 16 );
   
}

TEST_CASE( "retrieve non-existent query from empty manager", "[QueryManager]" )
{
   trecs::QueryManager queries;
   REQUIRE(queries.getArchetypeEntities(123).size() == 0);
}

TEST_CASE( "retrieve non-existent query from non-empty manager", "[QueryManager]" )
{
   trecs::QueryManager queries;

   trecs::DefaultArchetype ss_archetypes[2] = {
      archFromBits(0b10001),
      archFromBits(0b01001)
   };

   trecs::query_t query_a = queries.addArchetypeQuery(ss_archetypes[0]);
   trecs::query_t query_b = queries.addArchetypeQuery(ss_archetypes[1]);

   REQUIRE(queries.getArchetypeEntities(123).size() == 0);
   REQUIRE(queries.getArchetypeEntities(13).size() == 0);
}
