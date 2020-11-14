#include <stdlib.h>

#include "mutators.h"

void mutator_rand_byte(unsigned char *input, int length)
{
  for (int i = 0; i < length * .5 ; ++i)
    input[rand() % length] = (unsigned char)(rand() % 256);
}
