#ifndef BLANK_SYSTEMS_TEST_UTIL_HEADER
#define BLANK_SYSTEMS_TEST_UTIL_HEADER

#include "system.hpp"

// A basic system implementation.
class SandoSystem : public trecs::System
{
   public:
      virtual ~SandoSystem(void)
      { }

      void registerComponents(trecs::Allocator & allocator) const
      { }

      void registerQueries(trecs::Allocator & allocator)
      { }
};

// A basic system implementation with an unused const member.
class NandoSystem : public trecs::System
{
   public:
      NandoSystem(void)
         : thing_(12)
      { }

      void registerComponents(trecs::Allocator & allocator) const
      { }

      void registerQueries(trecs::Allocator & allocator)
      { }

      virtual ~NandoSystem(void)
      { }

   private:
      const int thing_;
};

#endif
