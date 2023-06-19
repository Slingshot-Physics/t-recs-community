#include "system_manager.hpp"

#include "allocator.hpp"

namespace trecs
{

   SystemManager::~SystemManager(void)
   {
      for (auto & system : systems_)
      {
         if (system.second != nullptr)
         {
            delete system.second;
            system.second = nullptr;
         }
      }
   }

   void SystemManager::registerComponents(trecs::Allocator & allocator)
   {
      for (auto & system : systems_)
      {
         system.second->registerComponents(allocator);
      }
   }

   void SystemManager::registerQueries(trecs::Allocator & allocator)
   {
      for (auto & system : systems_)
      {
         system.second->registerQueries(allocator);
      }
   }

   void SystemManager::initializeSystems(trecs::Allocator & allocator)
   {
      for (auto & system : systems_)
      {
         system.second->initialize(allocator);
      }
   }

}
