// A series of tests to increase feel-good-ness for the template pool allocator
// class. This tests all of the core functionality of the pool allocator.

#include "pool_allocator.hpp"

#include "complicated_types.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <vector>

typedef unsigned int uid_t;

TEST_CASE( "pool allocator interface works" )
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

TEST_CASE( "pool allocator can be instantiated")
{
   trecs::PoolAllocator<float> alloc_float(100, 8);

   trecs::PoolAllocator<complicatedType_t<0> > alloc_rb(100, 8);

   REQUIRE( alloc_rb.capacity() == 100 );

   REQUIRE( true );
}

TEST_CASE( "pool allocator accept additions")
{
   trecs::PoolAllocator<complicatedType_t<0> > alloc_rb(100, 8);

   complicatedType_t<0> component_a;

   unsigned int id1 = alloc_rb.addComponent(component_a);

   REQUIRE( alloc_rb.size() == 1 );

   unsigned int id2 = alloc_rb.addComponent(component_a);

   REQUIRE( alloc_rb.size() == 2 );
}

TEST_CASE( "pool allocator can be modified")
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

TEST_CASE( "pool allocator respects alignment")
{
   size_t byte_alignment = 32;
   trecs::PoolAllocator<complicatedType_t<0> > alloc_rb(100, byte_alignment);

   complicatedType_t<0> component_a;

   unsigned int id1 = alloc_rb.addComponent(component_a);

   REQUIRE( reinterpret_cast<size_t>(&(alloc_rb.getComponent(id1))) % byte_alignment == 0 );

   unsigned int id2 = alloc_rb.addComponent(component_a);

   REQUIRE( reinterpret_cast<size_t>(&(alloc_rb.getComponent(id2))) % byte_alignment == 0 );
}

TEST_CASE( "pool allocator size restrictions")
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
