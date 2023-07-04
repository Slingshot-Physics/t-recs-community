#include "entity_component_buffer.hpp"

#include "complicated_types.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <map>
#include <vector>

// template <unsigned int BufferSize>
void artificial_copy_from_const(
   trecs::EntityComponentBuffer & dest,
   const trecs::EntityComponentBuffer & src
)
{
   dest = src;
}

// template <unsigned int BufferSize>
void artificial_copy(
   trecs::EntityComponentBuffer & dest,
   trecs::EntityComponentBuffer & src
)
{
   dest = src;
}

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

TEST_CASE( "can't add unregistered component types", "[EntityComponentBuffer]" )
{
   trecs::EntityComponentBuffer ecb(256);
   ecb.registerComponent<float>();
   
   auto new_entity = ecb.addEntity();
   REQUIRE( !ecb.updateComponent(new_entity, 'a') );
}

// Verifies that the ECB returns entities >= 0 while the entity count is
// below the maximum number of entities.
TEST_CASE( "add more than the maximum number of entities", "[EntityComponentBuffer]" )
{
   const size_t max_size = 256;
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
   const size_t max_size = 256;
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

   ecb.clear();

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

TEST_CASE( "add more than the maximum number of components, then delete them all", "[EntityComponentBuffer]" )
{
   const size_t max_size = 256;
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

   std::cout << "number of entities: " << entities.size() << "\n";

   REQUIRE( ecb.getComponentEntities<int>().size() == 86);
   REQUIRE( ecb.getComponentEntities<float>().size() == 85);
   REQUIRE( ecb.getComponentEntities<complicatedType_t<0> >().size() == 85);

   ecb.clear();

   REQUIRE( ecb.getComponentEntities<int>().size() == 0);
   REQUIRE( ecb.getComponentEntities<float>().size() == 0);
   REQUIRE( ecb.getComponentEntities<complicatedType_t<0> >().size() == 0);
}

TEST_CASE( "register multiple components at once", "[EntityComponentBuffer]" )
{
   const size_t max_size = 256;
   trecs::EntityComponentBuffer ecb(max_size);

   ecb.registerComponents<complicatedType_t<0>, int, float >();

   complicatedType_t<0> temp_ct = {15, -90.43f};

   auto new_entity = ecb.addEntity();
   ecb.updateComponent(new_entity, 1);
   ecb.updateComponent(new_entity, 8.5f);
   ecb.updateComponent(new_entity, temp_ct);

   REQUIRE( ecb.getComponent<int>(new_entity) != nullptr );
   REQUIRE( ecb.getComponent<float>(new_entity) != nullptr );
   REQUIRE( ecb.getComponent<complicatedType_t<0> >(new_entity) != nullptr );

   REQUIRE( *ecb.getComponent<int>(new_entity) == 1 );
   REQUIRE( *ecb.getComponent<float>(new_entity) == 8.5f );
   REQUIRE( *ecb.getComponent<complicatedType_t<0> >(new_entity) == temp_ct );
}

TEST_CASE( "assignment operator empty to empty", "[EntityComponentBuffer]" )
{
   const size_t max_size = 256;
   trecs::EntityComponentBuffer ecb1(max_size);
   ecb1.registerComponent<complicatedType_t<0> >();
   ecb1.registerComponent<int>();
   ecb1.registerComponent<float>();

   trecs::EntityComponentBuffer ecb2(max_size);

   auto num_signatures = ecb1.numSignatures();

   ecb2 = ecb1;

   REQUIRE( ecb2.numSignatures() == ecb1.numSignatures() );
   REQUIRE( ecb2.numEntities() == ecb1.numEntities() );
}

TEST_CASE( "assignment operator non-empty to empty", "[EntityComponentBuffer]" )
{
   const size_t max_size = 256;
   trecs::EntityComponentBuffer ecb1(max_size);
   ecb1.registerComponent<complicatedType_t<0> >();
   ecb1.registerComponent<int>();
   ecb1.registerComponent<float>();

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 100; ++i)
   {
      entities.push_back(ecb1.addEntity());
   }

   std::map<trecs::uid_t, int> entity_ints;
   std::map<trecs::uid_t, float> entity_floats;
   std::map<trecs::uid_t, complicatedType_t<0> > entity_cts;

   int i = 0;
   for (const auto entity : entities)
   {
      entity_ints[entity] = ((i % 2) ? -1 : 1) * 3 * i;
      entity_floats[entity] = ((i % 2) ? 1 : -1) * 5.f * static_cast<float>(i);
      entity_cts[entity].int_field = i - entities.size() / 2;
      entity_cts[entity].float_field = ((i % 3) ? 1 : -1) * 4.4f * static_cast<float>(i);
      ++i;
   }

   for (const auto entity_int : entity_ints)
   {
      ecb1.updateComponent(entity_int.first, entity_int.second);
   }

   for (const auto entity_float : entity_floats)
   {
      ecb1.updateComponent(entity_float.first, entity_float.second);
   }

   for (const auto entity_ct : entity_cts)
   {
      ecb1.updateComponent(entity_ct.first, entity_ct.second);
   }

   trecs::EntityComponentBuffer ecb2(max_size);

   auto num_signatures = ecb1.numSignatures();

   ecb2 = ecb1;

   REQUIRE( ecb2.numSignatures() == ecb1.numSignatures() );
   REQUIRE( ecb2.numEntities() == ecb1.numEntities() );

   for (const auto entity_int : entity_ints)
   {
      REQUIRE( *ecb2.getComponent<int>(entity_int.first) == entity_int.second );
   }

   for (const auto entity_float : entity_floats)
   {
      REQUIRE( *ecb2.getComponent<float>(entity_float.first) == entity_float.second );
   }

   for (const auto entity_ct : entity_cts)
   {
      REQUIRE( ecb2.getComponent<complicatedType_t<0> >(entity_ct.first)->int_field == entity_ct.second.int_field );
      REQUIRE( ecb2.getComponent<complicatedType_t<0> >(entity_ct.first)->float_field == entity_ct.second.float_field );
   }

}

TEST_CASE( "assignment operator non-empty to non-empty", "[EntityComponentBuffer]" )
{
   const size_t max_size = 256;
   trecs::EntityComponentBuffer ecb1(max_size);
   ecb1.registerComponent<complicatedType_t<0> >();
   ecb1.registerComponent<int>();
   ecb1.registerComponent<float>();

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 100; ++i)
   {
      entities.push_back(ecb1.addEntity());
   }

   std::map<trecs::uid_t, int> entity_ints;
   std::map<trecs::uid_t, float> entity_floats;
   std::map<trecs::uid_t, complicatedType_t<0> > entity_cts;

   int i = 0;
   for (const auto entity : entities)
   {
      entity_ints[entity] = ((i % 2) ? -1 : 1) * 3 * i;
      entity_floats[entity] = ((i % 2) ? 1 : -1) * 5.f * static_cast<float>(i);
      entity_cts[entity].int_field = i - entities.size() / 2;
      entity_cts[entity].float_field = ((i % 3) ? 1 : -1) * 4.4f * static_cast<float>(i);
      ++i;
   }

   for (const auto entity_int : entity_ints)
   {
      ecb1.updateComponent(entity_int.first, entity_int.second);
   }

   for (const auto entity_float : entity_floats)
   {
      ecb1.updateComponent(entity_float.first, entity_float.second);
   }

   for (const auto entity_ct : entity_cts)
   {
      ecb1.updateComponent(entity_ct.first, entity_ct.second);
   }

   trecs::EntityComponentBuffer ecb2(max_size);

   ecb2.registerComponent<double>();
   ecb2.registerComponent<unsigned char>();

   ecb2.addEntity();
   ecb2.updateComponent(ecb2.addEntity(), 2.0);
   ecb2.updateComponent(ecb2.addEntity(), 77);
   ecb2.updateComponent(ecb2.addEntity(), -22.0);
   ecb2.updateComponent(ecb2.addEntity(), 42);

   auto num_signatures = ecb1.numSignatures();

   ecb2 = ecb1;

   REQUIRE( ecb2.numSignatures() == ecb1.numSignatures() );
   REQUIRE( ecb2.numEntities() == ecb1.numEntities() );

   for (const auto entity_int : entity_ints)
   {
      REQUIRE( *ecb2.getComponent<int>(entity_int.first) == entity_int.second );
   }

   for (const auto entity_float : entity_floats)
   {
      REQUIRE( *ecb2.getComponent<float>(entity_float.first) == entity_float.second );
   }

   for (const auto entity_ct : entity_cts)
   {
      REQUIRE( ecb2.getComponent<complicatedType_t<0> >(entity_ct.first)->int_field == entity_ct.second.int_field );
      REQUIRE( ecb2.getComponent<complicatedType_t<0> >(entity_ct.first)->float_field == entity_ct.second.float_field );
   }

}

TEST_CASE( "assignment operator const non-empty to non-empty", "[EntityComponentBuffer]" )
{
   const size_t max_size = 256;
   trecs::EntityComponentBuffer ecb1(max_size);
   ecb1.registerComponent<complicatedType_t<0> >();
   ecb1.registerComponent<int>();
   ecb1.registerComponent<float>();

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 100; ++i)
   {
      entities.push_back(ecb1.addEntity());
   }

   std::map<trecs::uid_t, int> entity_ints;
   std::map<trecs::uid_t, float> entity_floats;
   std::map<trecs::uid_t, complicatedType_t<0> > entity_cts;

   int i = 0;
   for (const auto entity : entities)
   {
      entity_ints[entity] = ((i % 2) ? -1 : 1) * 3 * i;
      entity_floats[entity] = ((i % 2) ? 1 : -1) * 5.f * static_cast<float>(i);
      entity_cts[entity].int_field = i - entities.size() / 2;
      entity_cts[entity].float_field = ((i % 3) ? 1 : -1) * 4.4f * static_cast<float>(i);
      ++i;
   }

   for (const auto entity_int : entity_ints)
   {
      ecb1.updateComponent(entity_int.first, entity_int.second);
   }

   for (const auto entity_float : entity_floats)
   {
      ecb1.updateComponent(entity_float.first, entity_float.second);
   }

   for (const auto entity_ct : entity_cts)
   {
      ecb1.updateComponent(entity_ct.first, entity_ct.second);
   }

   trecs::EntityComponentBuffer ecb2(max_size);

   ecb2.registerComponent<double>();
   ecb2.registerComponent<unsigned char>();

   ecb2.addEntity();
   ecb2.updateComponent(ecb2.addEntity(), 2.0);
   ecb2.updateComponent(ecb2.addEntity(), 77);
   ecb2.updateComponent(ecb2.addEntity(), -22.0);
   ecb2.updateComponent(ecb2.addEntity(), 42);

   auto num_signatures = ecb1.numSignatures();

   artificial_copy_from_const(ecb2, ecb1);
   ecb1.release();

   REQUIRE( ecb2.numSignatures() == ecb1.numSignatures() );
   REQUIRE( ecb2.numEntities() == ecb1.numEntities() );

   for (const auto entity_int : entity_ints)
   {
      REQUIRE( *ecb2.getComponent<int>(entity_int.first) == entity_int.second );
   }

   for (const auto entity_float : entity_floats)
   {
      REQUIRE( *ecb2.getComponent<float>(entity_float.first) == entity_float.second );
   }

   for (const auto entity_ct : entity_cts)
   {
      REQUIRE( ecb2.getComponent<complicatedType_t<0> >(entity_ct.first)->int_field == entity_ct.second.int_field );
      REQUIRE( ecb2.getComponent<complicatedType_t<0> >(entity_ct.first)->float_field == entity_ct.second.float_field );
   }

}

TEST_CASE( "assignment operator const non-empty to empty", "[EntityComponentBuffer]" )
{
   const size_t max_size = 256;
   trecs::EntityComponentBuffer ecb1(max_size);
   ecb1.registerComponent<complicatedType_t<0> >();
   ecb1.registerComponent<int>();
   ecb1.registerComponent<float>();

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 100; ++i)
   {
      entities.push_back(ecb1.addEntity());
   }

   std::map<trecs::uid_t, int> entity_ints;
   std::map<trecs::uid_t, float> entity_floats;
   std::map<trecs::uid_t, complicatedType_t<0> > entity_cts;

   int i = 0;
   for (const auto entity : entities)
   {
      entity_ints[entity] = ((i % 2) ? -1 : 1) * 3 * i;
      entity_floats[entity] = ((i % 2) ? 1 : -1) * 5.f * static_cast<float>(i);
      entity_cts[entity].int_field = i - entities.size() / 2;
      entity_cts[entity].float_field = ((i % 3) ? 1 : -1) * 4.4f * static_cast<float>(i);
      ++i;
   }

   for (const auto entity_int : entity_ints)
   {
      ecb1.updateComponent(entity_int.first, entity_int.second);
   }

   for (const auto entity_float : entity_floats)
   {
      ecb1.updateComponent(entity_float.first, entity_float.second);
   }

   for (const auto entity_ct : entity_cts)
   {
      ecb1.updateComponent(entity_ct.first, entity_ct.second);
   }

   trecs::EntityComponentBuffer ecb2(max_size);

   auto num_signatures = ecb1.numSignatures();

   artificial_copy_from_const(ecb2, ecb1);
   ecb1.release();

   REQUIRE( ecb2.numSignatures() == ecb1.numSignatures() );
   REQUIRE( ecb2.numEntities() == ecb1.numEntities() );

   for (const auto entity_int : entity_ints)
   {
      REQUIRE( *ecb2.getComponent<int>(entity_int.first) == entity_int.second );
   }

   for (const auto entity_float : entity_floats)
   {
      REQUIRE( *ecb2.getComponent<float>(entity_float.first) == entity_float.second );
   }

   for (const auto entity_ct : entity_cts)
   {
      REQUIRE( ecb2.getComponent<complicatedType_t<0> >(entity_ct.first)->int_field == entity_ct.second.int_field );
      REQUIRE( ecb2.getComponent<complicatedType_t<0> >(entity_ct.first)->float_field == entity_ct.second.float_field );
   }

}

TEST_CASE( "verify that the registration lock works", "[EntityComponentBuffer]")
{
   const size_t max_size = 256;
   trecs::EntityComponentBuffer ecb1(max_size);
   ecb1.registerComponent<complicatedType_t<0> >();
   ecb1.registerComponent<int>();
   ecb1.registerComponent<float>();

   ecb1.updateComponent(ecb1.addEntity(), 2);
   ecb1.updateComponent(ecb1.addEntity(), 2.3f);
   ecb1.updateComponent(ecb1.addEntity(), complicatedType_t<0>{123, -0.00123f});

   ecb1.lockRegistration();

   ecb1.registerComponent<unsigned char>();
   REQUIRE( ecb1.supportsComponents<int, float, complicatedType_t<0> >() );
   REQUIRE( !ecb1.supportsComponents<unsigned char>() );
}

// Verify that clearing an empty ECB doesn't change the number of registered
// components and that new entities and components can be added to the ECB.
TEST_CASE( "clear empty ECB", "[EntityComponentBuffer]" )
{
   const size_t max_size = 256;
   trecs::EntityComponentBuffer ecb(max_size);
   ecb.registerComponent<complicatedType_t<0> >();
   ecb.registerComponent<int>();
   ecb.registerComponent<float>();

   REQUIRE( ecb.numEntities() == 0 );
   REQUIRE( ecb.numSignatures() == 3 );

   ecb.clear();

   REQUIRE( ecb.numEntities() == 0 );
   REQUIRE( ecb.numSignatures() == 3 );

   auto new_entity = ecb.addEntity();

   ecb.updateComponent(new_entity, 3.5f);
   ecb.updateComponent(new_entity, 123456);
   ecb.updateComponent(new_entity, complicatedType_t<0>{99, -99.3f});

   REQUIRE( ecb.numEntities() == 1 );
   REQUIRE( *ecb.getComponent<float>(new_entity) == 3.5f );
   REQUIRE( *ecb.getComponent<int>(new_entity) == 123456 );
   REQUIRE( *ecb.getComponent<complicatedType_t<0> >(new_entity) == complicatedType_t<0>{99, -99.3f} );
}

TEST_CASE( "clear partially-filled ECB", "[EntityComponentBuffer]" )
{
   const size_t max_size = 256;
   trecs::EntityComponentBuffer ecb(max_size);
   ecb.registerComponent<complicatedType_t<0> >();
   ecb.registerComponent<int>();
   ecb.registerComponent<float>();

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 100; ++i)
   {
      entities.push_back(ecb.addEntity());
   }

   std::map<trecs::uid_t, int> entity_ints;
   std::map<trecs::uid_t, float> entity_floats;
   std::map<trecs::uid_t, complicatedType_t<0> > entity_cts;

   int i = 0;
   for (const auto entity : entities)
   {
      entity_ints[entity] = ((i % 2) ? -1 : 1) * 3 * i;
      entity_floats[entity] = ((i % 2) ? 1 : -1) * 5.f * static_cast<float>(i);
      entity_cts[entity].int_field = i - entities.size() / 2;
      entity_cts[entity].float_field = ((i % 3) ? 1 : -1) * 4.4f * static_cast<float>(i);
      ++i;
   }

   for (const auto entity_int : entity_ints)
   {
      ecb.updateComponent(entity_int.first, entity_int.second);
   }

   for (const auto entity_float : entity_floats)
   {
      ecb.updateComponent(entity_float.first, entity_float.second);
   }

   for (const auto entity_ct : entity_cts)
   {
      ecb.updateComponent(entity_ct.first, entity_ct.second);
   }

   REQUIRE( ecb.numEntities() == 100 );
   REQUIRE( ecb.getComponentEntities<float>().size() == entity_floats.size() );
   REQUIRE( ecb.getComponentEntities<int>().size() == entity_ints.size() );
   REQUIRE( ecb.getComponentEntities<complicatedType_t<0> >().size() == entity_cts.size() );

   ecb.clear();

   REQUIRE( ecb.numEntities() == 0 );
   REQUIRE( ecb.getComponentEntities<float>().size() == 0 );
   REQUIRE( ecb.getComponentEntities<int>().size() == 0 );
   REQUIRE( ecb.getComponentEntities<complicatedType_t<0> >().size() == 0 );
}

TEST_CASE( "clear full ECB", "[EntityComponentBuffer]" )
{
   const size_t max_size = 256;
   trecs::EntityComponentBuffer ecb(max_size);
   ecb.registerComponent<complicatedType_t<0> >();
   ecb.registerComponent<int>();
   ecb.registerComponent<float>();

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < max_size; ++i)
   {
      entities.push_back(ecb.addEntity());
   }

   std::map<trecs::uid_t, int> entity_ints;
   std::map<trecs::uid_t, float> entity_floats;
   std::map<trecs::uid_t, complicatedType_t<0> > entity_cts;

   int i = 0;
   for (const auto entity : entities)
   {
      entity_ints[entity] = ((i % 2) ? -1 : 1) * 3 * i;
      entity_floats[entity] = ((i % 2) ? 1 : -1) * 5.f * static_cast<float>(i);
      entity_cts[entity].int_field = i - entities.size() / 2;
      entity_cts[entity].float_field = ((i % 3) ? 1 : -1) * 4.4f * static_cast<float>(i);
      ++i;
   }

   for (const auto entity_int : entity_ints)
   {
      ecb.updateComponent(entity_int.first, entity_int.second);
   }

   for (const auto entity_float : entity_floats)
   {
      ecb.updateComponent(entity_float.first, entity_float.second);
   }

   for (const auto entity_ct : entity_cts)
   {
      ecb.updateComponent(entity_ct.first, entity_ct.second);
   }

   REQUIRE( ecb.numEntities() == max_size );
   REQUIRE( ecb.getComponentEntities<float>().size() == entity_floats.size() );
   REQUIRE( ecb.getComponentEntities<int>().size() == entity_ints.size() );
   REQUIRE( ecb.getComponentEntities<complicatedType_t<0> >().size() == entity_cts.size() );

   ecb.clear();

   REQUIRE( ecb.numEntities() == 0 );
   REQUIRE( ecb.getComponentEntities<float>().size() == 0 );
   REQUIRE( ecb.getComponentEntities<int>().size() == 0 );
   REQUIRE( ecb.getComponentEntities<complicatedType_t<0> >().size() == 0 );
}
