#include "system_manager.hpp"
#include "blank_systems.hpp"

#include "complicated_types.hpp"

#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include <vector>

TEST_CASE("instantiation", "[ecs-mssm]")
{
   trecs::SystemManager sys_manager;
   REQUIRE( true );
}

TEST_CASE("add random system", "[ecs-mssm]")
{
   trecs::SystemManager sys_manager;

   SandoSystem * rs = sys_manager.registerSystem<SandoSystem>();

   REQUIRE( rs != nullptr );
}

// Verifies that a single system type can only be registered with the system
// manager once.
TEST_CASE( "can't add same system twice", "[ecs-mssm]" )
{
   trecs::SystemManager sys_manager;

   SandoSystem * rs = sys_manager.registerSystem<SandoSystem>();

   SandoSystem * rs2 = sys_manager.registerSystem<SandoSystem>();

   REQUIRE( rs != nullptr );

   REQUIRE( rs2 == nullptr );
}

// Verifies that two systems can be registered in the system manager.
TEST_CASE( "register two systems", "[ecs-mssm]" )
{
   trecs::SystemManager sys_manager;

   SandoSystem * rs = sys_manager.registerSystem<SandoSystem>();

   NandoSystem * ms = sys_manager.registerSystem<NandoSystem>();

   REQUIRE( rs != nullptr );
   REQUIRE( ms != nullptr );
}
