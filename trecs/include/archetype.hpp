#ifndef ARCHETYPE_HEADER
#define ARCHETYPE_HEADER

#include "ecs_types.hpp"

#include <cstdint>
#include <iostream>

namespace trecs
{
   typedef struct block_archetype_s
   {
      uint32_t block;
      uint32_t bits;
   } block_archetype_t;

   template <int BlockCount>
   class Archetype
   {
      public:
         Archetype(void)
         {
            for (int i = 0; i < BlockCount; ++i)
            {
               archetypes_[i] = 0;
            }
         }

         Archetype(const Archetype<BlockCount> & other)
         {
            for (int i = 0; i < BlockCount; ++i)
            {
               archetypes_[i] = other.archetypes_[i];
            }
         }

         ~Archetype(void) = default;

         void reset(void)
         {
            for (int i = 0; i < BlockCount; ++i)
            {
               archetypes_[i] = 0;
            }
         }

         void mergeSignature(const signature_t sig)
         {
            block_archetype_t arch = signatureToArchetype(sig);
            archetypes_[arch.block] |= arch.bits;
         }

         void removeSignature(const signature_t sig)
         {
            block_archetype_t arch = signatureToArchetype(sig);
            archetypes_[arch.block] &= (~arch.bits);
         }

         bool supports(const signature_t sig) const
         {
            block_archetype_t arch = signatureToArchetype(sig);
            return (archetypes_[arch.block] & arch.bits) == arch.bits;
         }

         bool supports(const Archetype<BlockCount> & arch) const
         {
            if (empty())
            {
               return false;
            }

            for (int i = 0; i < BlockCount; ++i)
            {
               if ((archetypes_[i] & arch.archetypes_[i]) != archetypes_[i])
               {
                  return false;
               }
            }

            return true;
         }

         // Returns true if the archetype has zero associated component
         // signatures. Returns false otherwise.
         bool empty(void) const
         {
            for (int i = 0; i < BlockCount; ++i)
            {
               if (archetypes_[i] != 0)
               {
                  return false;
               }
            }

            return true;
         }

         void print(void) const
         {
            std::cout << "Archetype:\n";
            for (int i = 0; i < BlockCount; ++i)
            {
               std::cout << "\t" << archetypes_[i] << "\n";
            }
         }

         bool operator==(const Archetype<BlockCount> & other) const
         {
            for (int i = 0; i < BlockCount; ++i)
            {
               if (archetypes_[i] != other.archetypes_[i])
               {
                  return false;
               }
            }

            return true;
         }

         bool operator!=(const Archetype<BlockCount> & other) const
         {
            return !(*this == other);
         }

         Archetype & operator=(const Archetype<BlockCount> & other)
         {
            for (int i = 0; i < BlockCount; ++i)
            {
               archetypes_[i] = other.archetypes_[i];
            }

            return *this;
         }

         bool operator<(const Archetype<BlockCount> & other) const
         {
            for (int i = BlockCount - 1; i > -1; --i)
            {
               if (archetypes_[i] < other.archetypes_[i])
               {
                  return true;
               }
               else if (archetypes_[i] > other.archetypes_[i])
               {
                  return false;
               }
            }

            return false;
         }

         uint32_t at(const int32_t i) const
         {
            return archetypes_[i];
         }

      private:

         uint32_t archetypes_[BlockCount];

         block_archetype_t signatureToArchetype(const signature_t sig) const
         {
            block_archetype_t arch;
            if (sig / 32 >= BlockCount)
            {
               std::cout << "Signature " << sig << " exceeds maximum signature!\n";
               arch.block = 0;
               arch.bits = 0;
               return arch;
            }

            arch.block = sig / 32;
            arch.bits = 1 << (sig % 32);

            return arch;
         }

   };

   typedef Archetype<4> DefaultArchetype;
}

#endif
