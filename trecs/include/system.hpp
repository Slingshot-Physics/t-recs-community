#ifndef SYSTEM_HEADER
#define SYSTEM_HEADER

#include "ecs_types.hpp"

namespace trecs
{
   class Allocator;

   class System
   {
      public:
         System(void)
         { }

         virtual ~System(void)
         { }

         // Tells the allocator the component types that will be associated
         // with an entity type.
         virtual void registerComponents(trecs::Allocator & allocator) const = 0;

         // Registers an archetype query with the allocator. The allocator will
         // track the entities that satisfy this query, and the system can
         // retrieve the entities associated with this query later.
         virtual void registerQueries(trecs::Allocator & allocator) = 0;

         // Performs any non-const initialization of the system class.
         // Optionally initializes components or adds entities.
         virtual void initialize(trecs::Allocator & allocator)
         {
            (void)allocator;
         }
   };

}

#endif
