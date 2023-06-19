#include "ext_uid_byte_pool.hpp"

#include "complicated_types.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

TEST_CASE( "pool allocator interface works", "[ExternalUidBytePool]" )
{
   std::cout << "adding new allocators\n";
   trecs::IDataPool * rb_alloc = new trecs::ExternalUidBytePool<complicatedType_t<0> >(100, 8);
   std::cout << "added an allocator for the complicated type\n";
   trecs::IDataPool * mesh_alloc = new trecs::ExternalUidBytePool<complicatedType_t<5> >(100, 8);
   std::cout << "added an allocator for another complicated type\n";

   REQUIRE(rb_alloc->size() == 0);
   REQUIRE(mesh_alloc->size() == 0);

   complicatedType_t<0> component_a;
   trecs::ExternalUidBytePool<complicatedType_t<0> > * rb_alloc_dc = static_cast< trecs::ExternalUidBytePool<complicatedType_t<0> > * >(rb_alloc);
   rb_alloc_dc->addComponent(15, component_a);

   REQUIRE(rb_alloc->size() == 1);

   delete rb_alloc;
   delete mesh_alloc;
}

TEST_CASE( "pool allocator can be instantiated", "[ExternalUidBytePool]" )
{
   trecs::ExternalUidBytePool<float> alloc_float(100, 8);

   trecs::ExternalUidBytePool<complicatedType_t<0> > alloc_rb(100, 8);

   REQUIRE( alloc_rb.capacity() == 100 );

   REQUIRE( true );
}

TEST_CASE( "pool allocator accept additions", "[ExternalUidBytePool]" )
{
   trecs::ExternalUidBytePool<complicatedType_t<0> > alloc_rb(100, 8);

   complicatedType_t<0> component_a;

   unsigned int id1 = 1;
   int ret = alloc_rb.addComponent(id1, component_a);

   REQUIRE( alloc_rb.size() == 1 );
   REQUIRE( ret == (int )id1 );

   unsigned int id2 = 2;
   ret = alloc_rb.addComponent(id2, component_a);

   REQUIRE( alloc_rb.size() == 2 );
   REQUIRE( ret == (int )id2 );
}

TEST_CASE( "pool allocator can be modified", "[ExternalUidBytePool]" )
{
   trecs::ExternalUidBytePool<complicatedType_t<0> > alloc_rb(100, 1);

   complicatedType_t<0> component_a;

   unsigned int id1 = 1;
   int ret = alloc_rb.addComponent(id1, component_a);

   REQUIRE( alloc_rb.size() == 1 );
   REQUIRE( ret == (int )id1 );

   unsigned int id2 = 2;
   ret = alloc_rb.addComponent(id2, component_a);

   REQUIRE( alloc_rb.size() == 2 );
   REQUIRE( ret == (int )id2 );

   alloc_rb.removeComponent(id1);

   REQUIRE( alloc_rb.size() == 1 );

   alloc_rb.removeComponent(id1);

   REQUIRE( alloc_rb.size() == 1 );

   alloc_rb.removeComponent(id2);

   REQUIRE( alloc_rb.size() == 0 );
}

TEST_CASE( "pool allocator respects alignment", "[ExternalUidBytePool]" )
{
   size_t byte_alignment = 32;
   trecs::ExternalUidBytePool<complicatedType_t<0> > alloc_rb(100, byte_alignment);

   complicatedType_t<0> component_a;

   unsigned int id1 = 1;
   int ret = alloc_rb.addComponent(id1, component_a);

   // Convert the memory address of the component to a size_t, then make sure
   // its memory address lines up with the desired alignment.
   REQUIRE( reinterpret_cast<size_t>((alloc_rb.getComponent(id1))) % byte_alignment == 0 );
   REQUIRE( ret == (int )id1);

   unsigned int id2 = 2;
   ret = alloc_rb.addComponent(id2, component_a);

   REQUIRE( reinterpret_cast<size_t>((alloc_rb.getComponent(id2))) % byte_alignment == 0 );
   REQUIRE( ret == (int )id2);
}

TEST_CASE( "pool allocator size restrictions", "[ExternalUidBytePool]" )
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::ExternalUidBytePool<complicatedType_t<0> > alloc_rb(max_size, byte_alignment);

   complicatedType_t<0> component_a;

   for (unsigned int i = 0; i < 2 * max_size; ++i)
   {
      int ret = alloc_rb.addComponent(i, component_a);
      REQUIRE( alloc_rb.size() <= max_size );

      if (i >= max_size)
      {
         REQUIRE( ret == -1 );
      }
      else
      {
         REQUIRE( ret == (int )i );
      }
   }
}

TEST_CASE( "pool allocator veracity", "[ExternalUidBytePool]" )
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::ExternalUidBytePool<bigType_t> alloc_rb(max_size, byte_alignment);

   bigType_t temp_big_type;

   // Verify that bodies can be added to the pool allocator.
   std::vector<bigType_t> big_types;
   for (unsigned int i = 0; i < max_size; ++i)
   {
      temp_big_type.float_field = 2.125f * static_cast<float>(i);
      for (int i = 0; i < 8; ++i)
      {
         temp_big_type.float_arr[i] = -1.f * static_cast<float>(i);
         temp_big_type.stuff.things[i] = i - 4;
      }
      temp_big_type.int_field = alloc_rb.addComponent(i, temp_big_type);
      alloc_rb.getComponent(i)->int_field = temp_big_type.int_field;
      big_types.push_back(temp_big_type);
      REQUIRE( alloc_rb.size() <= max_size );
      REQUIRE( temp_big_type.int_field == i );
   }

   // Verify that data can be retrieved in with the same level of veracity as
   // when it was added to the pool WHILE every third record is deleted.
   std::vector<unsigned int> deleted_uids;
   for (unsigned int i = 0; i < big_types.size(); ++i)
   {
      unsigned int big_type_index = (i > 49) * (149 - i) + (i <= 49) * i;
      REQUIRE( alloc_rb.getComponent(big_types[big_type_index].int_field)->float_field == big_types[big_type_index].float_field);
      if ((i % 3) == 0)
      {
         std::cout << "deleting component UID: " << big_types[big_type_index].int_field << "\n";
         alloc_rb.removeComponent(big_types[big_type_index].int_field);
         deleted_uids.push_back(big_types[big_type_index].int_field);
      }
   }

   // Verify that components that were deleted are actually deleted.
   for (unsigned int i = 0; i < deleted_uids.size(); ++i)
   {
      std::vector<trecs::uid_t> uids = alloc_rb.getUids();
      bool deleted_uid_doesnt_exist = std::find(
         uids.begin(), uids.end(), deleted_uids[i]
      ) == uids.end();
      if (!deleted_uid_doesnt_exist)
      {
         std::cout << "found uid: " << deleted_uids[i] << "\n";
      }
      REQUIRE(deleted_uid_doesnt_exist);
   }

   // Verify that components that weren't deleted are still accessible.
   for (unsigned int i = 0; i < big_types.size(); ++i)
   {
      unsigned int big_type_index = i;
      if (std::find(deleted_uids.begin(), deleted_uids.end(), big_types[i].int_field) != deleted_uids.end())
      {
         continue;
      }
      REQUIRE( alloc_rb.getComponent(big_types[big_type_index].int_field)->float_field == big_types[big_type_index].float_field);
   }

   // Delete everything from the allocator.
   for (unsigned int i = 0; i < big_types.size(); ++i)
   {
      // unsigned int big_type_index = i;
      if (std::find(deleted_uids.begin(), deleted_uids.end(), big_types[i].int_field) != deleted_uids.end())
      {
         continue;
      }
      alloc_rb.removeComponent(big_types[i].int_field);
   }

   REQUIRE(alloc_rb.size() == 0);

   // Put it all back in the allocator.
   for (unsigned int i = 0; i < big_types.size(); ++i)
   {
      unsigned int new_bigboi_id = alloc_rb.addComponent(i, big_types[i]);
      alloc_rb.getComponent(new_bigboi_id)->int_field = new_bigboi_id;
      big_types[i].int_field = new_bigboi_id;
   }

   REQUIRE( alloc_rb.size() == big_types.size() );

   // Verify that the data chunk is all correct.
   for (unsigned int i = 0; i < big_types.size(); ++i)
   {
      unsigned int big_type_index = i;
      REQUIRE( alloc_rb.getComponent(big_types[big_type_index].int_field)->float_field == big_types[big_type_index].float_field);
   }

   // Delete it all again.
   for (unsigned int i = 0; i < big_types.size(); ++i)
   {
      alloc_rb.removeComponent(big_types[i].int_field);
   }

   REQUIRE( alloc_rb.size() == 0 );
}

TEST_CASE( "assignment operator on from empty to non-empty allocator", "[ExternalUidBytePool]" )
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::ExternalUidBytePool<bigType_t> alloc(max_size, byte_alignment);

   trecs::ExternalUidBytePool<bigType_t> alloc_b(max_size, byte_alignment);

   bigType_t temp_big_type;

   alloc_b.addComponent(1, temp_big_type);

   REQUIRE( alloc_b.size() == 1 );

   alloc_b = alloc;

   REQUIRE( alloc_b.size() == alloc.size() );
}

TEST_CASE(
   "assignment operator after only additions to empty allocator",
   "[ExternalUidBytePool]"
)
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::ExternalUidBytePool<bigType_t> alloc(max_size, byte_alignment);

   bigType_t temp_big_type;

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 100; ++i)
   {
      entities.push_back(i);
   }

   for (const auto entity : entities)
   {
      temp_big_type.int_field = entity;
      temp_big_type.float_field = static_cast<float>(entity) * -3.f;
      for (int i = 0; i < 18; ++i)
      {
         temp_big_type.stuff.things[i] = entity + i;
      }

      alloc.addComponent(entity, temp_big_type);
   }

   trecs::ExternalUidBytePool<bigType_t> alloc_b(max_size, byte_alignment);

   alloc_b = alloc;

   for (const auto entity : entities)
   {
      REQUIRE( alloc.getComponent(entity) != nullptr );
      bigType_t var_a = *alloc.getComponent(entity);

      REQUIRE( alloc_b.getComponent(entity) != nullptr );
      bigType_t var_b = *alloc_b.getComponent(entity);

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
   "[ExternalUidBytePool]"
)
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::ExternalUidBytePool<bigType_t> alloc(max_size, byte_alignment);

   bigType_t temp_big_type;

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 100; ++i)
   {
      entities.push_back(i);
   }

   for (const auto entity : entities)
   {
      temp_big_type.int_field = entity;
      temp_big_type.float_field = static_cast<float>(entity) * -3.f;
      for (int i = 0; i < 18; ++i)
      {
         temp_big_type.stuff.things[i] = entity + i;
      }

      alloc.addComponent(entity, temp_big_type);
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

   trecs::ExternalUidBytePool<bigType_t> alloc_b(max_size, byte_alignment);

   alloc_b = alloc;

   for (const auto entity : entities)
   {
      REQUIRE( alloc.getComponent(entity) != nullptr );
      bigType_t var_a = *alloc.getComponent(entity);

      REQUIRE( alloc_b.getComponent(entity) != nullptr );
      bigType_t var_b = *alloc_b.getComponent(entity);

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
   "[ExternalUidBytePool]"
)
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::ExternalUidBytePool<bigType_t> alloc(max_size, byte_alignment);

   bigType_t temp_big_type;

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 100; ++i)
   {
      entities.push_back(i);
   }

   for (const auto entity : entities)
   {
      temp_big_type.int_field = entity;
      temp_big_type.float_field = static_cast<float>(entity) * -3.f;
      for (int i = 0; i < 18; ++i)
      {
         temp_big_type.stuff.things[i] = entity + i;
      }

      alloc.addComponent(entity, temp_big_type);
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

   trecs::ExternalUidBytePool<bigType_t> alloc_b(max_size, byte_alignment);

   for (int i = 0; i < 16; ++i)
   {
      temp_big_type.int_field = -2 * i;
      alloc_b.addComponent(2 * i, temp_big_type);
   }

   alloc_b = alloc;

   for (const auto entity : entities)
   {
      REQUIRE( alloc.getComponent(entity) != nullptr );
      bigType_t var_a = *alloc.getComponent(entity);

      REQUIRE( alloc_b.getComponent(entity) != nullptr );
      bigType_t var_b = *alloc_b.getComponent(entity);

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
   "[ExternalUidBytePool]"
)
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::ExternalUidBytePool<bigType_t> alloc(max_size, byte_alignment);

   bigType_t temp_big_type;

   std::vector<trecs::uid_t> entities;

   for (int i = 0; i < 100; ++i)
   {
      entities.push_back(i);
   }

   for (const auto entity : entities)
   {
      temp_big_type.int_field = entity;
      temp_big_type.float_field = static_cast<float>(entity) * -3.f;
      for (int i = 0; i < 18; ++i)
      {
         temp_big_type.stuff.things[i] = entity + i;
      }

      alloc.addComponent(entity, temp_big_type);
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

   trecs::ExternalUidBytePool<bigType_t> alloc_b(max_size, byte_alignment);

   for (int i = 0; i < 16; ++i)
   {
      temp_big_type.int_field = -2 * i;
      alloc_b.addComponent(2 * i, temp_big_type);
   }

   // alloc_b = alloc;

   trecs::IDataPool & alloc_b_base = alloc_b;

   alloc_b_base = alloc;

   for (const auto entity : entities)
   {
      REQUIRE( alloc.getComponent(entity) != nullptr );
      bigType_t var_a = *alloc.getComponent(entity);

      REQUIRE( alloc_b.getComponent(entity) != nullptr );
      bigType_t var_b = *alloc_b.getComponent(entity);

      REQUIRE( var_a.int_field == var_b.int_field );
      REQUIRE( var_a.float_field == var_b.float_field );

      for (int i = 0; i < 18; ++i)
      {
         REQUIRE( var_a.stuff.things[i] == var_b.stuff.things[i] );
      }
   }
}

TEST_CASE( "zero-out deleted bytes", "[ExternalUidBytePool]" )
{
   size_t max_size = 100;
   size_t byte_alignment = 32;
   trecs::ExternalUidBytePool<byteChecker_t> pooler(max_size, byte_alignment);

   byteChecker_t temp;
   temp.is_dirty = false;
   temp.values[0] = -12.f;
   temp.values[1] = 15.123f;
   temp.values[2] = -7.5f;

   pooler.addComponent(0, temp);
   pooler.addComponent(1, temp);
   pooler.addComponent(2, temp);

   byteChecker_t * weird_copy = pooler.getComponent(2);

   for (int i = 0; i < 3; ++i)
   {
      std::cout << weird_copy->values[i] << "\n";
   }

   bool not_all_bytes_zero = false;
   for (int i = 0; i < 12; ++i)
   {
      if (weird_copy->bytes[i] != 0)
      {
         not_all_bytes_zero = true;
         break;
      }
   }

   REQUIRE( not_all_bytes_zero );

   pooler.removeComponent(1);

   for (int i = 0; i < 12; ++i)
   {
      REQUIRE( weird_copy->bytes[i] == 0 );
   }
}
