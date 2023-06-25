#include <iostream>
#include <memory>

void change_ownership(std::unique_ptr<int> & thing_a, const std::unique_ptr<int> & thing_b)
{
   thing_a.reset(thing_b.get());
}

int main(void)
{
   std::unique_ptr<int> int_ptr(nullptr);
   // int_ptr = new int(69);
   int_ptr.reset(new int(69));
   std::cout << "pointed to: " << *int_ptr << "\n";

   std::unique_ptr<int> other_int_ptr;
   // other_int_ptr = int_ptr;
   other_int_ptr.reset(int_ptr.release());

   std::cout << "active? " << (int_ptr ? true : false) << "\n";

   std::cout << "pointed to: " << *other_int_ptr << "\n";

   //------

   std::unique_ptr<int> my_thing(new int);
   *my_thing = 77;

   std::unique_ptr<int> my_other_thing(new int);
   *my_other_thing = -420;

   change_ownership(my_thing, my_other_thing);
   my_other_thing.release();

   return 0;
}
