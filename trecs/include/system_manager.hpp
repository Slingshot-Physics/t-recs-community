#ifndef SYSTEM_MANAGER_HEADER
#define SYSTEM_MANAGER_HEADER

#include "system.hpp"

#include "ecs_types.hpp"

#include <iostream>
#include <map>
#include <typeinfo>

namespace trecs
{
   class Allocator;

   class SystemManager
   {
      public:
         ~SystemManager(void);

         template <typename System_T>
         System_T * registerSystem(void)
         {
            auto system_iter = getSystemIterator<System_T>();

            if (system_iter != systems_.end())
            {
               std::cout << "Couldn't register system with type " << typeid(System_T).name() << " because it's already registered\n";
               return nullptr;
            }

            size_t system_type = typeid(System_T).hash_code();
            System_T * derived_system = new System_T;
            systems_[system_type] = derived_system;

            return derived_system;
         }

         void registerComponents(trecs::Allocator & allocator);

         void registerQueries(trecs::Allocator & allocator);

         void initializeSystems(trecs::Allocator & allocator);

      private:
         std::map<size_t, System *> systems_;

         template <typename System_T>
         std::map<size_t, System *>::iterator getSystemIterator(void)
         {
            size_t system_type = typeid(System_T).hash_code();

            auto system_iter = systems_.find(system_type);

            return system_iter;
         }
   };
}

#endif
