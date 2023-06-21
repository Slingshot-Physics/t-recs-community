#include "allocator.hpp"

namespace trecs
{

   Allocator::Allocator(void)
      : max_num_entities_(20000)
      , entities_(max_num_entities_)
      , components_(max_num_entities_)
   {
      registerComponent<trecs::edge_t>();
      edge_query_ = addArchetypeQuery<trecs::edge_t>();
   }

   Allocator::Allocator(unsigned int max_num_entities)
      : max_num_entities_(max_num_entities)
      , entities_(max_num_entities_)
      , components_(max_num_entities_)
   {
      registerComponent<trecs::edge_t>();
      edge_query_ = addArchetypeQuery<trecs::edge_t>();
   }

   void Allocator::clear(void)
   {
      entities_.clear();
      components_.clear();
   }

   uid_t Allocator::addEntity(void)
   {
      return entities_.addEntity();
   }

   uid_t Allocator::addEntity(uid_t node_entity_a, uid_t node_entity_b)
   {
      uid_t entity = entities_.addEntity();
      edge_t edge;
      edge.edgeId = entity;
      edge.nodeIdA = node_entity_a;
      edge.nodeIdB = node_entity_b;
      edge.flag = trecs::edge_flag_enum_t::TRANSITIVE;
      addComponent(entity, edge);
      return entity;
   }

   uid_t Allocator::addEntity(uid_t node_entity)
   {
      uid_t entity = entities_.addEntity();
      edge_t edge;
      edge.edgeId = entity;
      edge.nodeIdA = -1;
      edge.nodeIdB = node_entity;
      edge.flag = trecs::edge_flag_enum_t::NODE_A_TERMINAL;
      addComponent(entity, edge);
      return entity;
   }

   const std::vector<uid_t> & Allocator::getEntities(void) const
   {
      return entities_.getUids();
   }

   void Allocator::removeEntity(uid_t entity_uid)
   {
      entities_.removeEntity(entity_uid);
      components_.removeComponents(entity_uid);
      removeNodeEntityFromEdge(entity_uid);
      queries_.removeEntity(entity_uid);
   }

   edge_t Allocator::getEdge(uid_t edge_entity_uid)
   {
      const trecs::edge_t * edge = \
         components_.getComponent<trecs::edge_t>(edge_entity_uid);

      trecs::edge_t temp_edge;
      if (edge == nullptr)
      {
         temp_edge.edgeId = -1;
         edge = &temp_edge;
      }

      return *edge;
   }

   edge_t Allocator::updateEdge(
      uid_t entity,
      uid_t node_entity_a,
      uid_t node_entity_b
   )
   {
      edge_t edge = getEdge(entity);
      if (edge.edgeId == -1)
      {
         return edge;
      }

      edge.nodeIdA = node_entity_a;
      edge.nodeIdB = node_entity_b;
      edge.flag = trecs::edge_flag_enum_t::TRANSITIVE;

      updateComponent(entity, edge);

      return edge;
   }

   edge_t Allocator::updateEdge(
      uid_t entity, uid_t node_entity
   )
   {
      edge_t edge = getEdge(entity);
      if (edge.edgeId == -1)
      {
         return edge;
      }

      edge.nodeIdA = -1;
      edge.nodeIdB = node_entity;
      edge.flag = trecs::edge_flag_enum_t::NODE_A_TERMINAL;

      updateComponent(entity, edge);

      return edge;
   }

   void Allocator::removeNodeEntityFromEdge(uid_t entity_uid)
   {
      auto edges = getComponents<trecs::edge_t>();
      const auto & edge_entities = getQueryEntities(edge_query_);

      for (auto & entity : edge_entities)
      {
         trecs::edge_t * temp_edge = edges[entity];
         if (temp_edge == nullptr)
         {
            continue;
         }

         if (temp_edge->nodeIdA == entity_uid)
         {
            temp_edge->nodeIdA = -1;
         }
         if (temp_edge->nodeIdB == entity_uid)
         {
            temp_edge->nodeIdB = -1;
         }

         if (temp_edge->nodeIdA == -1 && temp_edge->nodeIdB == -1)
         {
            temp_edge->flag = trecs::edge_flag_enum_t::NULL_EDGE;
         }
         else if (temp_edge->nodeIdA == -1 && temp_edge->nodeIdB > -1)
         {
            temp_edge->flag = trecs::edge_flag_enum_t::NODE_A_TERMINAL;
         }
         else if (temp_edge->nodeIdA > -1 && temp_edge->nodeIdB == -1)
         {
            temp_edge->flag = trecs::edge_flag_enum_t::NODE_B_TERMINAL;
         }
      }
   }

   void Allocator::initializeSystems(void)
   {
      systems_.registerComponents(*this);
      systems_.registerQueries(*this);
      systems_.initializeSystems(*this);
   }

}
