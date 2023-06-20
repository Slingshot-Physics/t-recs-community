#include "archetype.hpp"

#include "complicated_types.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <map>
#include <vector>

template <int N>
struct component
{
   int thing;
};

TEST_CASE( "archetype instantiation", "[Archetype]" )
{
   trecs::Archetype<4> archetype;
   REQUIRE( true );
}

TEST_CASE( "merge valid signatures", "[Archetype]" )
{
   trecs::Archetype<4> archetype;
   
   archetype.mergeSignature(5);

   REQUIRE( archetype.at(0) == (1 << 5) );

   archetype.mergeSignature(8);

   REQUIRE( archetype.at(0) == ((1 << 5) | (1 << 8)) );
}

TEST_CASE( "empty archetypes don't support anything", "[Archetype]" )
{
   trecs::Archetype<4> blank;
   trecs::Archetype<4> archetype;
   
   archetype.mergeSignature(5);
   archetype.mergeSignature(8);

   REQUIRE( !blank.supports(archetype) );
}

TEST_CASE( "merge invalid signatures", "[Archetype]" )
{
   trecs::Archetype<4> archetype;
   
   archetype.mergeSignature(5 * 32);

   REQUIRE( archetype.at(0) == 0 );
}

// Verifies that the archetype changes when signatures that have been merged
// into the archetype are removed later.
TEST_CASE( "remove signatures", "[Archetype]" )
{
   trecs::Archetype<4> archetype;
   
   archetype.mergeSignature(5);
   archetype.mergeSignature(8);
   archetype.mergeSignature(26);
   archetype.mergeSignature(34);
   archetype.mergeSignature(57);
   archetype.mergeSignature(63);
   archetype.mergeSignature(64);
   archetype.mergeSignature(79);
   archetype.mergeSignature(90);
   archetype.mergeSignature(123);

   REQUIRE( archetype.at(0) == ((1 << 5) | (1 << 8) | (1 << 26)) );
   REQUIRE( archetype.at(1) == ((1 << (34 - 32)) | (1 << (57 - 32)) | (1 << (63 - 32))) );
   REQUIRE( archetype.at(2) == ((1 << (64 - 64)) | (1 << (79 - 64)) | (1 << (90 - 64))) );
   REQUIRE( archetype.at(3) == (1 << (123 - 96)) );

   archetype.removeSignature(8);
   REQUIRE( archetype.at(0) == ((1 << 5) | (1 << 26)) );
   REQUIRE( archetype.at(1) == ((1 << (34 - 32)) | (1 << (57 - 32)) | (1 << (63 - 32))) );
   REQUIRE( archetype.at(2) == ((1 << (64 - 64)) | (1 << (79 - 64)) | (1 << (90 - 64))) );
   REQUIRE( archetype.at(3) == (1 << (123 - 96)) );

   archetype.removeSignature(8);
   REQUIRE( archetype.at(0) == ((1 << 5) | (1 << 26)) );
   REQUIRE( archetype.at(1) == ((1 << (34 - 32)) | (1 << (57 - 32)) | (1 << (63 - 32))) );
   REQUIRE( archetype.at(2) == ((1 << (64 - 64)) | (1 << (79 - 64)) | (1 << (90 - 64))) );
   REQUIRE( archetype.at(3) == (1 << (123 - 96)) );

   archetype.removeSignature(5);
   archetype.removeSignature(26);
   REQUIRE( archetype.at(0) == 0 );
   REQUIRE( archetype.at(1) == ((1 << (34 - 32)) | (1 << (57 - 32)) | (1 << (63 - 32))) );
   REQUIRE( archetype.at(2) == ((1 << (64 - 64)) | (1 << (79 - 64)) | (1 << (90 - 64))) );
   REQUIRE( archetype.at(3) == (1 << (123 - 96)) );

   archetype.removeSignature(123);
   archetype.removeSignature(26);
   REQUIRE( archetype.at(0) == 0 );
   REQUIRE( archetype.at(1) == ((1 << (34 - 32)) | (1 << (57 - 32)) | (1 << (63 - 32))) );
   REQUIRE( archetype.at(2) == ((1 << (64 - 64)) | (1 << (79 - 64)) | (1 << (90 - 64))) );
   REQUIRE( archetype.at(3) == 0 );
}

TEST_CASE( "check for equality", "[Archetype]" )
{
   trecs::Archetype<4> a, b;

   a.mergeSignature(16);
   b.mergeSignature(16);

   REQUIRE( (a == b) );

   a.mergeSignature(5);
   b.mergeSignature(5);

   REQUIRE( (a == b) );

   a.mergeSignature(123);
   b.mergeSignature(123);

   REQUIRE( (a == b) );

   // Test the negative case
   trecs::Archetype<4> c, d;
   c.mergeSignature(3);
   c.mergeSignature(2);
   c.mergeSignature(1);
   d.mergeSignature(3);
   d.mergeSignature(2);
   d.mergeSignature(1);
   d.mergeSignature(0);

   REQUIRE( c != d );
}

TEST_CASE( "check for inequality", "[Archetype]" )
{
   trecs::Archetype<4> a, b;

   a.mergeSignature(16);
   b.mergeSignature(7);

   REQUIRE( !(a == b) );
   REQUIRE( (a != b) );

   b.mergeSignature(5);

   REQUIRE( !(a == b) );
   REQUIRE( (a != b) );
}

TEST_CASE( "verify assignment", "[Archetype]" )
{
   trecs::Archetype<4> a, b;

   b.mergeSignature(7);
   b.mergeSignature(9);
   b.mergeSignature(123);
   b.mergeSignature(44);

   a = b;

   REQUIRE( (a == b) );

   REQUIRE( a.at(0) == ((1 << 7) | (1 << 9)) );
   REQUIRE( a.at(1) == (1 << (44 - 32)) );
   REQUIRE( a.at(3) == (1 << (123 - 32 * 3)) );

   b.mergeSignature(5);

   REQUIRE( (a != b) );
}

TEST_CASE( "verify LT", "[Archetype]" )
{
   trecs::Archetype<4> a, b;

   b.mergeSignature(7);
   b.mergeSignature(9);
   b.mergeSignature(44);
   b.mergeSignature(123);

   a.mergeSignature(7);
   a.mergeSignature(9);
   a.mergeSignature(44);

   REQUIRE( a < b );

   a.mergeSignature(123);

   REQUIRE( !(a < b) );
   REQUIRE( a == b );
}

TEST_CASE( "map of archetypes", "[Archetype]" )
{
   std::map<trecs::Archetype<4>, int> arch_counter;

   trecs::Archetype<4> a;
   a.mergeSignature(1);
   a.mergeSignature(6);
   a.mergeSignature(22);
   a.mergeSignature(38);
   a.mergeSignature(63);
   a.mergeSignature(70);
   a.mergeSignature(127);

   arch_counter[a] = 0;

   REQUIRE( arch_counter.size() == 1 );

   trecs::Archetype<4> b;
   b.mergeSignature(1);
   b.mergeSignature(6);
   b.mergeSignature(7);
   b.mergeSignature(22);
   b.mergeSignature(38);
   b.mergeSignature(63);
   b.mergeSignature(70);
   b.mergeSignature(127);

   arch_counter[b] = 1;

   REQUIRE( arch_counter.size() == 2 );

   trecs::Archetype<4> c;
   c.mergeSignature(1);
   c.mergeSignature(6);
   c.mergeSignature(38);
   c.mergeSignature(63);
   c.mergeSignature(70);

   arch_counter[c] = 2;

   REQUIRE( arch_counter.size() == 3 );

   trecs::Archetype<4> d;
   d.mergeSignature(1);
   d.mergeSignature(6);
   d.mergeSignature(22);
   d.mergeSignature(38);
   d.mergeSignature(63);
   d.mergeSignature(70);
   d.mergeSignature(127);

   arch_counter[d] = 5;

   REQUIRE( arch_counter.size() == 3 );
}

TEST_CASE( "verify signature support", "[Archetype]" )
{
   trecs::Archetype<4> a;
   a.mergeSignature(1);
   a.mergeSignature(6);
   a.mergeSignature(22);
   a.mergeSignature(38);
   a.mergeSignature(63);
   a.mergeSignature(70);
   a.mergeSignature(127);

   REQUIRE( a.supports(1) );
   REQUIRE( a.supports(6) );
   REQUIRE( a.supports(22) );
   REQUIRE( a.supports(38) );
   REQUIRE( a.supports(63) );
   REQUIRE( a.supports(70) );
   REQUIRE( a.supports(127) );

   REQUIRE( !a.supports(0) );
   REQUIRE( !a.supports(2) );
   REQUIRE( !a.supports(5) );
   REQUIRE( !a.supports(7) );
   REQUIRE( !a.supports(21) );
   REQUIRE( !a.supports(23) );
   REQUIRE( !a.supports(37) );
   REQUIRE( !a.supports(39) );
   REQUIRE( !a.supports(62) );
   REQUIRE( !a.supports(64) );
   REQUIRE( !a.supports(69) );
   REQUIRE( !a.supports(71) );
   REQUIRE( !a.supports(125) );
   REQUIRE( !a.supports(126) );
}

TEST_CASE( "verify archetype support works", "[Archetype]")
{
   trecs::Archetype<4> a, b;
   a.mergeSignature(1);
   a.mergeSignature(6);
   a.mergeSignature(22);
   a.mergeSignature(38);
   a.mergeSignature(63);
   a.mergeSignature(70);
   a.mergeSignature(127);

   b.mergeSignature(22);
   b.mergeSignature(38);
   b.mergeSignature(63);

   // Since the intersection of B and A is B, then B supports A.
   REQUIRE( b.supports(a) );

   // Since the intersection of B and A is B, then A does not support B.
   REQUIRE( !a.supports(b) );
}

TEST_CASE( "verify empty method works", "[Archetype]")
{
   trecs::Archetype<4> a;

   REQUIRE( a.empty() );

   trecs::Archetype<16> b;
   
   REQUIRE( b.empty() );
}

TEST_CASE( "verify copy constructor works", "[Archetype]")
{
   trecs::Archetype<6> a;
   a.mergeSignature(32 * 4 + 31);
   a.mergeSignature(32 * 3 + 31);
   a.mergeSignature(32 * 2 + 31);
   a.mergeSignature(32 * 1 + 31);
   a.mergeSignature(32 * 0 + 31);

   trecs::Archetype<6> b;

   // Since B is empty, B does not support A.
   REQUIRE( !b.supports(a) );

   b = a;

   REQUIRE( b.supports(a) );
   REQUIRE( (b == a) );
}
