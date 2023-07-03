#include "component_manager.hpp"

#include "complicated_types.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

template <int N>
struct component
{
   int thing;
};

void artificial_assignment(
   trecs::ComponentManager & dest,
   const trecs::ComponentManager & src
)
{
   dest = src;
}

// Instantiate the component manager.
TEST_CASE( "instantiation", "[ComponentManager]" )
{
   trecs::ComponentManager cman(120);
}

TEST_CASE( "get num components from zero registrations", "[ComponentManager]" )
{
   trecs::ComponentManager cman(120);
   REQUIRE( cman.getNumComponents<float>() == 0 );
   REQUIRE( cman.getNumComponents<int>() == 0 );
   REQUIRE( cman.getNumComponents<complicatedType_t<77> >() == 0 );
}

// Try to insert an unregistered type into the component manager.
TEST_CASE( "unregistered type", "[ComponentManager]" )
{
   trecs::ComponentManager cman(120);
   int new_uid = cman.addComponent<float>(1, 1234.f);
   REQUIRE(new_uid == -1);
}

// Verify that trying to retrieve a non-present UID returns nullptr.
TEST_CASE( "retrieve non-present UID", "[ComponentManager]" )
{
   trecs::ComponentManager cman(120);
   cman.registerComponent<complicatedType_t<0> >();
   REQUIRE( cman.getComponent<complicatedType_t<0> >(1) == nullptr );
}

// Insert one component of one type into the component manager.
TEST_CASE( "register one component", "[ComponentManager]" )
{
   trecs::ComponentManager cman(120);
   cman.registerComponent<complicatedType_t<0> >();

   complicatedType_t<0> comp_a;
   comp_a.int_field = 202;

   cman.addComponent<complicatedType_t<0> >(1, comp_a);

   REQUIRE(cman.getComponent<complicatedType_t<0> >(1) != nullptr);
   if (cman.getComponent<complicatedType_t<0> >(1) != nullptr)
   {
      REQUIRE(cman.getComponent<complicatedType_t<0> >(1)->int_field == 202);
   }
}

// Register the same component multiple times, verify that it doesn't overwrite
// any existing data when it's re-registered.
TEST_CASE( "register same component multiple times", "[ComponentManager]" )
{
   trecs::ComponentManager cman(120);
   cman.registerComponent<complicatedType_t<0> >();

   complicatedType_t<0> comp_a;
   comp_a.int_field = 202;

   // Add the same component a ton of times.
   for (int i = 0; i < 100; ++i)
   {
      cman.addComponent<complicatedType_t<0> >(i, comp_a);
   }

   // Verify that the component is present across all of the entities
   for (int i = 0; i < cman.getNumComponents<complicatedType_t<0> >(); ++i)
   {
      REQUIRE(cman.getComponent<complicatedType_t<0> >(i) != nullptr);
      if (cman.getComponent<complicatedType_t<0> >(i) != nullptr)
      {
         REQUIRE(cman.getComponent<complicatedType_t<0> >(i)->int_field == 202);
      }
   }

   // Register the same component again
   cman.registerComponent<complicatedType_t<0> >();

   // Verify that all of the old data is still present
   for (int i = 0; i < cman.getNumComponents<complicatedType_t<0> >(); ++i)
   {
      REQUIRE(cman.getComponent<complicatedType_t<0> >(i) != nullptr);
      if (cman.getComponent<complicatedType_t<0> >(i) != nullptr)
      {
         REQUIRE(cman.getComponent<complicatedType_t<0> >(i)->int_field == 202);
      }
   }

   // And just to be sure, register the same component again
   cman.registerComponent<complicatedType_t<0> >();

   // Verify that all of the old data is still present
   for (int i = 0; i < cman.getNumComponents<complicatedType_t<0> >(); ++i)
   {
      REQUIRE(cman.getComponent<complicatedType_t<0> >(i) != nullptr);
      if (cman.getComponent<complicatedType_t<0> >(i) != nullptr)
      {
         REQUIRE(cman.getComponent<complicatedType_t<0> >(i)->int_field == 202);
      }
   }
}

// Insert components of one type into the component manager past its capacity
// and verify that components can be retrieved from it.
TEST_CASE( "insert one registered component to capacity", "[ComponentManager]" )
{
   size_t max_size = 123;
   trecs::ComponentManager cman(max_size);
   cman.registerComponent<complicatedType_t<0> >();

   complicatedType_t<0> comp_a;

   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      int new_uid = cman.addComponent<complicatedType_t<0> >(i, comp_a);
      if (new_uid > -1)
      {
         cman.getComponent<complicatedType_t<0> >(new_uid)->int_field = new_uid;
      }
      if (i >= max_size)
      {
         REQUIRE(new_uid == -1);
      }
      else
      {
         REQUIRE(new_uid != -1);
      }
   }

   for (unsigned int i = 0; i < cman.getNumComponents<complicatedType_t<0> >(); ++i)
   {
      REQUIRE(cman.getComponent<complicatedType_t<0> >(i) != nullptr);
      if (cman.getComponent<complicatedType_t<0> >(i) != nullptr)
      {
         REQUIRE(cman.getComponent<complicatedType_t<0> >(i)->int_field == i);
      }
   }
}

TEST_CASE( "deletion of multiple components", "[ComponentManager]" )
{
   size_t max_size = 123;
   trecs::ComponentManager cman(max_size);
   cman.registerComponent<complicatedType_t<0> >();
   cman.registerComponent<complicatedType_t<1> >();
   cman.registerComponent<complicatedType_t<2> >();
   cman.registerComponent<complicatedType_t<3> >();

   complicatedType_t<0> component_a;
   complicatedType_t<1> component_b;
   complicatedType_t<2> component_c;
   complicatedType_t<3> component_d;

   for (int i = 0; i < max_size; ++i)
   {
      int new_comp0_uid = cman.addComponent(i, component_a);
      if (new_comp0_uid > -1)
      {
         cman.getComponent<complicatedType_t<0> >(new_comp0_uid)->int_field = new_comp0_uid;
      }

      int new_comp1_uid = cman.addComponent(i, component_b);
      if (new_comp1_uid > -1)
      {
         cman.getComponent<complicatedType_t<1> >(new_comp1_uid)->int_field = new_comp1_uid;
      }

      int new_comp2_uid = cman.addComponent(i, component_c);
      if (new_comp2_uid > -1)
      {
         cman.getComponent<complicatedType_t<2> >(new_comp2_uid)->float_field = static_cast<float>(i);
      }

      int new_comp3_uid = cman.addComponent(i, component_d);
      if (new_comp3_uid > -1)
      {
         cman.getComponent<complicatedType_t<3> >(new_comp3_uid)->int_field = 2 * i;
         cman.getComponent<complicatedType_t<3> >(new_comp3_uid)->float_field = -2.f * static_cast<float>(i);
      }
   }

   REQUIRE( cman.getNumComponents<complicatedType_t<0> >() == max_size );

   int num_deletions = 0;
   for (int i = 0; i < max_size; ++i)
   {
      if ((i % 2) == 0)
      {
         cman.removeComponents(i);
         ++num_deletions;
      }
   }

   std::cout << "number of components after deletion: " << cman.getNumComponents<complicatedType_t<0> >() << "\n";
   std::cout << "number of deletions: " << num_deletions << "\n";
   std::cout << "max size: " << max_size << "\n";

   for (int i = 0; i < max_size; ++i)
   {
      if ((i % 2) == 0)
      {
         REQUIRE( cman.getComponent<complicatedType_t<0> >(i) == nullptr );
         REQUIRE( cman.getComponent<complicatedType_t<1> >(i) == nullptr );
         REQUIRE( cman.getComponent<complicatedType_t<2> >(i) == nullptr );
         REQUIRE( cman.getComponent<complicatedType_t<3> >(i) == nullptr );
      }
      else
      {
         REQUIRE( cman.getComponent<complicatedType_t<0> >(i) != nullptr );
         REQUIRE( cman.getComponent<complicatedType_t<1> >(i) != nullptr );
         REQUIRE( cman.getComponent<complicatedType_t<2> >(i) != nullptr );
         REQUIRE( cman.getComponent<complicatedType_t<3> >(i) != nullptr );
      }
   }
}

// Insert multiple component types of the component manager's capacity.
TEST_CASE( "insert multiple registered components to capacity", "[ComponentManager]" )
{
   size_t max_size = 123;
   trecs::ComponentManager cman(max_size);
   cman.registerComponent<complicatedType_t<0> >();
   cman.registerComponent<complicatedType_t<1> >();

   complicatedType_t<0> tempBody;
   complicatedType_t<1> tempCollider;

   for (unsigned int i = 0; i < 4 * max_size; ++i)
   {
      int new_body_uid = cman.addComponent<complicatedType_t<0> >(i, tempBody);
      int new_collider_uid = cman.addComponent<complicatedType_t<1> >(i, tempCollider);

      if (new_body_uid > -1)
      {
         cman.getComponent<complicatedType_t<0> >(new_body_uid)->int_field = new_body_uid;
         cman.getComponent<complicatedType_t<1> >(new_collider_uid)->int_field = new_collider_uid;
      }
      if (i >= max_size)
      {
         REQUIRE(new_body_uid == -1);
         REQUIRE(new_collider_uid == -1);

      }
      else
      {
         REQUIRE(new_body_uid != -1);
         REQUIRE(new_collider_uid != -1);

      }
   }

   for (unsigned int i = 0; i < cman.getNumComponents<complicatedType_t<0> >(); ++i)
   {
      REQUIRE(cman.getComponent<complicatedType_t<0> >(i) != nullptr);
      if (cman.getComponent<complicatedType_t<0> >(i) != nullptr)
      {
         REQUIRE(cman.getComponent<complicatedType_t<0> >(i)->int_field == i);
      }
   }

   for (unsigned int i = 0; i < cman.getNumComponents<complicatedType_t<1> >(); ++i)
   {
      REQUIRE(cman.getComponent<complicatedType_t<1> >(i) != nullptr);
      if (cman.getComponent<complicatedType_t<1> >(i) != nullptr)
      {
         REQUIRE(cman.getComponent<complicatedType_t<1> >(i)->int_field == i);
      }
   }
}

// Verify that all types can be inserted in different unusual orderings and
// then deleted.
TEST_CASE( "insert all types at varying capacities and delete them all", "[ComponentManager]" )
{
   typedef complicatedType_t<0> rb_t;
   typedef complicatedType_t<1> collider_t;
   typedef complicatedType_t<2> aabb_t;

   unsigned int offsets[6] = {21, 30, 15, 7, 50, 44};

   size_t max_size = 350;
   trecs::ComponentManager cman(max_size);
   cman.registerComponent<rb_t>();
   cman.registerComponent<aabb_t>();
   cman.registerComponent<collider_t>();

   rb_t tempBody;
   collider_t tempCollider;
   aabb_t tempAabb;

   std::vector<unsigned int> body_uids;
   std::vector<unsigned int> collider_uids;
   std::vector<unsigned int> aabb_uids;

   unsigned int last_body_uid = 0;
   unsigned int last_collider_uid = 0;
   unsigned int last_mesh_uid = 0;
   unsigned int last_polyhedron_uid = 0;
   unsigned int last_trans_uid = 0;
   unsigned int last_aabb_uid = 0;

   unsigned int max_loops = 1000;

   unsigned int num_loops = 0;
   while(
      cman.getNumComponents<rb_t>() < max_size &&
      cman.getNumComponents<collider_t>() < max_size &&
      cman.getNumComponents<aabb_t>() < max_size &&
      num_loops < max_loops
   )
   {
      for (unsigned int i = 0; i < offsets[0]; ++i)
      {
         int new_body_uid = cman.addComponent<rb_t>(last_body_uid + 1, tempBody);
         if (new_body_uid == -1)
         {
            REQUIRE(cman.getNumComponents<rb_t>() == max_size);
         }
         else
         {
            REQUIRE( cman.getComponent<rb_t>(new_body_uid) != nullptr);
            cman.getComponent<rb_t>(new_body_uid)->int_field = new_body_uid;
            last_body_uid = new_body_uid;
            body_uids.push_back(new_body_uid);
         }
      }

      for (unsigned int i = 0; i < offsets[1]; ++i)
      {
         int new_collider_uid = cman.addComponent<collider_t>(last_collider_uid + 1, tempCollider);
         if (new_collider_uid == -1)
         {
            REQUIRE(cman.getNumComponents<collider_t>() == max_size);
         }
         else
         {
            REQUIRE( cman.getComponent<collider_t>(new_collider_uid) != nullptr);
            cman.getComponent<collider_t>(new_collider_uid)->int_field = new_collider_uid;
            last_collider_uid = new_collider_uid;
            collider_uids.push_back(new_collider_uid);
         }
      }

      for (unsigned int i = 0; i < offsets[5]; ++i)
      {
         int new_aabb_uid = cman.addComponent<aabb_t>(last_aabb_uid + 1, tempAabb);
         if (new_aabb_uid == -1)
         {
            REQUIRE(cman.getNumComponents<aabb_t>() == max_size);
         }
         else
         {
            REQUIRE( cman.getComponent<aabb_t>(new_aabb_uid) != nullptr );
            last_aabb_uid = new_aabb_uid;
            aabb_uids.push_back(new_aabb_uid);
         }
      }

      ++num_loops;
   }

   num_loops = 0;
   while(
      cman.getNumComponents<rb_t>() > 0 &&
      cman.getNumComponents<collider_t>() < 0 &&
      cman.getNumComponents<aabb_t>() < 0 &&
      num_loops < max_loops
   )
   {
      for (unsigned int i = 0; i < offsets[5]; ++i)
      {
         if (body_uids.size() > 0)
         {
            cman.removeComponent<rb_t>(body_uids.back());
            body_uids.pop_back();
         }
      }

      for (unsigned int i = 0; i < offsets[4]; ++i)
      {
         if (collider_uids.size() > 0)
         {
            cman.removeComponent<collider_t>(collider_uids.back());
            collider_uids.pop_back();
         }
      }

      for (unsigned int i = 0; i < offsets[0]; ++i)
      {
         if (aabb_uids.size() > 0)
         {
            cman.removeComponent<aabb_t>(aabb_uids.back());
            aabb_uids.pop_back();
         }
      }

      ++num_loops;
   }
}

TEST_CASE( "get UIDs from unregistered component type", "[ComponentManager]" )
{
   trecs::ComponentManager cman(120);
   cman.registerComponent<complicatedType_t<26> >();
   cman.registerComponent<complicatedType_t<34> >();
   cman.registerComponent<float>();
   cman.registerComponent<int>();

   REQUIRE( cman.getComponentEntities<double>().size() == 0 );
}

TEST_CASE( "correct UIDs are retrieved from component types", "[ComponentManager]" )
{
   trecs::ComponentManager cman(120);
   cman.registerComponent<complicatedType_t<26> >();
   cman.registerComponent<complicatedType_t<34> >();
   cman.registerComponent<float>();
   cman.registerComponent<int>();

   // Add five int types to unique entities in the component manager.
   cman.addComponent(0, 2);
   cman.addComponent(1, 4);
   cman.addComponent(2, 8);
   cman.addComponent(3, 16);
   cman.addComponent(4, 32);

   // Add three float types to unique entities in the component manager.
   cman.addComponent(1, 4.5f);
   cman.addComponent(3, -4.5f);
   cman.addComponent(5, -4.5f);

   // Add seven complicated type <26> types to unique entities.
   cman.addComponent(33, complicatedType_t<26>{1, -2.f});
   cman.addComponent(34, complicatedType_t<26>{1, 12.f});
   cman.addComponent(35, complicatedType_t<26>{2, -4.f});
   cman.addComponent(36, complicatedType_t<26>{3, 22.f});
   cman.addComponent(37, complicatedType_t<26>{5, -7.3f});
   cman.addComponent(38, complicatedType_t<26>{8, -9.f});
   cman.addComponent(40, complicatedType_t<26>{13, -11.f});

   // These sizes must match up
   REQUIRE( cman.getComponentEntities<complicatedType_t<34> >().size() == 0 );
   REQUIRE( cman.getComponentEntities<complicatedType_t<26> >().size() == 7 );
   REQUIRE( cman.getComponentEntities<int>().size() == 5 );
   REQUIRE( cman.getComponentEntities<float>().size() == 3 );

   const auto int_entities = cman.getComponentEntities<int>();
   const auto float_entities = cman.getComponentEntities<float>();
   const auto ct26_entities = cman.getComponentEntities<complicatedType_t<26> >();
   const auto ct34_entities = cman.getComponentEntities<complicatedType_t<34> >();

   // Verify that all of the entity UIDs that were used to add int components are found.
   REQUIRE(
      (
         (std::find(int_entities.begin(), int_entities.end(), 0) != int_entities.end()) &&
         (std::find(int_entities.begin(), int_entities.end(), 1) != int_entities.end()) &&
         (std::find(int_entities.begin(), int_entities.end(), 2) != int_entities.end()) &&
         (std::find(int_entities.begin(), int_entities.end(), 3) != int_entities.end()) &&
         (std::find(int_entities.begin(), int_entities.end(), 4) != int_entities.end())
      )
   );

   // Verify that all of the entity UIDs that were used to add float components are found.
   REQUIRE(
      (
         (std::find(float_entities.begin(), float_entities.end(), 1) != float_entities.end()) &&
         (std::find(float_entities.begin(), float_entities.end(), 3) != float_entities.end()) &&
         (std::find(float_entities.begin(), float_entities.end(), 5) != float_entities.end())
      )
   );

   // Verify that all of the entity UIDs that were used to add ct26 components are found.
   REQUIRE(
      (
         (std::find(ct26_entities.begin(), ct26_entities.end(), 33) != ct26_entities.end()) &&
         (std::find(ct26_entities.begin(), ct26_entities.end(), 34) != ct26_entities.end()) &&
         (std::find(ct26_entities.begin(), ct26_entities.end(), 35) != ct26_entities.end()) &&
         (std::find(ct26_entities.begin(), ct26_entities.end(), 36) != ct26_entities.end()) &&
         (std::find(ct26_entities.begin(), ct26_entities.end(), 37) != ct26_entities.end()) &&
         (std::find(ct26_entities.begin(), ct26_entities.end(), 38) != ct26_entities.end()) &&
         (std::find(ct26_entities.begin(), ct26_entities.end(), 40) != ct26_entities.end())
      )
   );

   // Check some negative cases - these entities shouldn't be parts of these components.
   REQUIRE( (std::find(int_entities.begin(), int_entities.end(), 5) == int_entities.end()) );
   REQUIRE( (std::find(float_entities.begin(), float_entities.end(), 0) == float_entities.end()) );
}

TEST_CASE( "correct UIDs are retrieved from component types after addition and deletion", "[ComponentManager]" )
{
   trecs::ComponentManager cman(120);
   cman.registerComponent<complicatedType_t<26> >();
   cman.registerComponent<complicatedType_t<34> >();
   cman.registerComponent<float>();
   cman.registerComponent<int>();

   // Add five int types to unique entities in the component manager.
   cman.addComponent(0, 2);
   cman.addComponent(1, 4);
   cman.addComponent(2, 8);
   cman.addComponent(3, 16);
   cman.addComponent(4, 32);

   // Add three float types to unique entities in the component manager.
   cman.addComponent(1, 4.5f);
   cman.addComponent(3, -4.5f);
   cman.addComponent(5, -4.5f);

   // Add seven complicated type <26> types to unique entities.
   cman.addComponent(33, complicatedType_t<26>{1, -2.f});
   cman.addComponent(34, complicatedType_t<26>{1, 12.f});
   cman.addComponent(35, complicatedType_t<26>{2, -4.f});
   cman.addComponent(36, complicatedType_t<26>{3, 22.f});
   cman.addComponent(37, complicatedType_t<26>{5, -7.3f});
   cman.addComponent(38, complicatedType_t<26>{8, -9.f});
   cman.addComponent(40, complicatedType_t<26>{13, -11.f});

   // These sizes must match up
   REQUIRE( cman.getComponentEntities<complicatedType_t<34> >().size() == 0 );
   REQUIRE( cman.getComponentEntities<complicatedType_t<26> >().size() == 7 );
   REQUIRE( cman.getComponentEntities<int>().size() == 5 );
   REQUIRE( cman.getComponentEntities<float>().size() == 3 );

   // Remove an int component from a couple entities
   cman.removeComponent<int>(0);
   cman.removeComponent<int>(4);

   auto int_entities = cman.getComponentEntities<int>();

   // Verify that deleting the components from the entities removed those entities
   // from the component's entity list.
   REQUIRE( (std::find(int_entities.begin(), int_entities.end(), 0) == int_entities.end()) );
   REQUIRE( (std::find(int_entities.begin(), int_entities.end(), 4) == int_entities.end()) );

   // Verify that the old entities are still in the component's entity list.
   REQUIRE(
      (
         (std::find(int_entities.begin(), int_entities.end(), 1) != int_entities.end()) &&
         (std::find(int_entities.begin(), int_entities.end(), 2) != int_entities.end()) &&
         (std::find(int_entities.begin(), int_entities.end(), 3) != int_entities.end())
      )
   );

   // Remove the rest of the int components in existence
   cman.removeComponent<int>(1);
   cman.removeComponent<int>(2);
   cman.removeComponent<int>(3);

   int_entities = cman.getComponentEntities<int>();

   REQUIRE( int_entities.size() == 0 );

   // Remove one float component from an entity.
   cman.removeComponent<float>(3);

   auto float_entities = cman.getComponentEntities<float>();

   // Verify that the entity that had its float component deleted isn't in the
   // list of entities that have a float component.
   REQUIRE( std::find(float_entities.begin(), float_entities.end(), 3) == float_entities.end() );

   // Verify that all of the remaining entity UIDs that have float components
   // are found.
   REQUIRE(
      (
         (std::find(float_entities.begin(), float_entities.end(), 1) != float_entities.end()) &&
         (std::find(float_entities.begin(), float_entities.end(), 5) != float_entities.end())
      )
   );

   cman.removeComponent<complicatedType_t<26> >(38);

   auto ct26_entities = cman.getComponentEntities<complicatedType_t<26> >();

   REQUIRE( ct26_entities.size() == 6 );
   REQUIRE( std::find(ct26_entities.begin(), ct26_entities.end(), 38) == ct26_entities.end() );
}

TEST_CASE( "add up to the maximum number of different components", "[ComponentManager]" )
{
   trecs::ComponentManager cman(120);

   cman.registerComponent<component<0> >();
   cman.registerComponent<component<1> >();
   cman.registerComponent<component<2> >();
   cman.registerComponent<component<3> >();
   cman.registerComponent<component<4> >();
   cman.registerComponent<component<5> >();
   cman.registerComponent<component<6> >();
   cman.registerComponent<component<7> >();
   cman.registerComponent<component<8> >();
   cman.registerComponent<component<9> >();
   cman.registerComponent<component<10> >();
   cman.registerComponent<component<11> >();
   cman.registerComponent<component<12> >();
   cman.registerComponent<component<13> >();
   cman.registerComponent<component<14> >();
   cman.registerComponent<component<15> >();
   cman.registerComponent<component<16> >();
   cman.registerComponent<component<17> >();
   cman.registerComponent<component<18> >();
   cman.registerComponent<component<19> >();
   cman.registerComponent<component<20> >();
   cman.registerComponent<component<21> >();
   cman.registerComponent<component<22> >();
   cman.registerComponent<component<23> >();
   cman.registerComponent<component<24> >();
   cman.registerComponent<component<25> >();
   cman.registerComponent<component<26> >();
   cman.registerComponent<component<27> >();
   cman.registerComponent<component<28> >();
   cman.registerComponent<component<29> >();
   cman.registerComponent<component<30> >();
   cman.registerComponent<component<31> >();
   cman.registerComponent<component<32> >();
   cman.registerComponent<component<33> >();
   cman.registerComponent<component<34> >();
   cman.registerComponent<component<35> >();
   cman.registerComponent<component<36> >();
   cman.registerComponent<component<37> >();
   cman.registerComponent<component<38> >();
   cman.registerComponent<component<39> >();
   cman.registerComponent<component<40> >();
   cman.registerComponent<component<41> >();
   cman.registerComponent<component<42> >();
   cman.registerComponent<component<43> >();
   cman.registerComponent<component<44> >();
   cman.registerComponent<component<45> >();
   cman.registerComponent<component<46> >();
   cman.registerComponent<component<47> >();
   cman.registerComponent<component<48> >();
   cman.registerComponent<component<49> >();
   cman.registerComponent<component<50> >();
   cman.registerComponent<component<51> >();
   cman.registerComponent<component<52> >();
   cman.registerComponent<component<53> >();
   cman.registerComponent<component<54> >();
   cman.registerComponent<component<55> >();
   cman.registerComponent<component<56> >();
   cman.registerComponent<component<57> >();
   cman.registerComponent<component<58> >();
   cman.registerComponent<component<59> >();
   cman.registerComponent<component<60> >();
   cman.registerComponent<component<61> >();
   cman.registerComponent<component<62> >();
   cman.registerComponent<component<63> >();
   cman.registerComponent<component<64> >();
   cman.registerComponent<component<65> >();
   cman.registerComponent<component<66> >();
   cman.registerComponent<component<67> >();
   cman.registerComponent<component<68> >();
   cman.registerComponent<component<69> >();
   cman.registerComponent<component<70> >();
   cman.registerComponent<component<71> >();
   cman.registerComponent<component<72> >();
   cman.registerComponent<component<73> >();
   cman.registerComponent<component<74> >();
   cman.registerComponent<component<75> >();
   cman.registerComponent<component<76> >();
   cman.registerComponent<component<77> >();
   cman.registerComponent<component<78> >();
   cman.registerComponent<component<79> >();
   cman.registerComponent<component<80> >();
   cman.registerComponent<component<81> >();
   cman.registerComponent<component<82> >();
   cman.registerComponent<component<83> >();
   cman.registerComponent<component<84> >();
   cman.registerComponent<component<85> >();
   cman.registerComponent<component<86> >();
   cman.registerComponent<component<87> >();
   cman.registerComponent<component<88> >();
   cman.registerComponent<component<89> >();
   cman.registerComponent<component<90> >();
   cman.registerComponent<component<91> >();
   cman.registerComponent<component<92> >();
   cman.registerComponent<component<93> >();
   cman.registerComponent<component<94> >();
   cman.registerComponent<component<95> >();
   cman.registerComponent<component<96> >();
   cman.registerComponent<component<97> >();
   cman.registerComponent<component<98> >();
   cman.registerComponent<component<99> >();
   cman.registerComponent<component<100> >();
   cman.registerComponent<component<101> >();
   cman.registerComponent<component<102> >();
   cman.registerComponent<component<103> >();
   cman.registerComponent<component<104> >();
   cman.registerComponent<component<105> >();
   cman.registerComponent<component<106> >();
   cman.registerComponent<component<107> >();
   cman.registerComponent<component<108> >();
   cman.registerComponent<component<109> >();
   cman.registerComponent<component<110> >();
   cman.registerComponent<component<111> >();
   cman.registerComponent<component<112> >();
   cman.registerComponent<component<113> >();
   cman.registerComponent<component<114> >();
   cman.registerComponent<component<115> >();
   cman.registerComponent<component<116> >();
   cman.registerComponent<component<117> >();
   cman.registerComponent<component<118> >();
   cman.registerComponent<component<119> >();
   cman.registerComponent<component<120> >();
   cman.registerComponent<component<121> >();
   cman.registerComponent<component<122> >();
   cman.registerComponent<component<123> >();
   cman.registerComponent<component<124> >();
   cman.registerComponent<component<125> >();
   cman.registerComponent<component<126> >();
   cman.registerComponent<component<127> >();
   cman.registerComponent<component<128> >();
   cman.registerComponent<component<129> >();
   cman.registerComponent<component<130> >();
   cman.registerComponent<component<131> >();
   cman.registerComponent<component<132> >();
   cman.registerComponent<component<133> >();
   cman.registerComponent<component<134> >();
   cman.registerComponent<component<135> >();
   cman.registerComponent<component<136> >();
   cman.registerComponent<component<137> >();
   cman.registerComponent<component<138> >();
   cman.registerComponent<component<139> >();
   cman.registerComponent<component<140> >();
   cman.registerComponent<component<141> >();
   cman.registerComponent<component<142> >();
   cman.registerComponent<component<143> >();
   cman.registerComponent<component<144> >();
   cman.registerComponent<component<145> >();
   cman.registerComponent<component<146> >();
   cman.registerComponent<component<147> >();
   cman.registerComponent<component<148> >();
   cman.registerComponent<component<149> >();
   cman.registerComponent<component<150> >();
   cman.registerComponent<component<151> >();
   cman.registerComponent<component<152> >();
   cman.registerComponent<component<153> >();
   cman.registerComponent<component<154> >();
   cman.registerComponent<component<155> >();
   cman.registerComponent<component<156> >();
   cman.registerComponent<component<157> >();
   cman.registerComponent<component<158> >();
   cman.registerComponent<component<159> >();
   cman.registerComponent<component<160> >();
   cman.registerComponent<component<161> >();
   cman.registerComponent<component<162> >();
   cman.registerComponent<component<163> >();
   cman.registerComponent<component<164> >();
   cman.registerComponent<component<165> >();
   cman.registerComponent<component<166> >();
   cman.registerComponent<component<167> >();
   cman.registerComponent<component<168> >();
   cman.registerComponent<component<169> >();
   cman.registerComponent<component<170> >();
   cman.registerComponent<component<171> >();
   cman.registerComponent<component<172> >();
   cman.registerComponent<component<173> >();
   cman.registerComponent<component<174> >();
   cman.registerComponent<component<175> >();
   cman.registerComponent<component<176> >();
   cman.registerComponent<component<177> >();
   cman.registerComponent<component<178> >();
   cman.registerComponent<component<179> >();
   cman.registerComponent<component<180> >();
   cman.registerComponent<component<181> >();
   cman.registerComponent<component<182> >();
   cman.registerComponent<component<183> >();
   cman.registerComponent<component<184> >();
   cman.registerComponent<component<185> >();
   cman.registerComponent<component<186> >();
   cman.registerComponent<component<187> >();
   cman.registerComponent<component<188> >();
   cman.registerComponent<component<189> >();
   cman.registerComponent<component<190> >();
   cman.registerComponent<component<191> >();
   cman.registerComponent<component<192> >();
   cman.registerComponent<component<193> >();
   cman.registerComponent<component<194> >();
   cman.registerComponent<component<195> >();
   cman.registerComponent<component<196> >();
   cman.registerComponent<component<197> >();
   cman.registerComponent<component<198> >();
   cman.registerComponent<component<199> >();
   cman.registerComponent<component<200> >();
   cman.registerComponent<component<201> >();
   cman.registerComponent<component<202> >();
   cman.registerComponent<component<203> >();
   cman.registerComponent<component<204> >();
   cman.registerComponent<component<205> >();
   cman.registerComponent<component<206> >();
   cman.registerComponent<component<207> >();
   cman.registerComponent<component<208> >();
   cman.registerComponent<component<209> >();
   cman.registerComponent<component<210> >();
   cman.registerComponent<component<211> >();
   cman.registerComponent<component<212> >();
   cman.registerComponent<component<213> >();
   cman.registerComponent<component<214> >();
   cman.registerComponent<component<215> >();
   cman.registerComponent<component<216> >();
   cman.registerComponent<component<217> >();
   cman.registerComponent<component<218> >();
   cman.registerComponent<component<219> >();
   cman.registerComponent<component<220> >();
   cman.registerComponent<component<221> >();
   cman.registerComponent<component<222> >();
   cman.registerComponent<component<223> >();
   cman.registerComponent<component<224> >();
   cman.registerComponent<component<225> >();
   cman.registerComponent<component<226> >();
   cman.registerComponent<component<227> >();
   cman.registerComponent<component<228> >();
   cman.registerComponent<component<229> >();
   cman.registerComponent<component<230> >();
   cman.registerComponent<component<231> >();
   cman.registerComponent<component<232> >();
   cman.registerComponent<component<233> >();
   cman.registerComponent<component<234> >();
   cman.registerComponent<component<235> >();
   cman.registerComponent<component<236> >();
   cman.registerComponent<component<237> >();
   cman.registerComponent<component<238> >();
   cman.registerComponent<component<239> >();
   cman.registerComponent<component<240> >();
   cman.registerComponent<component<241> >();
   cman.registerComponent<component<242> >();
   cman.registerComponent<component<243> >();
   cman.registerComponent<component<244> >();
   cman.registerComponent<component<245> >();
   cman.registerComponent<component<246> >();
   cman.registerComponent<component<247> >();
   cman.registerComponent<component<248> >();
   cman.registerComponent<component<249> >();
   cman.registerComponent<component<250> >();
   cman.registerComponent<component<251> >();
   cman.registerComponent<component<252> >();
   cman.registerComponent<component<253> >();
   cman.registerComponent<component<254> >();
   cman.registerComponent<component<255> >();
   cman.registerComponent<component<256> >();
   cman.registerComponent<component<257> >();
   cman.registerComponent<component<258> >();
   cman.registerComponent<component<259> >();
   cman.registerComponent<component<260> >();
   cman.registerComponent<component<261> >();
   cman.registerComponent<component<262> >();
   cman.registerComponent<component<263> >();
   cman.registerComponent<component<264> >();
   cman.registerComponent<component<265> >();
   cman.registerComponent<component<266> >();
   cman.registerComponent<component<267> >();
   cman.registerComponent<component<268> >();
   cman.registerComponent<component<269> >();
   cman.registerComponent<component<270> >();
   cman.registerComponent<component<271> >();
   cman.registerComponent<component<272> >();
   cman.registerComponent<component<273> >();
   cman.registerComponent<component<274> >();
   cman.registerComponent<component<275> >();
   cman.registerComponent<component<276> >();
   cman.registerComponent<component<277> >();
   cman.registerComponent<component<278> >();
   cman.registerComponent<component<279> >();
   cman.registerComponent<component<280> >();
   cman.registerComponent<component<281> >();
   cman.registerComponent<component<282> >();
   cman.registerComponent<component<283> >();
   cman.registerComponent<component<284> >();
   cman.registerComponent<component<285> >();
   cman.registerComponent<component<286> >();
   cman.registerComponent<component<287> >();
   cman.registerComponent<component<288> >();
   cman.registerComponent<component<289> >();
   cman.registerComponent<component<290> >();
   cman.registerComponent<component<291> >();
   cman.registerComponent<component<292> >();
   cman.registerComponent<component<293> >();
   cman.registerComponent<component<294> >();
   cman.registerComponent<component<295> >();
   cman.registerComponent<component<296> >();
   cman.registerComponent<component<297> >();
   cman.registerComponent<component<298> >();
   cman.registerComponent<component<299> >();
   cman.registerComponent<component<300> >();
   cman.registerComponent<component<301> >();
   cman.registerComponent<component<302> >();
   cman.registerComponent<component<303> >();
   cman.registerComponent<component<304> >();
   cman.registerComponent<component<305> >();
   cman.registerComponent<component<306> >();
   cman.registerComponent<component<307> >();
   cman.registerComponent<component<308> >();
   cman.registerComponent<component<309> >();
   cman.registerComponent<component<310> >();
   cman.registerComponent<component<311> >();
   cman.registerComponent<component<312> >();
   cman.registerComponent<component<313> >();
   cman.registerComponent<component<314> >();
   cman.registerComponent<component<315> >();
   cman.registerComponent<component<316> >();
   cman.registerComponent<component<317> >();
   cman.registerComponent<component<318> >();
   cman.registerComponent<component<319> >();
   cman.registerComponent<component<320> >();
   cman.registerComponent<component<321> >();
   cman.registerComponent<component<322> >();
   cman.registerComponent<component<323> >();
   cman.registerComponent<component<324> >();
   cman.registerComponent<component<325> >();
   cman.registerComponent<component<326> >();
   cman.registerComponent<component<327> >();
   cman.registerComponent<component<328> >();
   cman.registerComponent<component<329> >();
   cman.registerComponent<component<330> >();
   cman.registerComponent<component<331> >();
   cman.registerComponent<component<332> >();
   cman.registerComponent<component<333> >();
   cman.registerComponent<component<334> >();
   cman.registerComponent<component<335> >();
   cman.registerComponent<component<336> >();
   cman.registerComponent<component<337> >();
   cman.registerComponent<component<338> >();
   cman.registerComponent<component<339> >();
   cman.registerComponent<component<340> >();
   cman.registerComponent<component<341> >();
   cman.registerComponent<component<342> >();
   cman.registerComponent<component<343> >();
   cman.registerComponent<component<344> >();
   cman.registerComponent<component<345> >();
   cman.registerComponent<component<346> >();
   cman.registerComponent<component<347> >();
   cman.registerComponent<component<348> >();
   cman.registerComponent<component<349> >();
   cman.registerComponent<component<350> >();
   cman.registerComponent<component<351> >();
   cman.registerComponent<component<352> >();
   cman.registerComponent<component<353> >();
   cman.registerComponent<component<354> >();
   cman.registerComponent<component<355> >();
   cman.registerComponent<component<356> >();
   cman.registerComponent<component<357> >();
   cman.registerComponent<component<358> >();
   cman.registerComponent<component<359> >();
   cman.registerComponent<component<360> >();
   cman.registerComponent<component<361> >();
   cman.registerComponent<component<362> >();
   cman.registerComponent<component<363> >();
   cman.registerComponent<component<364> >();
   cman.registerComponent<component<365> >();
   cman.registerComponent<component<366> >();
   cman.registerComponent<component<367> >();
   cman.registerComponent<component<368> >();
   cman.registerComponent<component<369> >();
   cman.registerComponent<component<370> >();
   cman.registerComponent<component<371> >();
   cman.registerComponent<component<372> >();
   cman.registerComponent<component<373> >();
   cman.registerComponent<component<374> >();
   cman.registerComponent<component<375> >();
   cman.registerComponent<component<376> >();
   cman.registerComponent<component<377> >();
   cman.registerComponent<component<378> >();
   cman.registerComponent<component<379> >();
   cman.registerComponent<component<380> >();
   cman.registerComponent<component<381> >();
   cman.registerComponent<component<382> >();
   cman.registerComponent<component<383> >();
   cman.registerComponent<component<384> >();
   cman.registerComponent<component<385> >();
   cman.registerComponent<component<386> >();
   cman.registerComponent<component<387> >();
   cman.registerComponent<component<388> >();
   cman.registerComponent<component<389> >();
   cman.registerComponent<component<390> >();
   cman.registerComponent<component<391> >();
   cman.registerComponent<component<392> >();
   cman.registerComponent<component<393> >();
   cman.registerComponent<component<394> >();
   cman.registerComponent<component<395> >();
   cman.registerComponent<component<396> >();
   cman.registerComponent<component<397> >();
   cman.registerComponent<component<398> >();
   cman.registerComponent<component<399> >();
   cman.registerComponent<component<400> >();
   cman.registerComponent<component<401> >();
   cman.registerComponent<component<402> >();
   cman.registerComponent<component<403> >();
   cman.registerComponent<component<404> >();
   cman.registerComponent<component<405> >();
   cman.registerComponent<component<406> >();
   cman.registerComponent<component<407> >();
   cman.registerComponent<component<408> >();
   cman.registerComponent<component<409> >();
   cman.registerComponent<component<410> >();
   cman.registerComponent<component<411> >();
   cman.registerComponent<component<412> >();
   cman.registerComponent<component<413> >();
   cman.registerComponent<component<414> >();
   cman.registerComponent<component<415> >();
   cman.registerComponent<component<416> >();
   cman.registerComponent<component<417> >();
   cman.registerComponent<component<418> >();
   cman.registerComponent<component<419> >();
   cman.registerComponent<component<420> >();
   cman.registerComponent<component<421> >();
   cman.registerComponent<component<422> >();
   cman.registerComponent<component<423> >();
   cman.registerComponent<component<424> >();
   cman.registerComponent<component<425> >();
   cman.registerComponent<component<426> >();
   cman.registerComponent<component<427> >();
   cman.registerComponent<component<428> >();
   cman.registerComponent<component<429> >();
   cman.registerComponent<component<430> >();
   cman.registerComponent<component<431> >();
   cman.registerComponent<component<432> >();
   cman.registerComponent<component<433> >();
   cman.registerComponent<component<434> >();
   cman.registerComponent<component<435> >();
   cman.registerComponent<component<436> >();
   cman.registerComponent<component<437> >();
   cman.registerComponent<component<438> >();
   cman.registerComponent<component<439> >();
   cman.registerComponent<component<440> >();
   cman.registerComponent<component<441> >();
   cman.registerComponent<component<442> >();
   cman.registerComponent<component<443> >();
   cman.registerComponent<component<444> >();
   cman.registerComponent<component<445> >();
   cman.registerComponent<component<446> >();
   cman.registerComponent<component<447> >();
   cman.registerComponent<component<448> >();
   cman.registerComponent<component<449> >();
   cman.registerComponent<component<450> >();
   cman.registerComponent<component<451> >();
   cman.registerComponent<component<452> >();
   cman.registerComponent<component<453> >();
   cman.registerComponent<component<454> >();
   cman.registerComponent<component<455> >();
   cman.registerComponent<component<456> >();
   cman.registerComponent<component<457> >();
   cman.registerComponent<component<458> >();
   cman.registerComponent<component<459> >();
   cman.registerComponent<component<460> >();
   cman.registerComponent<component<461> >();
   cman.registerComponent<component<462> >();
   cman.registerComponent<component<463> >();
   cman.registerComponent<component<464> >();
   cman.registerComponent<component<465> >();
   cman.registerComponent<component<466> >();
   cman.registerComponent<component<467> >();
   cman.registerComponent<component<468> >();
   cman.registerComponent<component<469> >();
   cman.registerComponent<component<470> >();
   cman.registerComponent<component<471> >();
   cman.registerComponent<component<472> >();
   cman.registerComponent<component<473> >();
   cman.registerComponent<component<474> >();
   cman.registerComponent<component<475> >();
   cman.registerComponent<component<476> >();
   cman.registerComponent<component<477> >();
   cman.registerComponent<component<478> >();
   cman.registerComponent<component<479> >();
   cman.registerComponent<component<480> >();
   cman.registerComponent<component<481> >();
   cman.registerComponent<component<482> >();
   cman.registerComponent<component<483> >();
   cman.registerComponent<component<484> >();
   cman.registerComponent<component<485> >();
   cman.registerComponent<component<486> >();
   cman.registerComponent<component<487> >();
   cman.registerComponent<component<488> >();
   cman.registerComponent<component<489> >();
   cman.registerComponent<component<490> >();
   cman.registerComponent<component<491> >();
   cman.registerComponent<component<492> >();
   cman.registerComponent<component<493> >();
   cman.registerComponent<component<494> >();
   cman.registerComponent<component<495> >();
   cman.registerComponent<component<496> >();
   cman.registerComponent<component<497> >();
   cman.registerComponent<component<498> >();
   cman.registerComponent<component<499> >();
   cman.registerComponent<component<500> >();
   cman.registerComponent<component<501> >();
   cman.registerComponent<component<502> >();
   cman.registerComponent<component<503> >();
   cman.registerComponent<component<504> >();
   cman.registerComponent<component<505> >();
   cman.registerComponent<component<506> >();
   cman.registerComponent<component<507> >();
   cman.registerComponent<component<508> >();
   cman.registerComponent<component<509> >();
   cman.registerComponent<component<510> >();
   cman.registerComponent<component<511> >();

   REQUIRE( cman.getNumSignatures() == trecs::max_num_signatures );
}

TEST_CASE( "assignment operator empty to empty", "[ComponentManager]")
{
   trecs::ComponentManager cman1(120);
   trecs::ComponentManager cman2(120);

   cman2 = cman1;

   REQUIRE( cman2.getNumSignatures() == cman1.getNumSignatures() );
}

TEST_CASE( "assignment operator to empty destination", "[ComponentManager]" )
{
   trecs::ComponentManager cman1(120);
   cman1.registerComponent<complicatedType_t<26> >();
   cman1.registerComponent<complicatedType_t<34> >();
   cman1.registerComponent<float>();
   cman1.registerComponent<int>();

   std::vector<int> its;
   its.push_back(2);
   its.push_back(4);
   its.push_back(8);
   its.push_back(16);
   its.push_back(32);

   // Add five int types to unique entities in the component manager.
   cman1.addComponent(0, its[0]);
   cman1.addComponent(1, its[1]);
   cman1.addComponent(2, its[2]);
   cman1.addComponent(3, its[3]);
   cman1.addComponent(4, its[4]);

   std::vector<float> fts;
   fts.push_back(4.5f);
   fts.push_back(-4.5f);
   fts.push_back(-4.5f);

   // Add three float types to unique entities in the component manager.
   cman1.addComponent(1, fts[0]);
   cman1.addComponent(3, fts[1]);
   cman1.addComponent(5, fts[2]);

   std::vector<complicatedType_t<26> > cts;
   cts.push_back(complicatedType_t<26>{1, -2.f});
   cts.push_back(complicatedType_t<26>{1, 12.f});
   cts.push_back(complicatedType_t<26>{2, -4.f});
   cts.push_back(complicatedType_t<26>{3, 22.f});
   cts.push_back(complicatedType_t<26>{5, -7.3f});
   cts.push_back(complicatedType_t<26>{8, -9.f});
   cts.push_back(complicatedType_t<26>{13, -11.f});

   // Add seven complicated type <26> types to unique entities.
   cman1.addComponent(33, cts[0]);
   cman1.addComponent(34, cts[1]);
   cman1.addComponent(35, cts[2]);
   cman1.addComponent(36, cts[3]);
   cman1.addComponent(37, cts[4]);
   cman1.addComponent(38, cts[5]);
   cman1.addComponent(40, cts[6]);

   trecs::ComponentManager cman2(120);

   // These have to be cached here, because after assignment none of the
   // allocator-related functionality will work in cman1.
   auto num_floats = cman1.getNumComponents<float>();
   auto num_ints = cman1.getNumComponents<int>();
   auto num_cts = cman1.getNumComponents<complicatedType_t<26> >();

   cman2 = cman1;

   // Check number of signatures
   REQUIRE( cman2.getNumSignatures() == cman1.getNumSignatures() );
   
   // Check number of components
   REQUIRE( cman2.getNumComponents<float>() == num_floats );
   REQUIRE( cman2.getNumComponents<int>() == num_ints );
   REQUIRE( cman2.getNumComponents<complicatedType_t<26> >() == num_cts );

   auto complicated_types = cman2.getComponents<complicatedType_t<26> >();

   REQUIRE( cman2.getSignature<complicatedType_t<26> >() != trecs::error_signature);

   REQUIRE( cman2.getComponent<complicatedType_t<26> >(33) != nullptr );

   REQUIRE( cman2.getComponent<complicatedType_t<26> >(33)->int_field == cts[0].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(33)->float_field == cts[0].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(34)->int_field == cts[1].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(34)->float_field == cts[1].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(35)->int_field == cts[2].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(35)->float_field == cts[2].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(36)->int_field == cts[3].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(36)->float_field == cts[3].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(37)->int_field == cts[4].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(37)->float_field == cts[4].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(38)->int_field == cts[5].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(38)->float_field == cts[5].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(40)->int_field == cts[6].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(40)->float_field == cts[6].float_field );

   REQUIRE( *cman2.getComponent<float>(1) == fts[0] );
   REQUIRE( *cman2.getComponent<float>(3) == fts[1] );
   REQUIRE( *cman2.getComponent<float>(5) == fts[2] );

   REQUIRE( *cman2.getComponent<int>(0) == its[0] );
   REQUIRE( *cman2.getComponent<int>(1) == its[1] );
   REQUIRE( *cman2.getComponent<int>(2) == its[2] );
   REQUIRE( *cman2.getComponent<int>(3) == its[3] );
   REQUIRE( *cman2.getComponent<int>(4) == its[4] );
}

TEST_CASE( "assignment operator to non-empty destination", "[ComponentManager]" )
{
   trecs::ComponentManager cman1(120);
   cman1.registerComponent<complicatedType_t<26> >();
   cman1.registerComponent<complicatedType_t<34> >();
   cman1.registerComponent<float>();
   cman1.registerComponent<int>();

   std::vector<int> its;
   its.push_back(2);
   its.push_back(4);
   its.push_back(8);
   its.push_back(16);
   its.push_back(32);

   // Add five int types to unique entities in the component manager.
   cman1.addComponent(0, its[0]);
   cman1.addComponent(1, its[1]);
   cman1.addComponent(2, its[2]);
   cman1.addComponent(3, its[3]);
   cman1.addComponent(4, its[4]);

   std::vector<float> fts;
   fts.push_back(4.5f);
   fts.push_back(-4.5f);
   fts.push_back(-4.5f);

   // Add three float types to unique entities in the component manager.
   cman1.addComponent(1, fts[0]);
   cman1.addComponent(3, fts[1]);
   cman1.addComponent(5, fts[2]);

   std::vector<complicatedType_t<26> > cts;
   cts.push_back(complicatedType_t<26>{1, -2.f});
   cts.push_back(complicatedType_t<26>{1, 12.f});
   cts.push_back(complicatedType_t<26>{2, -4.f});
   cts.push_back(complicatedType_t<26>{3, 22.f});
   cts.push_back(complicatedType_t<26>{5, -7.3f});
   cts.push_back(complicatedType_t<26>{8, -9.f});
   cts.push_back(complicatedType_t<26>{13, -11.f});

   // Add seven complicated type <26> types to unique entities.
   cman1.addComponent(33, cts[0]);
   cman1.addComponent(34, cts[1]);
   cman1.addComponent(35, cts[2]);
   cman1.addComponent(36, cts[3]);
   cman1.addComponent(37, cts[4]);
   cman1.addComponent(38, cts[5]);
   cman1.addComponent(40, cts[6]);

   trecs::ComponentManager cman2(120);

   cman2.registerComponent<double>();
   cman2.addComponent(0, 4.0);
   cman2.addComponent(1, -8.0);
   cman2.addComponent(2, 16.0);

   cman2.registerComponent<unsigned char>();
   cman2.addComponent(0, 233);
   cman2.addComponent(1, 4);
   cman2.addComponent(2, 55);

   // These have to be cached here, because after assignment none of the
   // allocator-related functionality will work in cman1.
   auto num_floats = cman1.getNumComponents<float>();
   auto num_ints = cman1.getNumComponents<int>();
   auto num_cts = cman1.getNumComponents<complicatedType_t<26> >();

   cman2 = cman1;

   REQUIRE( cman2.getNumComponents<double>() == 0 );

   // Check number of signatures
   REQUIRE( cman2.getNumSignatures() == cman1.getNumSignatures() );
   
   // Check number of components
   REQUIRE( cman2.getNumComponents<float>() == num_floats );
   REQUIRE( cman2.getNumComponents<int>() == num_ints );
   REQUIRE( cman2.getNumComponents<complicatedType_t<26> >() == num_cts );

   auto complicated_types = cman2.getComponents<complicatedType_t<26> >();

   REQUIRE( cman2.getSignature<complicatedType_t<26> >() != trecs::error_signature);

   REQUIRE( cman2.getComponent<complicatedType_t<26> >(33) != nullptr );

   REQUIRE( cman2.getComponent<complicatedType_t<26> >(33)->int_field == cts[0].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(33)->float_field == cts[0].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(34)->int_field == cts[1].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(34)->float_field == cts[1].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(35)->int_field == cts[2].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(35)->float_field == cts[2].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(36)->int_field == cts[3].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(36)->float_field == cts[3].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(37)->int_field == cts[4].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(37)->float_field == cts[4].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(38)->int_field == cts[5].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(38)->float_field == cts[5].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(40)->int_field == cts[6].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(40)->float_field == cts[6].float_field );

   REQUIRE( *cman2.getComponent<float>(1) == fts[0] );
   REQUIRE( *cman2.getComponent<float>(3) == fts[1] );
   REQUIRE( *cman2.getComponent<float>(5) == fts[2] );

   REQUIRE( *cman2.getComponent<int>(0) == its[0] );
   REQUIRE( *cman2.getComponent<int>(1) == its[1] );
   REQUIRE( *cman2.getComponent<int>(2) == its[2] );
   REQUIRE( *cman2.getComponent<int>(3) == its[3] );
   REQUIRE( *cman2.getComponent<int>(4) == its[4] );
}

TEST_CASE( "assign empty to non-empty", "[ComponentManager]" )
{
   trecs::ComponentManager cman1(120);

   trecs::ComponentManager cman2(120);

   cman2.registerComponent<double>();
   cman2.addComponent(0, 4.0);
   cman2.addComponent(1, -8.0);
   cman2.addComponent(2, 16.0);

   cman2.registerComponent<unsigned char>();
   cman2.addComponent(0, 233);
   cman2.addComponent(1, 4);
   cman2.addComponent(2, 55);

   cman2 = cman1;

   REQUIRE( cman2.getNumSignatures() == 0 );
   REQUIRE( cman2.getNumComponents<double>() == 0 );
   REQUIRE( cman2.getNumComponents<unsigned char>() == 0 );
}

TEST_CASE( "assignment operator from const source to empty destination", "[ComponentManager]" )
{
   trecs::ComponentManager cman1(120);
   cman1.registerComponent<complicatedType_t<26> >();
   cman1.registerComponent<complicatedType_t<34> >();
   cman1.registerComponent<float>();
   cman1.registerComponent<int>();

   std::vector<int> its;
   its.push_back(2);
   its.push_back(4);
   its.push_back(8);
   its.push_back(16);
   its.push_back(32);

   // Add five int types to unique entities in the component manager.
   cman1.addComponent(0, its[0]);
   cman1.addComponent(1, its[1]);
   cman1.addComponent(2, its[2]);
   cman1.addComponent(3, its[3]);
   cman1.addComponent(4, its[4]);

   std::vector<float> fts;
   fts.push_back(4.5f);
   fts.push_back(-4.5f);
   fts.push_back(-4.5f);

   // Add three float types to unique entities in the component manager.
   cman1.addComponent(1, fts[0]);
   cman1.addComponent(3, fts[1]);
   cman1.addComponent(5, fts[2]);

   std::vector<complicatedType_t<26> > cts;
   cts.push_back(complicatedType_t<26>{1, -2.f});
   cts.push_back(complicatedType_t<26>{1, 12.f});
   cts.push_back(complicatedType_t<26>{2, -4.f});
   cts.push_back(complicatedType_t<26>{3, 22.f});
   cts.push_back(complicatedType_t<26>{5, -7.3f});
   cts.push_back(complicatedType_t<26>{8, -9.f});
   cts.push_back(complicatedType_t<26>{13, -11.f});

   // Add seven complicated type <26> types to unique entities.
   cman1.addComponent(33, cts[0]);
   cman1.addComponent(34, cts[1]);
   cman1.addComponent(35, cts[2]);
   cman1.addComponent(36, cts[3]);
   cman1.addComponent(37, cts[4]);
   cman1.addComponent(38, cts[5]);
   cman1.addComponent(40, cts[6]);

   trecs::ComponentManager cman2(120);

   // These have to be cached here, because after assignment none of the
   // allocator-related functionality will work in cman1.
   auto num_floats = cman1.getNumComponents<float>();
   auto num_ints = cman1.getNumComponents<int>();
   auto num_cts = cman1.getNumComponents<complicatedType_t<26> >();

   // cman2 = cman1;
   artificial_assignment(cman2, cman1);
   cman1.release();

   // Check number of signatures
   REQUIRE( cman2.getNumSignatures() == cman1.getNumSignatures() );
   
   // Check number of components
   REQUIRE( cman2.getNumComponents<float>() == num_floats );
   REQUIRE( cman2.getNumComponents<int>() == num_ints );
   REQUIRE( cman2.getNumComponents<complicatedType_t<26> >() == num_cts );

   auto complicated_types = cman2.getComponents<complicatedType_t<26> >();

   REQUIRE( cman2.getSignature<complicatedType_t<26> >() != trecs::error_signature);

   REQUIRE( cman2.getComponent<complicatedType_t<26> >(33) != nullptr );

   REQUIRE( cman2.getComponent<complicatedType_t<26> >(33)->int_field == cts[0].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(33)->float_field == cts[0].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(34)->int_field == cts[1].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(34)->float_field == cts[1].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(35)->int_field == cts[2].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(35)->float_field == cts[2].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(36)->int_field == cts[3].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(36)->float_field == cts[3].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(37)->int_field == cts[4].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(37)->float_field == cts[4].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(38)->int_field == cts[5].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(38)->float_field == cts[5].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(40)->int_field == cts[6].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(40)->float_field == cts[6].float_field );

   REQUIRE( *cman2.getComponent<float>(1) == fts[0] );
   REQUIRE( *cman2.getComponent<float>(3) == fts[1] );
   REQUIRE( *cman2.getComponent<float>(5) == fts[2] );

   REQUIRE( *cman2.getComponent<int>(0) == its[0] );
   REQUIRE( *cman2.getComponent<int>(1) == its[1] );
   REQUIRE( *cman2.getComponent<int>(2) == its[2] );
   REQUIRE( *cman2.getComponent<int>(3) == its[3] );
   REQUIRE( *cman2.getComponent<int>(4) == its[4] );
}

TEST_CASE( "assignment operator from const source to non-empty destination", "[ComponentManager]" )
{
   trecs::ComponentManager cman1(120);
   cman1.registerComponent<complicatedType_t<26> >();
   cman1.registerComponent<complicatedType_t<34> >();
   cman1.registerComponent<float>();
   cman1.registerComponent<int>();

   std::vector<int> its;
   its.push_back(2);
   its.push_back(4);
   its.push_back(8);
   its.push_back(16);
   its.push_back(32);

   // Add five int types to unique entities in the component manager.
   cman1.addComponent(0, its[0]);
   cman1.addComponent(1, its[1]);
   cman1.addComponent(2, its[2]);
   cman1.addComponent(3, its[3]);
   cman1.addComponent(4, its[4]);

   std::vector<float> fts;
   fts.push_back(4.5f);
   fts.push_back(-4.5f);
   fts.push_back(-4.5f);

   // Add three float types to unique entities in the component manager.
   cman1.addComponent(1, fts[0]);
   cman1.addComponent(3, fts[1]);
   cman1.addComponent(5, fts[2]);

   std::vector<complicatedType_t<26> > cts;
   cts.push_back(complicatedType_t<26>{1, -2.f});
   cts.push_back(complicatedType_t<26>{1, 12.f});
   cts.push_back(complicatedType_t<26>{2, -4.f});
   cts.push_back(complicatedType_t<26>{3, 22.f});
   cts.push_back(complicatedType_t<26>{5, -7.3f});
   cts.push_back(complicatedType_t<26>{8, -9.f});
   cts.push_back(complicatedType_t<26>{13, -11.f});

   // Add seven complicated type <26> types to unique entities.
   cman1.addComponent(33, cts[0]);
   cman1.addComponent(34, cts[1]);
   cman1.addComponent(35, cts[2]);
   cman1.addComponent(36, cts[3]);
   cman1.addComponent(37, cts[4]);
   cman1.addComponent(38, cts[5]);
   cman1.addComponent(40, cts[6]);

   trecs::ComponentManager cman2(120);

   cman2.registerComponent<double>();
   cman2.addComponent(0, 4.0);
   cman2.addComponent(1, -8.0);
   cman2.addComponent(2, 16.0);

   cman2.registerComponent<unsigned char>();
   cman2.addComponent(0, 233);
   cman2.addComponent(1, 4);
   cman2.addComponent(2, 55);

   // These have to be cached here, because after assignment none of the
   // allocator-related functionality will work in cman1.
   auto num_floats = cman1.getNumComponents<float>();
   auto num_ints = cman1.getNumComponents<int>();
   auto num_cts = cman1.getNumComponents<complicatedType_t<26> >();

   // cman2 = cman1;
   artificial_assignment(cman2, cman1);
   cman1.release();

   REQUIRE( cman2.getNumComponents<double>() == 0 );

   // Check number of signatures
   REQUIRE( cman2.getNumSignatures() == cman1.getNumSignatures() );
   
   // Check number of components
   REQUIRE( cman2.getNumComponents<float>() == num_floats );
   REQUIRE( cman2.getNumComponents<int>() == num_ints );
   REQUIRE( cman2.getNumComponents<complicatedType_t<26> >() == num_cts );

   auto complicated_types = cman2.getComponents<complicatedType_t<26> >();

   REQUIRE( cman2.getSignature<complicatedType_t<26> >() != trecs::error_signature);

   REQUIRE( cman2.getComponent<complicatedType_t<26> >(33) != nullptr );

   REQUIRE( cman2.getComponent<complicatedType_t<26> >(33)->int_field == cts[0].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(33)->float_field == cts[0].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(34)->int_field == cts[1].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(34)->float_field == cts[1].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(35)->int_field == cts[2].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(35)->float_field == cts[2].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(36)->int_field == cts[3].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(36)->float_field == cts[3].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(37)->int_field == cts[4].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(37)->float_field == cts[4].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(38)->int_field == cts[5].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(38)->float_field == cts[5].float_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(40)->int_field == cts[6].int_field );
   REQUIRE( cman2.getComponent<complicatedType_t<26> >(40)->float_field == cts[6].float_field );

   REQUIRE( *cman2.getComponent<float>(1) == fts[0] );
   REQUIRE( *cman2.getComponent<float>(3) == fts[1] );
   REQUIRE( *cman2.getComponent<float>(5) == fts[2] );

   REQUIRE( *cman2.getComponent<int>(0) == its[0] );
   REQUIRE( *cman2.getComponent<int>(1) == its[1] );
   REQUIRE( *cman2.getComponent<int>(2) == its[2] );
   REQUIRE( *cman2.getComponent<int>(3) == its[3] );
   REQUIRE( *cman2.getComponent<int>(4) == its[4] );
}

TEST_CASE( "assignment to unconstructed CM", "[ComponentManager]" )
{
   typedef struct byte_cm
   {
      byte_cm(void)
      {
         for (int i = 0; i < 512; ++i)
         {
            bytes[i] = 0;
         }
      }

      unsigned char bytes[512];
   } byte_cm_t;

   byte_cm_t raw_bytes;

   trecs::ComponentManager * byte_cm = reinterpret_cast<trecs::ComponentManager *>(&(raw_bytes.bytes[0]));

   trecs::ComponentManager cm(128);

   cm.registerComponent<float>();
   cm.registerComponent<int>();
   cm.registerComponent<complicatedType_t<24> >();

   cm.addComponent(0, 3.123f);
   cm.addComponent(0, 3);
   cm.addComponent(0, complicatedType_t<24>{987, -1234.32456f});

   cm.addComponent(1, 3.123f);
   cm.addComponent(1, 3);
   cm.addComponent(1, complicatedType_t<24>{987, -1234.32456f});

   cm.addComponent(2, 3.123f);
   cm.addComponent(2, 3);
   cm.addComponent(2, complicatedType_t<24>{987, -1234.32456f});

   cm.addComponent(3, 3.123f);
   cm.addComponent(3, 3);
   cm.addComponent(3, complicatedType_t<24>{987, -1234.32456f});

   cm.addComponent(4, 3.123f);
   cm.addComponent(4, 3);
   cm.addComponent(4, complicatedType_t<24>{987, -1234.32456f});

   *byte_cm = cm;

   for (int i = 0; i < 4; ++i)
   {
      REQUIRE( *byte_cm->getComponent<int>(i) == 3 );
      REQUIRE( *byte_cm->getComponent<float>(i) == 3.123f );
      REQUIRE( byte_cm->getComponent<complicatedType_t<24> >(i)->int_field == 987 );
      REQUIRE( byte_cm->getComponent<complicatedType_t<24> >(i)->float_field == -1234.32456f );
   }
}

TEST_CASE( "clear empty component manager", "[ComponentManager]" )
{
   trecs::ComponentManager cman(120);
   cman.registerComponent<complicatedType_t<26> >();
   cman.registerComponent<complicatedType_t<34> >();
   cman.registerComponent<float>();
   cman.registerComponent<int>();

   REQUIRE( cman.getNumComponents<float>() == 0 );
   REQUIRE( cman.getNumComponents<int>() == 0 );
   REQUIRE( cman.getNumComponents<complicatedType_t<26> >() == 0 );
   REQUIRE( cman.getNumComponents<complicatedType_t<34> >() == 0 );

   cman.clear();

   REQUIRE( cman.getNumComponents<float>() == 0 );
   REQUIRE( cman.getNumComponents<int>() == 0 );
   REQUIRE( cman.getNumComponents<complicatedType_t<26> >() == 0 );
   REQUIRE( cman.getNumComponents<complicatedType_t<34> >() == 0 );
}

TEST_CASE( "clear partially filled component manager", "[ComponentManager]" )
{
   trecs::ComponentManager cman(120);
   cman.registerComponent<complicatedType_t<26> >();
   cman.registerComponent<complicatedType_t<34> >();
   cman.registerComponent<float>();
   cman.registerComponent<int>();

   std::vector<int> its;
   its.push_back(2);
   its.push_back(4);
   its.push_back(8);
   its.push_back(16);
   its.push_back(32);

   // Add five int types to unique entities in the component manager.
   cman.addComponent(0, its[0]);
   cman.addComponent(1, its[1]);
   cman.addComponent(2, its[2]);
   cman.addComponent(3, its[3]);
   cman.addComponent(4, its[4]);

   std::vector<float> fts;
   fts.push_back(4.5f);
   fts.push_back(-4.5f);
   fts.push_back(-4.5f);

   // Add three float types to unique entities in the component manager.
   cman.addComponent(1, fts[0]);
   cman.addComponent(3, fts[1]);
   cman.addComponent(5, fts[2]);

   std::vector<complicatedType_t<26> > cts;
   cts.push_back(complicatedType_t<26>{1, -2.f});
   cts.push_back(complicatedType_t<26>{1, 12.f});
   cts.push_back(complicatedType_t<26>{2, -4.f});
   cts.push_back(complicatedType_t<26>{3, 22.f});
   cts.push_back(complicatedType_t<26>{5, -7.3f});
   cts.push_back(complicatedType_t<26>{8, -9.f});
   cts.push_back(complicatedType_t<26>{13, -11.f});

   // Add seven complicated type <26> types to unique entities.
   cman.addComponent(33, cts[0]);
   cman.addComponent(34, cts[1]);
   cman.addComponent(35, cts[2]);
   cman.addComponent(36, cts[3]);
   cman.addComponent(37, cts[4]);
   cman.addComponent(38, cts[5]);
   cman.addComponent(40, cts[6]);

   // These have to be cached here, because after assignment none of the
   // allocator-related functionality will work in cman.
   auto num_floats = cman.getNumComponents<float>();
   auto num_ints = cman.getNumComponents<int>();
   auto num_cts = cman.getNumComponents<complicatedType_t<26> >();

   REQUIRE( cman.getNumComponents<float>() == 3);
   REQUIRE( cman.getNumComponents<int>() == 5);
   REQUIRE( cman.getNumComponents<complicatedType_t<26> >() == 7);

   cman.clear();

   REQUIRE( cman.getNumComponents<float>() == 0);
   REQUIRE( cman.getNumComponents<int>() == 0);
   REQUIRE( cman.getNumComponents<complicatedType_t<26> >() == 0);
}
