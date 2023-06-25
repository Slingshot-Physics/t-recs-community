// A series of tests to increase feel-good-ness for the template pool allocator
// class. This tests all of the core functionality of the pool allocator.

#include "pool_allocator.hpp"

#include "complicated_types.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <vector>

TEST_CASE( "pool allocator interface works", "[PoolAllocator]" )
{
   trecs::PoolAllocatorInterface * rb_alloc = new trecs::PoolAllocator<complicatedType_t<0> >(100, 8);
   trecs::PoolAllocatorInterface * mesh_alloc = new trecs::PoolAllocator<bigType_t>(100, 8);

   REQUIRE(rb_alloc->size() == 0);
   REQUIRE(mesh_alloc->size() == 0);

   complicatedType_t<0> component_a;
   trecs::PoolAllocator<complicatedType_t<0> > * rb_alloc_dc = static_cast< trecs::PoolAllocator<complicatedType_t<0> > * >(rb_alloc);
   rb_alloc_dc->addComponent(component_a);

   REQUIRE(rb_alloc->size() == 1);

   delete rb_alloc;
   delete mesh_alloc;
}

TEST_CASE( "pool allocator can be instantiated", "[PoolAllocator]" )
{
   trecs::PoolAllocator<float> alloc_float(100, 8);

   trecs::PoolAllocator<complicatedType_t<0> > alloc_rb(100, 8);

   REQUIRE( alloc_rb.capacity() == 100 );

   REQUIRE( true );
}

TEST_CASE( "pool allocator accept additions", "[PoolAllocator]" )
{
   trecs::PoolAllocator<complicatedType_t<0> > alloc_rb(100, 8);

   complicatedType_t<0> component_a;

   unsigned int id1 = alloc_rb.addComponent(component_a);

   REQUIRE( alloc_rb.size() == 1 );

   unsigned int id2 = alloc_rb.addComponent(component_a);

   REQUIRE( alloc_rb.size() == 2 );
}

TEST_CASE( "pool allocator can be modified", "[PoolAllocator]" )
{
   trecs::PoolAllocator<complicatedType_t<0> > alloc_rb(100, 1);

   complicatedType_t<0> component_a;

   unsigned int id1 = alloc_rb.addComponent(component_a);

   REQUIRE( alloc_rb.size() == 1 );

   unsigned int id2 = alloc_rb.addComponent(component_a);

   REQUIRE( alloc_rb.size() == 2 );

   alloc_rb.removeComponent(id1);

   REQUIRE( alloc_rb.size() == 1 );

   alloc_rb.removeComponent(id1);

   REQUIRE( alloc_rb.size() == 1 );

   alloc_rb.removeComponent(id2);

   REQUIRE( alloc_rb.size() == 0 );
}

TEST_CASE( "pool allocator respects alignment", "[PoolAllocator]" )
{
   size_t byte_alignment = 32;
   trecs::PoolAllocator<complicatedType_t<0> > alloc_rb(100, byte_alignment);

   complicatedType_t<0> component_a;

   unsigned int id1 = alloc_rb.addComponent(component_a);

   REQUIRE( reinterpret_cast<size_t>(&(alloc_rb.getComponent(id1))) % byte_alignment == 0 );

   unsigned int id2 = alloc_rb.addComponent(component_a);

   REQUIRE( reinterpret_cast<size_t>(&(alloc_rb.getComponent(id2))) % byte_alignment == 0 );
}

TEST_CASE( "pool allocator size restrictions", "[PoolAllocator]" )
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::PoolAllocator<complicatedType_t<0> > alloc_rb(max_size, byte_alignment);

   complicatedType_t<0> component_a;

   for (unsigned int i = 0; i < 2 * max_size; ++i)
   {
      alloc_rb.addComponent(component_a);
      REQUIRE( alloc_rb.size() <= max_size );
   }
}

TEST_CASE( "assignment operator on from empty to non-empty allocator", "[PoolAllocator]" )
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::PoolAllocator<bigType_t> alloc(max_size, byte_alignment);

   trecs::PoolAllocator<bigType_t> alloc_b(max_size, byte_alignment);

   bigType_t temp_big_type;

   std::vector<trecs::uid_t> entities;

   alloc_b.addComponent(temp_big_type);

   REQUIRE( alloc_b.size() == 1 );

   alloc_b = alloc;

   REQUIRE( alloc_b.size() == alloc.size() );
}

TEST_CASE(
   "assignment operator after only additions to empty allocator",
   "[PoolAllocator]"
)
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::PoolAllocator<bigType_t> alloc(max_size, byte_alignment);

   bigType_t temp_big_type;

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 100; ++i)
   {
      temp_big_type.int_field = i;
      temp_big_type.float_field = static_cast<float>(i) * -3.f;
      for (int i = 0; i < 18; ++i)
      {
         temp_big_type.stuff.things[i] = 2 * i;
      }

      entities.push_back(alloc.addComponent(temp_big_type));
   }

   trecs::PoolAllocator<bigType_t> alloc_b(max_size, byte_alignment);

   alloc_b = alloc;

   for (const auto entity : entities)
   {
      bigType_t var_a = alloc.getComponent(entity);

      bigType_t var_b = alloc_b.getComponent(entity);

      REQUIRE( var_a.int_field == var_b.int_field );
      REQUIRE( var_a.float_field == var_b.float_field );

      for (int i = 0; i < 18; ++i)
      {
         REQUIRE( var_a.stuff.things[i] == var_b.stuff.things[i] );
      }
   }
}

TEST_CASE(
   "assignment operator after additions and deletions to empty allocator",
   "[PoolAllocator]"
)
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::PoolAllocator<bigType_t> alloc(max_size, byte_alignment);

   bigType_t temp_big_type;

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 100; ++i)
   {
      temp_big_type.int_field = i;
      temp_big_type.float_field = static_cast<float>(i) * -3.f;
      for (int i = 0; i < 18; ++i)
      {
         temp_big_type.stuff.things[i] = 2 * i;
      }

      entities.push_back(alloc.addComponent(temp_big_type));
   }

   std::vector<trecs::uid_t> entities_copy(entities);

   for (int i = 0; i < 100; ++i)
   {
      if (i % 5 == 0)
      {
         alloc.removeComponent(entities_copy[i]);
         entities.erase(
            std::find(entities.begin(), entities.end(), entities_copy[i])
         );
      }
   }

   trecs::PoolAllocator<bigType_t> alloc_b(max_size, byte_alignment);

   alloc_b = alloc;

   std::cout << "UID original:\n";
   for (const auto uid : alloc.getUids())
   {
      std::cout << uid << ", ";
   }
   std::cout << "\n";

   std::cout << "UID B:\n";
   for (const auto uid_b : alloc_b.getUids())
   {
      std::cout << uid_b << ", ";
   }
   std::cout << "\n";

   for (const auto entity : entities)
   {
      bigType_t var_a = alloc.getComponent(entity);

      bigType_t var_b = alloc_b.getComponent(entity);

      REQUIRE( var_a.int_field == var_b.int_field );
      REQUIRE( var_a.float_field == var_b.float_field );

      for (int i = 0; i < 18; ++i)
      {
         REQUIRE( var_a.stuff.things[i] == var_b.stuff.things[i] );
      }
   }
}

TEST_CASE(
   "assignment operator after additions and deletions to non-empty allocator",
   "[PoolAllocator]"
)
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::PoolAllocator<bigType_t> alloc(max_size, byte_alignment);

   bigType_t temp_big_type;

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 100; ++i)
   {
      temp_big_type.int_field = i;
      temp_big_type.float_field = static_cast<float>(i) * -3.f;
      for (int i = 0; i < 18; ++i)
      {
         temp_big_type.stuff.things[i] = 2 * i;
      }

      entities.push_back(alloc.addComponent(temp_big_type));
   }

   std::vector<trecs::uid_t> entities_copy(entities);

   for (int i = 0; i < 100; ++i)
   {
      if (i % 5 == 0)
      {
         alloc.removeComponent(entities_copy[i]);
         entities.erase(
            std::find(entities.begin(), entities.end(), entities_copy[i])
         );
      }
   }

   trecs::PoolAllocator<bigType_t> alloc_b(max_size, byte_alignment);

   for (int i = 0; i < 16; ++i)
   {
      temp_big_type.int_field = -2 * i;
      alloc_b.addComponent(temp_big_type);
   }

   alloc_b = alloc;

   for (const auto entity : entities)
   {
      bigType_t var_a = alloc.getComponent(entity);

      bigType_t var_b = alloc_b.getComponent(entity);

      REQUIRE( var_a.int_field == var_b.int_field );
      REQUIRE( var_a.float_field == var_b.float_field );

      for (int i = 0; i < 18; ++i)
      {
         REQUIRE( var_a.stuff.things[i] == var_b.stuff.things[i] );
      }
   }
}

TEST_CASE(
   "assignment operator after additions and deletions to non-empty allocator via base class",
   "[PoolAllocator]"
)
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::PoolAllocator<bigType_t> alloc(max_size, byte_alignment);

   bigType_t temp_big_type;

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 100; ++i)
   {
      temp_big_type.int_field = i;
      temp_big_type.float_field = static_cast<float>(i) * -3.f;
      for (int i = 0; i < 18; ++i)
      {
         temp_big_type.stuff.things[i] = 2 * i;
      }

      entities.push_back(alloc.addComponent(temp_big_type));
   }

   std::vector<trecs::uid_t> entities_copy(entities);

   for (int i = 0; i < 100; ++i)
   {
      if (i % 5 == 0)
      {
         alloc.removeComponent(entities_copy[i]);
         entities.erase(
            std::find(entities.begin(), entities.end(), entities_copy[i])
         );
      }
   }

   trecs::PoolAllocator<bigType_t> alloc_b(max_size, byte_alignment);

   for (int i = 0; i < 16; ++i)
   {
      temp_big_type.int_field = -2 * i;
      alloc_b.addComponent(temp_big_type);
   }

   trecs::PoolAllocatorInterface & alloc_b_base = alloc_b;

   alloc_b_base = alloc;

   for (const auto entity : entities)
   {
      bigType_t var_a = alloc.getComponent(entity);

      bigType_t var_b = alloc_b.getComponent(entity);

      REQUIRE( var_a.int_field == var_b.int_field );
      REQUIRE( var_a.float_field == var_b.float_field );

      for (int i = 0; i < 18; ++i)
      {
         REQUIRE( var_a.stuff.things[i] == var_b.stuff.things[i] );
      }
   }
}
