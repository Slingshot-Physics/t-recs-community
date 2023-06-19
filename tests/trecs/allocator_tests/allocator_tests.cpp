#include "allocator.hpp"

#include "blank_systems.hpp"
#include "complicated_types.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <vector>

TEST_CASE( "two components can't be added to one entity", "[allocator]")
{
   complicatedType_t<0> comp_a;
   comp_a.float_field = 3.14f;
   comp_a.int_field = -3;

   trecs::Allocator allocator;
   allocator.registerComponent<complicatedType_t<0> >();

   REQUIRE( allocator.getEntities().size() == 0 );

   int entity_id1 = allocator.addEntity();
   REQUIRE( allocator.addComponent(entity_id1, comp_a));

   int entity_id2 = allocator.addEntity();
   REQUIRE( allocator.addComponent(entity_id2, comp_a));

   REQUIRE( !allocator.addComponent(entity_id2, comp_a));
}

TEST_CASE( "body components be updated on one entity", "[allocator]")
{
   complicatedType_t<0> comp_a;
   comp_a.float_field = 1.f;
   comp_a.int_field = 0;

   trecs::Allocator allocator;
   allocator.registerComponent<complicatedType_t<0> >();

   REQUIRE( allocator.getEntities().size() == 0 );

   trecs::uid_t entity_id1 = allocator.addEntity();
   REQUIRE( allocator.updateComponent(entity_id1, comp_a) );
   REQUIRE( allocator.getComponent<complicatedType_t<0> >(entity_id1)->float_field == 1.f );

   comp_a.float_field = 2.5f;

   REQUIRE( allocator.updateComponent(entity_id1, comp_a) );
   REQUIRE( allocator.getComponent<complicatedType_t<0> >(entity_id1)->float_field == 2.5f );
}

TEST_CASE( "can't add components to inactive entity", "[allocator]")
{
   complicatedType_t<0> comp_a;
   comp_a.float_field = 1.f;
   comp_a.int_field = 0;

   trecs::Allocator allocator;
   allocator.registerComponent<complicatedType_t<0> >();

   REQUIRE( !allocator.addComponent(0, comp_a));

   REQUIRE( allocator.getEntities().size() == 0 );

   int entity_id1 = allocator.addEntity();
   REQUIRE( allocator.addComponent(entity_id1, comp_a));

   int entity_id2 = allocator.addEntity();
   REQUIRE( allocator.addComponent(entity_id2, comp_a));

   REQUIRE( !allocator.addComponent(7, comp_a));
}

TEST_CASE( "entities report correct components", "[allocator]")
{
   // complicatedType_t<0> comp_a;
   // comp_a.float_field = 1.f;
   // comp_a.int_field = 0;

   trecs::Allocator allocator;
   // allocator.registerComponent<complicatedType_t<0> >();

   trecs::uid_t entity = allocator.addEntity(12, 37);

   trecs::edge_t edge = allocator.getEdge(entity);

   REQUIRE((edge.edgeId == entity));
   REQUIRE( edge.nodeIdA == 12 );
   REQUIRE( edge.nodeIdB == 37 );
   REQUIRE( edge.flag == trecs::edge_flag::TRANSITIVE );
}

TEST_CASE( "edge components are deleted after entities are deleted", "[allocator]")
{
   // complicatedType_t<0> comp_a;
   // comp_a.float_field = 1.f;
   // comp_a.int_field = 0;

   trecs::Allocator allocator;
   // allocator.registerComponent<complicatedType_t<0> >();

   trecs::uid_t entity = allocator.addEntity(12, 37);

   trecs::edge_t edge = allocator.getEdge(entity);

   REQUIRE((edge.edgeId == entity));
   REQUIRE( edge.nodeIdA == 12 );
   REQUIRE( edge.nodeIdB == 37 );
   REQUIRE( edge.flag == trecs::edge_flag::TRANSITIVE );

   allocator.removeEntity(entity);

   const trecs::edge_t * edge_pointer = allocator.getComponent<trecs::edge_t>(entity);

   REQUIRE(edge_pointer == nullptr);
}

TEST_CASE( "changing the entity nodes on an edge component doesn't change entity uid", "[allocator]")
{
   // rigidBody_t testBody;
   // testBody.gravity = Vector3(0.0, 0.0, -9.8);
   // testBody.inertiaTensor = identityMatrix();
   // testBody.mass = 1;
   // testBody.ql2b[0] = 1.f;

   trecs::Allocator allocator;
   // allocator.registerComponent<rigidBody_t>();

   trecs::uid_t entity = allocator.addEntity(12, 37);

   allocator.updateEdge(entity, 22, 47);

   trecs::edge_t edge = allocator.getEdge(entity);

   REQUIRE( (edge.edgeId == entity) );
   REQUIRE( edge.nodeIdA == 22 );
   REQUIRE( edge.nodeIdB == 47 );
   REQUIRE( edge.flag == trecs::edge_flag::TRANSITIVE );

   allocator.updateEdge(entity, 47);

   edge = allocator.getEdge(entity);

   REQUIRE( (edge.edgeId == entity) );
   REQUIRE( edge.nodeIdA == -1 );
   REQUIRE( edge.nodeIdB == 47 );
   REQUIRE( edge.flag == trecs::edge_flag::NODE_A_TERMINAL );
}

TEST_CASE( "deleting an entity automatically updates the edges it's connected to", "[allocator]")
{
   // rigidBody_t testBody;
   // testBody.gravity = Vector3(0.0, 0.0, -9.8);
   // testBody.inertiaTensor = identityMatrix();
   // testBody.mass = 1;
   // testBody.ql2b[0] = 1.f;

   trecs::Allocator allocator;
   // allocator.registerComponent<rigidBody_t>();
   allocator.registerComponent<float>();

   trecs::uid_t node_id1 = allocator.addEntity();
   allocator.addComponent(node_id1, 12.45f);

   trecs::uid_t node_id2 = allocator.addEntity();
   allocator.addComponent(node_id2, -15.f);

   trecs::uid_t edge_id = allocator.addEntity(node_id1, node_id2);
   trecs::edge_t edge = allocator.getEdge(edge_id);

   REQUIRE(edge.edgeId == edge_id);
   REQUIRE(edge.nodeIdA == node_id1);
   REQUIRE(edge.nodeIdB == node_id2);
   REQUIRE(edge.flag == trecs::edge_flag_enum_t::TRANSITIVE);

   allocator.removeEntity(node_id1);

   edge = allocator.getEdge(edge_id);
   REQUIRE(edge.edgeId == edge_id);
   REQUIRE(edge.nodeIdA == -1);
   REQUIRE(edge.nodeIdB == node_id2);
   REQUIRE(edge.flag == trecs::edge_flag_enum_t::NODE_A_TERMINAL);
}

TEST_CASE( "register rando systems", "[allocator]" )
{
   trecs::Allocator allocator;
   SandoSystem * ss = allocator.registerSystem<SandoSystem>();
   NandoSystem * ns = allocator.registerSystem<NandoSystem>();

   REQUIRE( ss != nullptr );
   REQUIRE( ns != nullptr );
}

TEST_CASE( "registering the same system twice fails", "[allocator]" )
{
   trecs::Allocator allocator;
   SandoSystem * ss = allocator.registerSystem<SandoSystem>();
   SandoSystem * ss2 = allocator.registerSystem<SandoSystem>();

   REQUIRE( ss != nullptr );
   REQUIRE( ss2 == nullptr );
}

TEST_CASE( "the allocator queries get the correct archetypes", "[allocator]" )
{
   trecs::Allocator allocator;
   SandoSystem * ss = allocator.registerSystem<SandoSystem>();

   allocator.registerComponent<float>();
   allocator.registerComponent<int>();
   allocator.registerComponent<complicatedType_t<0> >();

   trecs::archetype_t float_arch = allocator.getComponentArchetype<float>();
   trecs::archetype_t int_arch = allocator.getComponentArchetype<int>();
   trecs::archetype_t rb_arch = allocator.getComponentArchetype<complicatedType_t<0> >();

   allocator.addArchetypeQuery(float_arch);
   allocator.addArchetypeQuery(int_arch);
   allocator.addArchetypeQuery(rb_arch);

   REQUIRE( allocator.getQueryEntities(float_arch).size() == 0 );
   REQUIRE( allocator.getQueryEntities(int_arch).size() == 0 );
   REQUIRE( allocator.getQueryEntities(rb_arch).size() == 0 );
}

TEST_CASE( "systems get the correct entities", "[allocator]" )
{
   trecs::Allocator allocator;
   SandoSystem * ss = allocator.registerSystem<SandoSystem>();
   NandoSystem * ns = allocator.registerSystem<NandoSystem>();

   allocator.registerComponent<float>();
   allocator.registerComponent<int>();
   allocator.registerComponent<complicatedType_t<0> >();

   trecs::archetype_t float_arch = allocator.getComponentArchetype<float>();
   trecs::archetype_t int_arch = allocator.getComponentArchetype<int>();
   trecs::archetype_t rb_arch = allocator.getComponentArchetype<complicatedType_t<0> >();

   allocator.addArchetypeQuery(float_arch);
   allocator.addArchetypeQuery(int_arch);
   allocator.addArchetypeQuery(rb_arch);

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 15; ++i)
   {
      entities.push_back(allocator.addEntity());
      allocator.addComponent(entities.back(), i);
   }

   for (int i = 0; i < 23; ++i)
   {
      entities.push_back(allocator.addEntity());
      allocator.addComponent(entities.back(), static_cast<float>(i) * 3.1f);
   }

   for (int i = 0; i < 9; ++i)
   {
      complicatedType_t<0> rb;
      // rb.mass = static_cast<float>(i);
      // rb.ql2b[0] = 1.f;
      // rb.id = i;
      rb.int_field = i;
      rb.float_field = 2.f * i - 3.f;

      entities.push_back(allocator.addEntity());
      allocator.addComponent(entities.back(), rb);
   }

   REQUIRE( allocator.getQueryEntities(float_arch).size() == 23 );
   REQUIRE( allocator.getQueryEntities(int_arch).size() == 15 );
   REQUIRE( allocator.getQueryEntities(rb_arch).size() == 9 );
}

// Verifies that attempting to add a query for unregistered components doesn't
// crash the Allocator.
TEST_CASE( "variadic query registration for unregistered components", "[allocator]" )
{
   trecs::Allocator allocator;

   allocator.addArchetypeQuery(
      float{},
      int{},
      complicatedType_t<0>{},
      complicatedType_t<1>{}
   );

   trecs::archetype_t arch = allocator.getArchetype(
      float{},
      int{},
      complicatedType_t<0>{},
      complicatedType_t<1>{}
   );

   REQUIRE( allocator.getQueryEntities(arch).size() == 0 );
}

TEST_CASE( "variadic query registration, component addition, component removal, and query retrieval for registered components", "[allocator]")
{
   trecs::Allocator allocator;

   allocator.registerComponent<int>();
   allocator.registerComponent<float>();
   allocator.registerComponent<complicatedType_t<0> >();
   allocator.registerComponent<complicatedType_t<1> >();

   allocator.addArchetypeQuery(
      float{},
      int{},
      complicatedType_t<0>{},
      complicatedType_t<1>{}
   );

   trecs::uid_t new_entity = allocator.addEntity();

   trecs::archetype_t arch = allocator.getArchetype(
      float{},
      int{},
      complicatedType_t<0>{},
      complicatedType_t<1>{}
   );

   allocator.addComponent(new_entity, complicatedType_t<0>{});
   REQUIRE( allocator.getQueryEntities(arch).size() == 0 );

   allocator.addComponent(new_entity, complicatedType_t<1>{});
   REQUIRE( allocator.getQueryEntities(arch).size() == 0 );

   allocator.addComponent(new_entity, int{});
   REQUIRE( allocator.getQueryEntities(arch).size() == 0 );

   allocator.addComponent(new_entity, float{});
   REQUIRE( allocator.getQueryEntities(arch).size() == 1 );

   allocator.removeComponent<float>(new_entity);
   REQUIRE( allocator.getQueryEntities(arch).size() == 0 );
}

TEST_CASE( "verify that adding and removing components changes query output", "[allocator]" )
{
   trecs::Allocator allocator;

   allocator.registerComponent<int>();
   allocator.registerComponent<float>();
   allocator.registerComponent<complicatedType_t<0> >();
   allocator.registerComponent<complicatedType_t<1> >();

   trecs::archetype_t archs[4] = {
      (
         // allocator.getComponentArchetype<int>()
         allocator.getArchetype(int{})
      ),
      (
         // allocator.getComponentArchetype<int>() |
         // allocator.getComponentArchetype<float>()
         allocator.getArchetype(int{}, float{})
      ),
      (
         // allocator.getComponentArchetype<int>() |
         // allocator.getComponentArchetype<float>() |
         // allocator.getComponentArchetype<rigidBody_t>()
         allocator.getArchetype(
            int{}, float{}, complicatedType_t<0>{}
         )
      ),
      (
         // allocator.getComponentArchetype<int>() |
         // allocator.getComponentArchetype<float>() |
         // allocator.getComponentArchetype<rigidBody_t>() |
         // allocator.getComponentArchetype<geometry::types::minkowskiDiffSimplex_t>()
         allocator.getArchetype(
            int{}, float{}, complicatedType_t<0>{}, complicatedType_t<1>{}
         )
      )
   };

   for (int i = 0; i < 4; ++i)
   {
      allocator.addArchetypeQuery(archs[i]);
   }

   trecs::uid_t new_entity = allocator.addEntity();

   allocator.addComponent(new_entity, int{});
   REQUIRE( allocator.getQueryEntities(archs[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[1]).size() == 0 );
   REQUIRE( allocator.getQueryEntities(archs[2]).size() == 0 );
   REQUIRE( allocator.getQueryEntities(archs[3]).size() == 0 );

   allocator.addComponent(new_entity, float{});
   REQUIRE( allocator.getQueryEntities(archs[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[1]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[2]).size() == 0 );
   REQUIRE( allocator.getQueryEntities(archs[3]).size() == 0 );

   allocator.addComponent(new_entity, complicatedType_t<0>{1, 2.f});
   REQUIRE( allocator.getQueryEntities(archs[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[1]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[2]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[3]).size() == 0 );

   allocator.addComponent(new_entity, complicatedType_t<1>{});
   REQUIRE( allocator.getQueryEntities(archs[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[1]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[2]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[3]).size() == 1 );

   allocator.removeComponent<float>(new_entity);
   REQUIRE( allocator.getQueryEntities(archs[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[1]).size() == 0 );
   REQUIRE( allocator.getQueryEntities(archs[2]).size() == 0 );
   REQUIRE( allocator.getQueryEntities(archs[3]).size() == 0 );

   allocator.addComponent(new_entity, float{});
   REQUIRE( allocator.getQueryEntities(archs[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[1]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[2]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[3]).size() == 1 );

   allocator.removeComponent<complicatedType_t<1> >(new_entity);
   REQUIRE( allocator.getQueryEntities(archs[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[1]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[2]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(archs[3]).size() == 0 );

}
