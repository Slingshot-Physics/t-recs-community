#include "allocator.hpp"

#include "blank_systems.hpp"
#include "complicated_types.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <vector>

TEST_CASE( "two components can't be added to one entity", "[Allocator]")
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

TEST_CASE( "can't add unregistered component types", "[Allocator]" )
{
   
   trecs::Allocator allocator;
   allocator.registerComponent<complicatedType_t<0> >();

   auto new_entity = allocator.addEntity();

   REQUIRE( !allocator.addComponent(new_entity, 3.f) );
}

TEST_CASE( "components can be updated on one entity", "[Allocator]")
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

TEST_CASE( "can't add components to inactive entity", "[Allocator]")
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

TEST_CASE( "entities report correct components", "[Allocator]")
{
   trecs::Allocator allocator;

   trecs::uid_t entity = allocator.addEntity(12, 37);

   trecs::edge_t edge = allocator.getEdge(entity);

   REQUIRE((edge.edgeId == entity));
   REQUIRE( edge.nodeIdA == 12 );
   REQUIRE( edge.nodeIdB == 37 );
   REQUIRE( edge.flag == trecs::edge_flag::TRANSITIVE );
}

TEST_CASE( "edge components are deleted after entities are deleted", "[Allocator]")
{
   trecs::Allocator allocator;

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

TEST_CASE( "changing the entity nodes on an edge component doesn't change entity uid", "[Allocator]")
{
   trecs::Allocator allocator;

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

TEST_CASE( "deleting an entity automatically updates the edges it's connected to", "[Allocator]")
{
   trecs::Allocator allocator;
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

TEST_CASE( "register rando systems", "[Allocator]" )
{
   trecs::Allocator allocator;
   SandoSystem * ss = allocator.registerSystem<SandoSystem>();
   NandoSystem * ns = allocator.registerSystem<NandoSystem>();

   REQUIRE( ss != nullptr );
   REQUIRE( ns != nullptr );
}

TEST_CASE( "registering the same system twice fails", "[Allocator]" )
{
   trecs::Allocator allocator;
   SandoSystem * ss = allocator.registerSystem<SandoSystem>();
   SandoSystem * ss2 = allocator.registerSystem<SandoSystem>();

   REQUIRE( ss != nullptr );
   REQUIRE( ss2 == nullptr );
}

TEST_CASE( "the allocator queries get the correct archetypes", "[Allocator]" )
{
   trecs::Allocator allocator;
   SandoSystem * ss = allocator.registerSystem<SandoSystem>();

   allocator.registerComponent<float>();
   allocator.registerComponent<int>();
   allocator.registerComponent<complicatedType_t<0> >();

   trecs::query_t float_query = allocator.addArchetypeQuery<float>();
   trecs::query_t int_query = allocator.addArchetypeQuery<int>();
   trecs::query_t rb_query = allocator.addArchetypeQuery<complicatedType_t<0> >();

   REQUIRE( allocator.getQueryEntities(float_query).size() == 0 );
   REQUIRE( allocator.getQueryEntities(int_query).size() == 0 );
   REQUIRE( allocator.getQueryEntities(rb_query).size() == 0 );
}

TEST_CASE( "systems get the correct entities", "[Allocator]" )
{
   trecs::Allocator allocator;
   SandoSystem * ss = allocator.registerSystem<SandoSystem>();
   NandoSystem * ns = allocator.registerSystem<NandoSystem>();

   allocator.registerComponent<float>();
   allocator.registerComponent<int>();
   allocator.registerComponent<complicatedType_t<0> >();

   trecs::query_t float_query = allocator.addArchetypeQuery<float>();
   trecs::query_t int_query = allocator.addArchetypeQuery<int>();
   trecs::query_t rb_query = allocator.addArchetypeQuery<complicatedType_t<0> >();

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
      rb.int_field = i;
      rb.float_field = 2.f * i - 3.f;

      entities.push_back(allocator.addEntity());
      allocator.addComponent(entities.back(), rb);
   }

   REQUIRE( allocator.getQueryEntities(float_query).size() == 23 );
   REQUIRE( allocator.getQueryEntities(int_query).size() == 15 );
   REQUIRE( allocator.getQueryEntities(rb_query).size() == 9 );
}

// Verifies that attempting to add a query for unregistered components doesn't
// crash the Allocator.
TEST_CASE( "variadic query registration for unregistered components", "[Allocator]" )
{
   trecs::Allocator allocator;

   trecs::query_t funky_query = allocator.addArchetypeQuery<
      float, int, complicatedType_t<0>, complicatedType_t<1>
   >();

   REQUIRE( allocator.getQueryEntities(funky_query).size() == 0 );
}

TEST_CASE( "variadic query registration, component addition, component removal, and query retrieval for registered components", "[Allocator]")
{
   trecs::Allocator allocator;

   allocator.registerComponent<int>();
   allocator.registerComponent<float>();
   allocator.registerComponent<complicatedType_t<0> >();
   allocator.registerComponent<complicatedType_t<1> >();

   trecs::query_t funky_query = allocator.addArchetypeQuery<
      float, int, complicatedType_t<0>, complicatedType_t<1>
   >();

   trecs::uid_t new_entity = allocator.addEntity();

   allocator.addComponent(new_entity, complicatedType_t<0>{});
   REQUIRE( allocator.getQueryEntities(funky_query).size() == 0 );

   allocator.addComponent(new_entity, complicatedType_t<1>{});
   REQUIRE( allocator.getQueryEntities(funky_query).size() == 0 );

   allocator.addComponent(new_entity, int{});
   REQUIRE( allocator.getQueryEntities(funky_query).size() == 0 );

   allocator.addComponent(new_entity, float{});
   REQUIRE( allocator.getQueryEntities(funky_query).size() == 1 );

   allocator.removeComponent<float>(new_entity);
   REQUIRE( allocator.getQueryEntities(funky_query).size() == 0 );
}

TEST_CASE( "verify that adding and removing components changes query output", "[Allocator]" )
{
   trecs::Allocator allocator;

   allocator.registerComponent<int>();
   allocator.registerComponent<float>();
   allocator.registerComponent<complicatedType_t<0> >();
   allocator.registerComponent<complicatedType_t<1> >();

   trecs::query_t queries[4] = {
      (
         allocator.addArchetypeQuery<int>()
      ),
      (
         allocator.addArchetypeQuery<int, float>()
      ),
      (
         allocator.addArchetypeQuery<
            int, float, complicatedType_t<0>
         >()
      ),
      (
         allocator.addArchetypeQuery<
            int, float, complicatedType_t<0>, complicatedType_t<1>
         >()
      )
   };

   trecs::uid_t new_entity = allocator.addEntity();

   allocator.addComponent(new_entity, int{});
   REQUIRE( allocator.getQueryEntities(queries[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[1]).size() == 0 );
   REQUIRE( allocator.getQueryEntities(queries[2]).size() == 0 );
   REQUIRE( allocator.getQueryEntities(queries[3]).size() == 0 );

   allocator.addComponent(new_entity, float{});
   REQUIRE( allocator.getQueryEntities(queries[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[1]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[2]).size() == 0 );
   REQUIRE( allocator.getQueryEntities(queries[3]).size() == 0 );

   allocator.addComponent(new_entity, complicatedType_t<0>{1, 2.f});
   REQUIRE( allocator.getQueryEntities(queries[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[1]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[2]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[3]).size() == 0 );

   allocator.addComponent(new_entity, complicatedType_t<1>{});
   REQUIRE( allocator.getQueryEntities(queries[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[1]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[2]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[3]).size() == 1 );

   allocator.removeComponent<float>(new_entity);
   REQUIRE( allocator.getQueryEntities(queries[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[1]).size() == 0 );
   REQUIRE( allocator.getQueryEntities(queries[2]).size() == 0 );
   REQUIRE( allocator.getQueryEntities(queries[3]).size() == 0 );

   allocator.addComponent(new_entity, float{});
   REQUIRE( allocator.getQueryEntities(queries[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[1]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[2]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[3]).size() == 1 );

   allocator.removeComponent<complicatedType_t<1> >(new_entity);
   REQUIRE( allocator.getQueryEntities(queries[0]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[1]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[2]).size() == 1 );
   REQUIRE( allocator.getQueryEntities(queries[3]).size() == 0 );
}

TEST_CASE( "verify adding the same archetype query multiple times results in the same query ID", "[Allocator]" )
{
   trecs::Allocator allocator;

   allocator.registerComponent<int>();
   allocator.registerComponent<float>();
   allocator.registerComponent<complicatedType_t<0> >();
   allocator.registerComponent<complicatedType_t<1> >();

   trecs::query_t queries[4] = {
      (
         allocator.addArchetypeQuery<int>()
      ),
      (
         allocator.addArchetypeQuery<int, float>()
      ),
      (
         allocator.addArchetypeQuery<int, float, complicatedType_t<0> >()
      ),
      (
         allocator.addArchetypeQuery<int, float, complicatedType_t<0>, complicatedType_t<1> >()
      )
   };

   trecs::query_t query_a = allocator.addArchetypeQuery<int, float>();
   trecs::query_t query_b = allocator.addArchetypeQuery<int, float>();
   trecs::query_t query_c = allocator.addArchetypeQuery<int, complicatedType_t<0>, float>();
   trecs::query_t query_d = allocator.addArchetypeQuery<int, complicatedType_t<0>, float>();

   REQUIRE( query_a == query_b );
   REQUIRE( query_c == query_d );
   REQUIRE( query_a != query_d );
}

TEST_CASE( "add one entity component buffer", "[Allocator]" )
{
   trecs::Allocator allocator;

   trecs::uid_t ecb_entity = allocator.addEntityComponentBuffer(120);

   REQUIRE( allocator.getComponent<trecs::EntityComponentBuffer>(ecb_entity) != nullptr );

   allocator.getComponent<trecs::EntityComponentBuffer>(ecb_entity)->registerComponents<int, float, double>();
}

TEST_CASE( "add multiple entity component buffers", "[Allocator]" )
{
   trecs::Allocator allocator;

   std::vector<trecs::uid_t> ecb_entities;

   ecb_entities.push_back(allocator.addEntityComponentBuffer(120));
   std::cout << "added entity " << ecb_entities.back() << "\n";
   ecb_entities.push_back(allocator.addEntityComponentBuffer(120));
   std::cout << "added entity " << ecb_entities.back() << "\n";
   ecb_entities.push_back(allocator.addEntityComponentBuffer(120));
   std::cout << "added entity " << ecb_entities.back() << "\n";
   ecb_entities.push_back(allocator.addEntityComponentBuffer(120));
   std::cout << "added entity " << ecb_entities.back() << "\n";
   ecb_entities.push_back(allocator.addEntityComponentBuffer(120));
   std::cout << "added entity " << ecb_entities.back() << "\n";

   for (const auto entity : ecb_entities)
   {
      std::cout << "entity: " << entity << "\n";
      REQUIRE( allocator.getComponent<trecs::EntityComponentBuffer>(entity) != nullptr );
   }

   // SFINAE
   // allocator.removeComponent<trecs::EntityComponentBuffer>(ecb_entities.back());
}

TEST_CASE( "add components to entity component buffer", "[Allocator]" )
{
   trecs::Allocator allocator;

   trecs::uid_t ecb_entity = allocator.addEntityComponentBuffer(120);

   auto ecb = allocator.getComponent<trecs::EntityComponentBuffer>(ecb_entity);
   ecb->registerComponents<int, float, double>();

   std::vector<trecs::uid_t> entities;

   entities.push_back(ecb->addEntity());
   ecb->updateComponent(entities.back(), 44);
   ecb->updateComponent(entities.back(), 44.9123f);
   ecb->updateComponent(entities.back(), 2.717819);

   REQUIRE( *ecb->getComponent<int>(entities.back()) == 44 );
   REQUIRE( *ecb->getComponent<float>(entities.back()) == 44.9123f );
   REQUIRE( *ecb->getComponent<double>(entities.back()) == 2.717819 );

   entities.push_back(ecb->addEntity());
   ecb->updateComponent(entities.back(), 44 * 2);
   ecb->updateComponent(entities.back(), 44.9123f * 2.f);
   ecb->updateComponent(entities.back(), 2.717819 * 2.0);

   REQUIRE( *ecb->getComponent<int>(entities.back()) == (44 * 2) );
   REQUIRE( *ecb->getComponent<float>(entities.back()) == (44.9123f * 2.f) );
   REQUIRE( *ecb->getComponent<double>(entities.back()) == (2.717819 * 2.0) );

   REQUIRE( *ecb->getComponent<int>(entities.front()) == 44 );
   REQUIRE( *ecb->getComponent<float>(entities.front()) == 44.9123f );
   REQUIRE( *ecb->getComponent<double>(entities.front()) == 2.717819 );
}

TEST_CASE( "add components to entity component buffer, delete entity", "[Allocator]" )
{
   trecs::Allocator allocator;

   trecs::uid_t ecb_entity = allocator.addEntityComponentBuffer(120);

   auto ecb = allocator.getComponent<trecs::EntityComponentBuffer>(ecb_entity);
   ecb->registerComponents<int, float, double>();

   std::vector<trecs::uid_t> entities;

   entities.push_back(ecb->addEntity());
   ecb->updateComponent(entities.back(), 44);
   ecb->updateComponent(entities.back(), 44.9123f);
   ecb->updateComponent(entities.back(), 2.717819);

   REQUIRE( *ecb->getComponent<int>(entities.back()) == 44 );
   REQUIRE( *ecb->getComponent<float>(entities.back()) == 44.9123f );
   REQUIRE( *ecb->getComponent<double>(entities.back()) == 2.717819 );

   entities.push_back(ecb->addEntity());
   ecb->updateComponent(entities.back(), 44 * 2);
   ecb->updateComponent(entities.back(), 44.9123f * 2.f);
   ecb->updateComponent(entities.back(), 2.717819 * 2.0);

   REQUIRE( *ecb->getComponent<int>(entities.back()) == (44 * 2) );
   REQUIRE( *ecb->getComponent<float>(entities.back()) == (44.9123f * 2.f) );
   REQUIRE( *ecb->getComponent<double>(entities.back()) == (2.717819 * 2.0) );

   REQUIRE( *ecb->getComponent<int>(entities.front()) == 44 );
   REQUIRE( *ecb->getComponent<float>(entities.front()) == 44.9123f );
   REQUIRE( *ecb->getComponent<double>(entities.front()) == 2.717819 );

   ecb->removeEntity(entities.front());

   REQUIRE( *ecb->getComponent<int>(entities.back()) == (44 * 2) );
   REQUIRE( *ecb->getComponent<float>(entities.back()) == (44.9123f * 2.f) );
   REQUIRE( *ecb->getComponent<double>(entities.back()) == (2.717819 * 2.0) );
}

TEST_CASE( "deny manually adding ECB", "[Allocator]")
{
   trecs::Allocator allocator;
   trecs::EntityComponentBuffer ecb(120);

   REQUIRE( !allocator.addComponent(allocator.addEntity(), ecb) );
}
