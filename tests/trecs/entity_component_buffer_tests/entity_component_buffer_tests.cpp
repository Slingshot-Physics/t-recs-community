#include "entity_component_buffer.hpp"

#include "complicated_types.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <vector>

TEST_CASE( "instantiation", "[EntityComponentBuffer]" )
{
   trecs::EntityComponentBuffer ecb(256);
}

// Verifies that an ECB with some set of registered component types correctly
// reports that it supports some user requested typelist.
TEST_CASE( "correctly reports supported types", "[EntityComponentBuffer]" )
{
   trecs::EntityComponentBuffer ecb(256);
   ecb.registerComponent<int>();
   ecb.registerComponent<float>();

   REQUIRE( ecb.supportsComponents<int, float>() );
   REQUIRE( ecb.supportsComponents<float, int>() );

   REQUIRE( !ecb.supportsComponents<float, int, double>() );
   REQUIRE( !ecb.supportsComponents<double>() );
   REQUIRE( !ecb.supportsComponents<complicatedType_t<77> >() );

   ecb.registerComponent<complicatedType_t<77> >();
   REQUIRE( ecb.supportsComponents<complicatedType_t<77> >() );
}

TEST_CASE( "add one entity", "[EntityComponentBuffer]" )
{
   trecs::EntityComponentBuffer ecb(256);
   REQUIRE( ecb.addEntity() >= 0 );
}

// Verifies that the ECB returns entities >= 0 while the entity count is
// below the maximum number of entities.
TEST_CASE( "add more than the maximum number of entities", "[EntityComponentBuffer]" )
{
   size_t max_size = 256;
   trecs::EntityComponentBuffer ecb(max_size);
   
   for (unsigned int i = 0; i < 5 * max_size; ++i)
   {
      if (i >= max_size)
      {
         // If more than the maximum number of entities are added, the ECB will
         // return negative entity UIDs.
         REQUIRE( ecb.addEntity() < 0 );
      }
      else
      {
         REQUIRE( ecb.addEntity() >= 0 );
      }
   }
}

// Verifies that entities can be deleted.
TEST_CASE( "add more than the maximum number of entities, delete them all", "[EntityComponentBuffer]" )
{
   size_t max_size = 256;
   trecs::EntityComponentBuffer ecb(max_size);

   std::vector<trecs::uid_t> entities;

   for (unsigned int i = 0; i < 5 * max_size; ++i)
   {
      entities.push_back(ecb.addEntity());
      if (i >= max_size)
      {
         // If more than the maximum number of entities are added, the ECB will
         // return negative entity UIDs.
         REQUIRE( entities.back() < 0 );
      }
      else
      {
         REQUIRE( entities.back() >= 0 );
      }
   }

   REQUIRE( ecb.numEntities() == max_size );

   for (const auto entity : entities)
   {
      ecb.removeEntity(entity);
   }

   REQUIRE( ecb.numEntities() == 0 );
}

// Verifies that components on an entity can be updated.
TEST_CASE( "components on an entity can be updated", "[EntityComponentBuffer]" )
{
   complicatedType_t<0> comp_a;
   comp_a.float_field = 3.14f;
   comp_a.int_field = -3;

   trecs::EntityComponentBuffer ecb(256);
   ecb.registerComponent<complicatedType_t<0> >();

   REQUIRE( ecb.getComponentEntities<complicatedType_t<0> >().size() == 0 );

   int entity_id1 = ecb.addEntity();
   REQUIRE( ecb.updateComponent(entity_id1, comp_a));

   int entity_id2 = ecb.addEntity();
   REQUIRE( ecb.updateComponent(entity_id2, comp_a));
   REQUIRE( ecb.getComponent<complicatedType_t<0> >(entity_id2) != nullptr );
   REQUIRE( ecb.getComponent<complicatedType_t<0> >(entity_id2)->int_field == -3 );
   REQUIRE( ecb.getComponent<complicatedType_t<0> >(entity_id2)->float_field == 3.14f );

   comp_a.int_field = 5;
   REQUIRE( ecb.updateComponent(entity_id2, comp_a));
   REQUIRE( ecb.getComponent<complicatedType_t<0> >(entity_id2)->int_field == 5 );
   REQUIRE( ecb.getComponent<complicatedType_t<0> >(entity_id2)->float_field == 3.14f );
}

TEST_CASE( "an entity's components can be updated and removed", "[EntityComponentBuffer]" )
{
   trecs::EntityComponentBuffer ecb(256);
   ecb.registerComponent<complicatedType_t<0> >();
   ecb.registerComponent<int>();
   ecb.registerComponent<float>();

   auto entity = ecb.addEntity();

   ecb.updateComponent(entity, -234);
   REQUIRE( ecb.getComponent<int>(entity) != nullptr );
   REQUIRE( *ecb.getComponent<int>(entity) == -234 );

   ecb.removeComponent<int>(entity);
   REQUIRE( ecb.getComponent<int>(entity) == nullptr );

   ecb.updateComponent(entity, 2.123f);
   REQUIRE( ecb.getComponent<float>(entity) != nullptr );
   REQUIRE( *ecb.getComponent<float>(entity) == 2.123f );

   ecb.updateComponent(entity, 2.f * 2.123f);
   REQUIRE( ecb.getComponent<float>(entity) != nullptr );
   REQUIRE( *ecb.getComponent<float>(entity) == (2.f * 2.123f) );

   complicatedType_t<0> temp_ct = {100000, -2.717};
   ecb.updateComponent(entity, temp_ct);
   REQUIRE( ecb.getComponent<complicatedType_t<0> >(entity) != nullptr );
   REQUIRE( (*ecb.getComponent<complicatedType_t<0> >(entity)) == temp_ct );
}

TEST_CASE( "no harm, no foul removing an unregistered component", "[EntityComponentBuffer]" )
{
   trecs::EntityComponentBuffer ecb(256);
   ecb.registerComponent<complicatedType_t<0> >();
   ecb.registerComponent<int>();
   ecb.registerComponent<float>();

   auto entity = ecb.addEntity();

   ecb.removeComponent<complicatedType_t<987> >(entity);

   REQUIRE( true );
}

TEST_CASE( "no harm, no foul removing a non-existent component", "[EntityComponentBuffer]" )
{
   trecs::EntityComponentBuffer ecb(256);
   ecb.registerComponent<complicatedType_t<0> >();
   ecb.registerComponent<int>();
   ecb.registerComponent<float>();

   auto entity = ecb.addEntity();

   ecb.removeComponent<int>(entity);

   REQUIRE( true );
}

TEST_CASE( "components can be updated on one entity", "[EntityComponentBuffer]" )
{
   complicatedType_t<0> comp_a;
   comp_a.float_field = 1.f;
   comp_a.int_field = 0;

   trecs::EntityComponentBuffer ecb(256);
   ecb.registerComponent<complicatedType_t<0> >();

   REQUIRE( ecb.getComponentEntities<complicatedType_t<0> >().size() == 0 );

   trecs::uid_t entity_id1 = ecb.addEntity();
   REQUIRE( ecb.updateComponent(entity_id1, comp_a) );
   REQUIRE( ecb.getComponent<complicatedType_t<0> >(entity_id1)->float_field == 1.f );

   comp_a.float_field = 2.5f;

   REQUIRE( ecb.updateComponent(entity_id1, comp_a) );
   REQUIRE( ecb.getComponent<complicatedType_t<0> >(entity_id1)->float_field == 2.5f );
}

// Verifies that components cannot be added to an inactive entity.
TEST_CASE( "can't add components to inactive entity", "[EntityComponentBuffer]" )
{
   complicatedType_t<0> comp_a;
   comp_a.float_field = 1.f;
   comp_a.int_field = 0;

   trecs::EntityComponentBuffer ecb(256);
   ecb.registerComponent<complicatedType_t<0> >();

   REQUIRE( !ecb.updateComponent(0, comp_a));

   REQUIRE( ecb.getComponentEntities<complicatedType_t<0> >().size() == 0 );

   int entity_id1 = ecb.addEntity();
   REQUIRE( ecb.updateComponent(entity_id1, comp_a));

   int entity_id2 = ecb.addEntity();
   REQUIRE( ecb.updateComponent(entity_id2, comp_a));

   REQUIRE( !ecb.updateComponent(7, comp_a));
}

TEST_CASE( "get component entities", "[EntityComponentBuffer]" )
{
   trecs::EntityComponentBuffer ecb(256);
   ecb.registerComponent<complicatedType_t<0> >();
   ecb.registerComponent<int>();
   ecb.registerComponent<float>();

   complicatedType_t<0> temp_ct = {2, -0.01};

   std::vector<trecs::uid_t> entities;
   for (unsigned int i = 0; i < 128; ++i)
   {
      entities.push_back(ecb.addEntity());

      if ((i % 3) == 0)
      {
         // The value of 'i' won't be added if the 'static_cast' isn't present.
         // Makes sense, unsigned int != int and unsigned ints aren't
         // registered as components with the ECB.
         ecb.updateComponent(entities.back(), static_cast<int>(i));
      }
      else if ((i % 3) == 1)
      {
         ecb.updateComponent(entities.back(), static_cast<float>(i) * -2.123f);
      }
      else
      {
         temp_ct.int_field += -2 * i * temp_ct.int_field;
         temp_ct.float_field *= -1.001f;
         ecb.updateComponent(entities.back(), temp_ct);
      }
   }

   REQUIRE( ecb.getComponentEntities<int>().size() == 43);
   REQUIRE( ecb.getComponentEntities<float>().size() == 43);
   REQUIRE( ecb.getComponentEntities<complicatedType_t<0> >().size() == 42);
}

TEST_CASE( "get a component array wrapper", "[EntityComponentBuffer]" )
{
   trecs::EntityComponentBuffer ecb(256);
   ecb.registerComponent<complicatedType_t<0> >();
   ecb.registerComponent<int>();
   ecb.registerComponent<float>();

   complicatedType_t<0> temp_ct = {2, -0.01};

   std::vector<trecs::uid_t> entities;
   std::vector<int> ints;
   std::vector<float> floats;
   std::vector<complicatedType_t<0> > cts;
   for (unsigned int i = 0; i < 128; ++i)
   {
      entities.push_back(ecb.addEntity());

      if ((i % 3) == 0)
      {
         // The value of 'i' won't be added if the 'static_cast' isn't present.
         // Makes sense, unsigned int != int and unsigned ints aren't
         // registered as components with the ECB.
         ecb.updateComponent(entities.back(), static_cast<int>(i));
         ints.push_back(static_cast<int>(i));
      }
      else if ((i % 3) == 1)
      {
         ecb.updateComponent(entities.back(), static_cast<float>(i) * -2.123f);
         floats.push_back(static_cast<float>(i) * -2.123f);
      }
      else
      {
         temp_ct.int_field += -2 * i * temp_ct.int_field;
         temp_ct.float_field *= -1.001f;
         ecb.updateComponent(entities.back(), temp_ct);
         cts.push_back(temp_ct);
      }
   }

   auto int_comps = ecb.getComponents<int>();
   REQUIRE( int_comps.size() == 43 );

   const auto int_entities = ecb.getComponentEntities<int>();
   int i = 0;
   for (const auto entity : int_entities)
   {
      REQUIRE( ints[i] == *int_comps[entity] );
      ++i;
   }

   auto float_comps = ecb.getComponents<float>();
   REQUIRE( ecb.getComponents<float>().size() == 43 );

   const auto float_entities = ecb.getComponentEntities<float>();
   i = 0;
   for (const auto entity : float_entities)
   {
      REQUIRE( floats[i] == *float_comps[entity] );
      ++i;
   }

   auto ct_comps = ecb.getComponents<complicatedType_t<0> >();
   REQUIRE( ecb.getComponents<complicatedType_t<0> >().size() == 42 );

   const auto ct_entities = ecb.getComponentEntities<complicatedType_t<0> >();
   i = 0;
   for (const auto entity : ct_entities)
   {
      REQUIRE( cts[i] == *ct_comps[entity] );
      ++i;
   }

}

TEST_CASE( "add more than allocated components, then delete them all", "[EntityComponentBuffer]" )
{
   size_t max_size = 256;
   trecs::EntityComponentBuffer ecb(max_size);
   ecb.registerComponent<complicatedType_t<0> >();
   ecb.registerComponent<int>();
   ecb.registerComponent<float>();

   complicatedType_t<0> temp_ct = {2, -0.01};

   std::vector<trecs::uid_t> entities;
   std::vector<int> ints;
   std::vector<float> floats;
   std::vector<complicatedType_t<0> > cts;
   for (unsigned int i = 0; i < 12 * max_size; ++i)
   {
      entities.push_back(ecb.addEntity());

      if ((i % 3) == 0)
      {
         // The value of 'i' won't be added if the 'static_cast' isn't present.
         // Makes sense, unsigned int != int and unsigned ints aren't
         // registered as components with the ECB.
         ecb.updateComponent(entities.back(), static_cast<int>(i));
         ints.push_back(static_cast<int>(i));
      }
      else if ((i % 3) == 1)
      {
         ecb.updateComponent(entities.back(), static_cast<float>(i) * -2.123f);
         floats.push_back(static_cast<float>(i) * -2.123f);
      }
      else
      {
         temp_ct.int_field += -2 * i * temp_ct.int_field;
         temp_ct.float_field *= -1.001f;
         ecb.updateComponent(entities.back(), temp_ct);
         cts.push_back(temp_ct);
      }
   }

   REQUIRE( ecb.getComponentEntities<int>().size() == 86);
   REQUIRE( ecb.getComponentEntities<float>().size() == 85);
   REQUIRE( ecb.getComponentEntities<complicatedType_t<0> >().size() == 85);

   for (const auto entity : entities)
   {
      ecb.removeEntity(entity);
   }

   REQUIRE( ecb.getComponentEntities<int>().size() == 0);
   REQUIRE( ecb.getComponentEntities<float>().size() == 0);
   REQUIRE( ecb.getComponentEntities<complicatedType_t<0> >().size() == 0);
}
