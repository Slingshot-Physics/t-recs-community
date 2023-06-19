#include <iostream>
#include <typeinfo>
#include <cstdint>
#include <type_traits>

#include <vector>

void varifunc(int & i)
{
   i += 0;
}

template <class First, class...Types>
void varifunc(int & i, First f, Types...args)
{
   i += 1;
   std::cout << "type: " << typeid(First).name() << "\n";
   varifunc(i, args...);
}

typedef struct some_base
{
   int stuff;
} some_base;

template <typename T>
struct some_derived : some_base
{
   T thing;
};

typedef struct termination_s
{

} termination_t;

// template <class First, class...Types>
typedef struct thing_collector
{
   thing_collector()
   {

   }

   template <class T>
   void initialize2(void)
   {
      containers.push_back(new some_derived<T>);
   }

   template <class First, class...Types>
   typename std::enable_if<sizeof...(Types) != 0, bool>::type initialize2(void)
   {
      containers.push_back(new some_derived<First>);
      initialize2<Types...>();
      return true;
   }

   std::vector<some_base*> containers;
} thing_collector;

typedef struct anon_s
{
   int a;
   int b;
   int c;
} anon_t;

template <typename Label_T>
struct silly_t
{
   float a[5];
   int bee[9];
   Label_T label;
};

template <unsigned int Size>
struct array_t
{
   int arr[Size];
};

template <unsigned int N, typename A, typename...Types>
void stuff(void)
{
   // int arr[N];
   array_t<N> arr;

   for (int i = 0; i < N; ++i)
   {
      arr.arr[i] = 2 * i;
   }

   thing_collector tc;
   tc.initialize2<A, Types...>();

   std::cout << "num weird containers:" << tc.containers.size() << "\n";
}

int main(void)
{
   int i = 0;
   varifunc(i, float{}, int{}, 1, 2.0, 'a', anon_t{}, silly_t<uint8_t>{});

   std::cout << "i: " << i << "\n"; // prints i: 5

   thing_collector c;
   c.initialize2<int, float, anon_t, silly_t<uint8_t> >();

   std::cout << "num containers: " << c.containers.size() << "\n";

   stuff<5, int, float, anon_t, silly_t<uint8_t> >();

   return 0;
}
