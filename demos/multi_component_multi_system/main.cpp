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

      void initialize(trecs::Allocator & allocator)
      {
         std::vector<trecs::uid_t> point_mass_entities;
         // Add a bunch of entities with pos, vel, accel components.
         for (int i = 0; i < 100; ++i)
         {
            auto new_entity = allocator.addEntity();
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
      {

      }
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
            auto body_a_entity = force_edges[force_entity]->nodeIdA;
            auto body_b_entity = force_edges[force_entity]->nodeIdB;

            float distance = sqrtf(
               powf(positions[body_a_entity]->vec[0] - positions[body_b_entity]->vec[0], 2.f) +
               powf(positions[body_a_entity]->vec[1] - positions[body_b_entity]->vec[1], 2.f) +
               powf(positions[body_a_entity]->vec[2] - positions[body_b_entity]->vec[2], 2.f)
            );

            for (int i = 0; i < 3; ++i)
            {
               float f_b_on_a = forces[force_entity]->k * (
                  (positions[body_a_entity]->vec[i] - positions[body_b_entity]->vec[i]) / distance
               ) + forces[force_entity]->c * (
                  velocities[body_a_entity]->vec[i] - velocities[body_b_entity]->vec[i]
               );

               accelerations[body_a_entity]->vec[i] = f_b_on_a;
               accelerations[body_b_entity]->vec[i] = -1.f * f_b_on_a;
            }
         }
      }

   private:
      trecs::query_t force_query_;

      trecs::query_t point_mass_query_;
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
            for (int i = 0; i < 3; ++i)
            {
               velocities[entity]->vec[i] += accelerations[entity]->vec[i] * dt_;
            }
         }

         for (const auto entity : point_mass_entities)
         {
            for (int i = 0; i < 3; ++i)
            {
               positions[entity]->vec[i] += velocities[entity]->vec[i] * dt_;
            }
         }
      }

   private:
      trecs::query_t point_mass_query_;

      const float dt_ = 0.001f;
};

int main(void)
{
   trecs::Allocator allocator(128);
   auto registerer = allocator.registerSystem<Registerer>();
   auto integrator = allocator.registerSystem<Integrator>();
   auto force_calculator = allocator.registerSystem<ForceCalculator>();

   allocator.initializeSystems();

   std::cout << "initialized\n";

   for (int i = 0; i < 1000000; ++i)
   {
      force_calculator->update(allocator);
      integrator->update(allocator);
      if ((i % 1000) == 0)
      {
         std::cout << "step " << (i / 1000) << "\n";
      }
   }

   return 0;
}
