#include <stdint.h> /* for int types */

char *ltoa(long value, char *string, int32_t radix)
{
  char tmp[33];
  char *tp = tmp;
  long i;
  unsigned long v;
  int32_t sign;
  char *sp;

  if (radix > 36 || radix <= 1)
  {
    return 0;
  }

  sign = (radix == 10 && value < 0);
  if (sign)
    v = -value;
  else
    v = (unsigned long)value;
  while (v || tp == tmp)
  {
    i = v % radix;
    v = v / radix;
    if (i < 10)
      *tp++ = i+'0';
    else
      *tp++ = i + 'a' - 10;
  }

  sp = string;

  if (sign)
    *sp++ = '-';
  while (tp > tmp)
    *sp++ = *--tp;
  *sp = 0;
  return string;
}
