#ifndef ECS_TYPES_HEADER
#define ECS_TYPES_HEADER

#include <cstdint>

namespace trecs
{
   typedef int64_t uid_t;

   typedef uint8_t signature_t;

   typedef uint64_t archetype_t;

   typedef uint32_t query_t;

   const auto error_query = __UINT32_MAX__;

   const auto max_num_signatures = 8 * sizeof(archetype_t) - 1;

   const auto error_signature = 8 * sizeof(archetype_t) - 1;

   typedef enum edge_flag
   {
      TRANSITIVE = 0,
      NODE_A_TERMINAL = 1,
      NODE_B_TERMINAL = 2,
      NULL_EDGE = 3
   } edge_flag_enum_t;

   typedef struct edge_s
   {
      uid_t edgeId;
      uid_t nodeIdA;
      uid_t nodeIdB;
      edge_flag_enum_t flag;
   } edge_t;

}

#endif
