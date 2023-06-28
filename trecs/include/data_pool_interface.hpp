#ifndef DATA_POOL_INTERFACE_HEADER
#define DATA_POOL_INTERFACE_HEADER

#include "ecs_types.hpp"

#include <cstring>

namespace trecs
{

   class IDataPool
   {
      public:
         // Virtualized so that child classes' destructors can be called.
         virtual ~IDataPool(void) { };

         virtual IDataPool & operator=(const IDataPool & other) = 0;

         virtual void removeComponent(uid_t uid) = 0;

         virtual size_t capacity(void) const = 0;

         virtual size_t size(void) const = 0;

         virtual void clear(void) = 0;
   };

}

#endif
