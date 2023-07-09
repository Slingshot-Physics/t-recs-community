#include "allocator.hpp"

#include <cmath>
#include <iostream>

typedef struct vec3
{
   vec3(void)
   {
      for (int i = 0; i < 3; ++i)
      {
         v[i] = 0.f;
      }
   }

   vec3(float a, float b, float c)
      : v{a, b, c}
   { }

   float & operator[](const int i)
   {
      return v[i];
   }

   const float & operator[](const int i) const
   {
      return v[i];
   }

   float v[3];
} vec3;

template <unsigned int JankyName>
struct named_vec3
{
   named_vec3(void)
   {
      for (int i = 0; i < 3; ++i)
      {
         vec[i] = 0.f;
      }
   }

   named_vec3(float a, float b, float c)
      : vec(a, b, c)
   { }

   vec3 vec;

   float & operator[](const int i)
   {
      return vec[i];
   }

   const float & operator[](const int i) const
   {
      return vec[i];
   }
};

typedef named_vec3<0> pos_t;
typedef named_vec3<1> vel_t;
typedef named_vec3<2> acc_t;

typedef struct spring_damper
{
   // Should be less than zero
   float k;

   // Should be less than zero
   float c;
} spring_damper_t;

typedef struct lennard_jones
{
   float k;
} lennard_jones_t;

class Registerer : public trecs::System
{
   public:
      void registerComponents(trecs::Allocator & allocator) const override
      {
         allocator.registerComponent<pos_t>();
         allocator.registerComponent<vel_t>();
         allocator.registerComponent<acc_t>();
         allocator.registerComponent<spring_damper_t>();
      }

      void registerQueries(trecs::Allocator & allocator) override
      {

      }

      void initialize(trecs::Allocator & allocator) override
      {
         std::vector<trecs::uid_t> point_mass_entities;
         // Add a bunch of entities with pos, vel, accel components.
         for (int i = 0; i < 100; ++i)
         {
            auto new_entity = allocator.addEntity();
            std::cout << "Got new entity: " << new_entity << "\n";
            allocator.addComponent(new_entity, pos_t{static_cast<float>(3.f * i), 0.f, 0.f});
            allocator.addComponent(new_entity, vel_t{0.f, 0.f, (((i % 2) == 0) ? 1.f : -1.f) * 10.f});
            allocator.addComponent(new_entity, acc_t{});

            point_mass_entities.push_back(new_entity);
         }

         for (int i = 0; i < 50; ++i)
         {
            auto new_force_entity = allocator.addEntity(
               point_mass_entities[2 * i],
               point_mass_entities[2 * i + 1]
            );
            allocator.addComponent(new_force_entity, spring_damper_t{-2.5f, -10.f});
         }
      }

      void update(trecs::Allocator & allocator)
      { }
};

class ForceCalculator : public trecs::System
{
   public:
      void registerComponents(trecs::Allocator & allocator) const override
      {
         allocator.registerComponent<pos_t>();
         allocator.registerComponent<vel_t>();
         allocator.registerComponent<acc_t>();
      }

      void registerQueries(trecs::Allocator & allocator) override
      {
         force_query_ = allocator.addArchetypeQuery<spring_damper_t>();
         point_mass_query_ = allocator.addArchetypeQuery<pos_t, vel_t, acc_t>();
      }

      void update(trecs::Allocator & allocator)
      {
         const auto & point_mass_entities = allocator.getQueryEntities(point_mass_query_);
         const auto & force_entities = allocator.getQueryEntities(force_query_);

         auto force_edges = allocator.getComponents<trecs::edge_t>();
         auto forces = allocator.getComponents<spring_damper_t>();
         auto positions = allocator.getComponents<pos_t>();
         auto velocities = allocator.getComponents<vel_t>();
         auto accelerations = allocator.getComponents<acc_t>();

         for (const auto & force_entity : force_entities)
         {
            spring_damper_t & force = *forces[force_entity];
            auto body_a_entity = force_edges[force_entity]->nodeIdA;
            auto body_b_entity = force_edges[force_entity]->nodeIdB;

            pos_t & pos_a = *positions[body_a_entity];
            pos_t & pos_b = *positions[body_b_entity];

            float distance = sqrtf(
               powf(pos_a.vec[0] - pos_b.vec[0], 2.f) +
               powf(pos_a.vec[1] - pos_b.vec[1], 2.f) +
               powf(pos_a.vec[2] - pos_b.vec[2], 2.f)
            );

            vel_t & vel_a = *velocities[body_a_entity];
            vel_t & vel_b = *velocities[body_b_entity];

            acc_t & accel_a = *accelerations[body_a_entity];
            acc_t & accel_b = *accelerations[body_b_entity];

            for (int i = 0; i < 3; ++i)
            {
               float f_b_on_a = force.k * (
                  (pos_a.vec[i] - pos_b.vec[i]) / distance
               ) + force.c * (
                  vel_a.vec[i] - vel_b.vec[i]
               );

               accel_a.vec[i] = f_b_on_a;
               accel_b.vec[i] = -1.f * f_b_on_a;
            }
         }
      }

   private:
      trecs::query_t force_query_;

      trecs::query_t point_mass_query_;
};

class ConditionalForceCalculator : public trecs::System
{
   public:
      void registerComponents(trecs::Allocator & allocator) const override
      {
         
      }

      void registerQueries(trecs::Allocator & allocator) override
      {
         ecb_entity_ = allocator.addEntityComponentBuffer<lennard_jones_t, trecs::edge_t>(100);
         point_mass_query_ = allocator.addArchetypeQuery<pos_t, vel_t, acc_t>();
      }

      void update(trecs::Allocator & allocator)
      {
         auto ecb = allocator.getEntityComponentBuffer(ecb_entity_);
         ecb->clear();

         auto positions = allocator.getComponents<pos_t>();

         const auto & point_mass_entities = allocator.getQueryEntities(point_mass_query_);

         for (
            auto pm_iter_a = point_mass_entities.begin();
            pm_iter_a != point_mass_entities.end();
            ++pm_iter_a
         )
         {
            const auto & pos_a = *positions[*pm_iter_a];
            for (
               auto pm_iter_b = pm_iter_a;
               pm_iter_b != point_mass_entities.end();
               ++pm_iter_b
            )
            {
               if (pm_iter_a == pm_iter_b)
               {
                  continue;
               }

               const auto & pos_b = *positions[*pm_iter_b];
               float distance = sqrtf(
                  powf(pos_a.vec[0] - pos_b.vec[0], 2.f) +
                  powf(pos_a.vec[1] - pos_b.vec[1], 2.f) +
                  powf(pos_a.vec[2] - pos_b.vec[2], 2.f)
               );

               if (distance < 0.5f)
               {
                  trecs::uid_t new_entity = ecb->addEntity();
                  if (new_entity < 0)
                  {
                     std::cout << "ECB said I have an invalid entity\n";
                  }
                  trecs::edge_t new_edge;
                  new_edge.nodeIdA = *pm_iter_a;
                  new_edge.nodeIdB = *pm_iter_b;
                  new_edge.flag = trecs::TRANSITIVE;
                  ecb->updateComponent(new_entity, new_edge);

                  lennard_jones_t lj;
                  lj.k = powf(0.1f / distance, 12) - powf(0.1f / distance, 6);
                  ecb->updateComponent(new_entity, lj);
               }
            }
         }
      }

   private:
      trecs::uid_t ecb_entity_;

      trecs::query_t point_mass_query_;
};

class ConditionalForceApplier : public trecs::System
{
   public:
      void registerComponents(trecs::Allocator & allocator) const override
      {
         
      }

      void registerQueries(trecs::Allocator & allocator) override
      {
         ecb_query_ = allocator.addArchetypeQuery<trecs::EntityComponentBuffer>();
         point_mass_query_ = allocator.addArchetypeQuery<pos_t, vel_t, acc_t>();
      }

      void initialize(trecs::Allocator & allocator) override
      {
         const auto ecb_entities = allocator.getQueryEntities(ecb_query_);
         for (const auto ecb_entity : ecb_entities)
         {
            if (
               allocator.getEntityComponentBuffer(
                  ecb_entity
               )->supportsComponents<lennard_jones_t>()
            )
            {
               ecb_entity_ = ecb_entity;
               break;
            }
         }
      }

      void update(trecs::Allocator & allocator)
      {
         auto ecb = allocator.getEntityComponentBuffer(ecb_entity_);

         const auto lj_edge_components = ecb->getComponents<trecs::edge_t>();
         const auto lj_components = ecb->getComponents<lennard_jones_t>();

         auto accelerations = allocator.getComponents<acc_t>();

         const auto & point_mass_entities = allocator.getQueryEntities(point_mass_query_);

         for (trecs::uid_t lj_entity = 0; lj_entity < ecb->numEntities(); ++lj_entity)
         {
            auto accel_a = accelerations[lj_edge_components[lj_entity]->nodeIdA];
            auto accel_b = accelerations[lj_edge_components[lj_entity]->nodeIdB];

            for (int i = 0; i < 3; ++i)
            {
               (*accel_a)[i] += lj_components[lj_entity]->k;
               (*accel_b)[i] -= lj_components[lj_entity]->k;
            }
         }
      }

   private:
      trecs::uid_t ecb_entity_;

      trecs::query_t point_mass_query_;

      trecs::query_t ecb_query_;
};

class Integrator : public trecs::System
{
   public:
      void registerComponents(trecs::Allocator & allocator) const override
      {
         allocator.registerComponent<pos_t>();
         allocator.registerComponent<vel_t>();
         allocator.registerComponent<acc_t>();
      }

      void registerQueries(trecs::Allocator & allocator) override
      {
         point_mass_query_ = allocator.addArchetypeQuery<pos_t, vel_t, acc_t>();
      }

      void update(trecs::Allocator & allocator)
      {
         const auto & point_mass_entities = allocator.getQueryEntities(point_mass_query_);
         auto positions = allocator.getComponents<pos_t>();
         auto velocities = allocator.getComponents<vel_t>();
         auto accelerations = allocator.getComponents<acc_t>();

         for (const auto entity : point_mass_entities)
         {
            vel_t & vel = *velocities[entity];
            acc_t & acc = *accelerations[entity];
            for (int i = 0; i < 3; ++i)
            {
               vel.vec[i] += acc.vec[i] * dt_;
            }
         }

         for (const auto entity : point_mass_entities)
         {
            pos_t & pos = *positions[entity];
            vel_t & vel = *velocities[entity];
            for (int i = 0; i < 3; ++i)
            {
               pos.vec[i] += vel.vec[i] * dt_;
            }
         }
      }

   private:
      trecs::query_t point_mass_query_;

      const float dt_ = 0.001f;
};

int main(void)
{
   trecs::Allocator allocator(256);
   auto registerer = allocator.registerSystem<Registerer>();
   auto integrator = allocator.registerSystem<Integrator>();
   auto conditional_force_calculator = allocator.registerSystem<ConditionalForceCalculator>();
   auto conditional_force_applier = allocator.registerSystem<ConditionalForceApplier>();
   auto force_calculator = allocator.registerSystem<ForceCalculator>();

   allocator.initializeSystems();

   std::cout << "initialized\n";

   trecs::uid_t ecb_entity = -1;
   trecs::query_t ecb_query = allocator.addArchetypeQuery<trecs::EntityComponentBuffer>();
   const auto ecb_entities = allocator.getQueryEntities(ecb_query);
   for (const auto temp_ecb_entity : ecb_entities)
   {
      if (
         allocator.getEntityComponentBuffer(
            temp_ecb_entity
         )->supportsComponents<lennard_jones_t>()
      )
      {
         ecb_entity = temp_ecb_entity;
         break;
      }
   }

   const auto ecb = allocator.getEntityComponentBuffer(ecb_entity);

   for (int i = 0; i < 100000; ++i)
   {
      force_calculator->update(allocator);
      conditional_force_calculator->update(allocator);
      conditional_force_applier->update(allocator);
      integrator->update(allocator);
      if ((i % 1000) == 0)
      {
         std::cout << "step " << (i / 1000) << "\n";
         std::cout << "\tnum ecb entities: " << ecb->numEntities() << "\n";
      }
   }

   return 0;
}
