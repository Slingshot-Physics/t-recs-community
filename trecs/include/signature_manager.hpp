#ifndef SIGNATURE_MANAGER_HEADER
#define SIGNATURE_MANAGER_HEADER

#include "ecs_types.hpp"

#include <iostream>
#include <cstdint>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

namespace trecs
{
   template <typename Signature_T>
   class SignatureManager
   {
      public:
         SignatureManager(void)
            : signature_counter_(0)
         { }

         SignatureManager & operator=(
            const SignatureManager<Signature_T> & other
         )
         {
            if (this == &other)
            {
               return *this;
            }

            signature_counter_ = other.signature_counter_;
            id_to_signature_.clear();
            id_to_signature_ = other.id_to_signature_;
            // id_to_signature_.insert(other.id_to_signature_.begin(), other.id_to_signature_.end());
            // for (const auto & other_id_to_sig : other.id_to_signature_)
            // {
            //    id_to_signature_[other_id_to_sig.first] = other_id_to_sig.second;
            //    // id_to_signature_.insert()
            // }

            return *this;
         }

         // Registers the component's type and assigns it a signature.
         // Returns the component type's signature.
         template <typename Component_T>
         Signature_T registerComponent(void)
         {
            std::type_index type_id = getTypeId<Component_T>();

            return registerTypeId(type_id);
         }

         // Retrieves a signature for a particular component type if the
         // component has been registered. Returns an error signature if the
         // component type has not been registered.
         
         template <typename Component_T>
         Signature_T getSignature(void) const
         {
            std::type_index type_id = getTypeId<Component_T>();

            return getSignature(type_id);
         }

         // Retrieves a signature for a particular component type regardless of
         // whether or not the component was properly registered. This will
         // crash if the signature of an unregistered component is requested.
         template <typename Component_T>
         Signature_T getSignatureUnsafe(void) const
         {
            std::type_index type_id = getTypeId<Component_T>();

            return id_to_signature_.at(type_id);
         }

         size_t size(void) const
         {
            return signature_counter_;
         }

      private:
         Signature_T signature_counter_;

         std::unordered_map<std::type_index, Signature_T> id_to_signature_;

         Signature_T registerTypeId(const std::type_index type_id)
         {
            if (signature_counter_ >= max_num_signatures)
            {
               std::cout << "Too many signatures in use! Can't register component " << type_id.name() << "\n";
               return error_signature;
            }

            const auto id_to_sig_iter = id_to_signature_.find(type_id);

            if (id_to_sig_iter != id_to_signature_.end())
            {
               // std::cout << "Component type " << type_id.name() << " has already been registered\n";
               return id_to_sig_iter->second;
            }

            const Signature_T new_signature = signature_counter_;
            id_to_signature_[type_id] = new_signature;
            ++signature_counter_;

            return new_signature;
         }

         Signature_T getSignature(const std::type_index type_id) const
         {
            const auto id_sig_iter = id_to_signature_.find(type_id);
            if (id_sig_iter == id_to_signature_.end())
            {
               std::cout << "Couldn't find component type " << type_id.name() << " in the id->signature map\n";
               return error_signature;
            }

            return id_sig_iter->second;
         }

         template <typename Component_T>
         std::type_index getTypeId(void) const
         {
            return std::type_index(typeid(Component_T));
         }
   };

   typedef SignatureManager<signature_t> DefaultSignatureManager;
}

#endif
