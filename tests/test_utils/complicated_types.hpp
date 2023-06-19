#ifndef COMPLICATED_TYPES_TESTS_UTILS_HEADER
#define COMPLICATED_TYPES_TESTS_UTILS_HEADER

template <unsigned int N>
struct complicatedType_t
{
   int int_field;
   float float_field;
};

typedef struct internalType_s
{
   int things[18];
} internalType_t;

typedef struct bigType_s
{
   int int_field;
   float float_field;
   float float_arr[12];
   internalType_t stuff;
} bigType_t;

#endif
