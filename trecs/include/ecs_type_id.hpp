#ifndef ECS_TYPE_ID_HEADER
#define ECS_TYPE_ID_HEADER

#include <typeindex>

namespace trecs
{
   template <typename Component_T>
   std::type_index getTypeId(void)
   {
      return std::type_index(typeid(Component_T));
   }
}


#endif
