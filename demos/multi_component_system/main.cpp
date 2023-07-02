#include "allocator.hpp"

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

class Registerer : public trecs::System
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

      }

      void initialize(trecs::Allocator & allocator)
      {
         // Add a bunch of entities with pos, vel, accel components.
         for (int i = 0; i < 100; ++i)
         {
            auto new_entity = allocator.addEntity();
            allocator.addComponent(new_entity, pos_t{});
            allocator.addComponent(new_entity, vel_t{});
            allocator.addComponent(new_entity, acc_t{0.f, 0.f, -9.8f});
         }
      }

      void update(trecs::Allocator & allocator)
      {

      }
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
   trecs::Allocator allocator(128);
   auto registerer = allocator.registerSystem<Registerer>();
   auto integrator = allocator.registerSystem<Integrator>();

   allocator.initializeSystems();

   std::cout << "initialized\n";

   for (int i = 0; i < 1000000; ++i)
   {
      integrator->update(allocator);
      if ((i % 1000) == 0)
      {
         std::cout << "step " << (i / 1000) << "\n";
      }
   }

   return 0;
}
