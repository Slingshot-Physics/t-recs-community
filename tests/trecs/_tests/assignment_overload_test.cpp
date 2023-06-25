#include <iostream>

#include <vector>

struct base
{
   virtual base & operator=(const base & other) = 0;
   virtual void print(void) const = 0;
};

template <typename T>
struct derived : public base
{
   derived<T> & operator=(const derived<T> & other)
   {
      if (this == &other)
      {
         std::cout << "assign to derived self?\n";
         return *this;
      }

      things.clear();
      for (const auto & thing : other.things)
      {
         things.push_back(thing);
      }

      return *this;
   }

   base & operator=(const base & other) override
   {
      if (this == &other)
      {
         std::cout << "assign to base self?\n";
         return *this;
      }

      const base * other_p = &other;
      const derived<T> * derived_other_p = dynamic_cast<const derived<T> *>(other_p);

      if (derived_other_p == nullptr)
      {
         std::cout << "couldn't downcast anything\n";
         return *this;
      }

      things.clear();
      for (const auto & thing : derived_other_p->things)
      {
         things.push_back(thing);
      }

      return *this;
   }

   void print(void) const override
   {
      for (const auto thing : things)
      {
         std::cout << "\t" << thing << "\n";
      }
   }

   std::vector<T> things;
};

typedef struct ptr_wrapper
{
   ptr_wrapper(void)
   {
      for (int i = 0; i < 8; ++i)
      {
         bytes[i] = 0;
      }
   }

   union
   {
      void * ptr;
      unsigned char bytes[8];
   };
} ptr_wrapper;

int main(void)
{
   derived<int> derived_int;
   for (int i = 0; i < 10; ++i)
   {
      derived_int.things.push_back(i);
   }

   derived<double> derived_double;

   derived<int> second_int;
   base & abs_second = second_int;

   abs_second = derived_int;

   abs_second.print();

   abs_second = derived_double;

   abs_second.print();

   derived_int = derived_int;

   abs_second = abs_second;

   std::cout << sizeof(int *) << "\n";

   ptr_wrapper p;

   std::cout << (p.ptr == nullptr) << "\n";

   std::cout << "pointer: " << p.ptr << "\n";;

   std::cout << "bytes:\n";
   for (int i = 0; i < 8; ++i)
   {
      std::cout << "\t" << static_cast<int>(p.bytes[i]) << "\n";
   }

   return 0;
}
