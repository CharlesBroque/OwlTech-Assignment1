#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// provided debug macro
# define DEBUG(msg, val) printf("[DEBUG] %s = %u\n", msg, val)

void div_convert(uint32_t n, int base, char *out) {
   // beginning with provided skeleton
   char temp[65]; // buffer
   int pos = 0;

   // zero case
   if (n == 0) {
      strcpy(out, "0");
      return;
   }

   // collect digits in reverse order
   while (n > 0) {
      int remainder = n % base;
      n = n / base;

      // digit to character
      if (remainder < 10)
         temp[pos++] = '0' + remainder;
      else
         temp[pos++] = 'A' + (remainder - 10);
   }
   temp[pos] = '\0'; // saves a lot of headaches later

   // reverse the result (build string)
   // actually since we're using <string.h> anyway
   // i think strrev(temp) would have worked
   // whatever
   char result[33]; // empty string without terminator
   result[pos] = '\0'; // terminate string
   int pos2 = 0;
   while (pos > 0) {
      pos--;
      result[pos2] = temp[pos];
      pos2++;
   }
   strcpy(out, result);
}

void sub_convert(uint32_t n, int base, char *out) {
   // "find highest power, subtract multiples"
   
   if (n == 0) {
    strcpy(out, "0");
    return;
   }

   // populate powers table
   uint32_t p = 1;
   /* EXPLANATION
   *  this weird hack with the 1e-13 (see below) prevents a precision error.
   *  it's easy to see the problem. (int)(log(1000)/log(10)) = 2 instead of 3 like you would expect.
   *  the maximum error this introduces to recovering the argument is of 10^-4 order
   *  within the uint32_t limit for bases 2-16 and is still of just 10^-3 order for bases up to 256.
   *  the trick may be extendable with a smaller constant, e.g. 1e-14, but i am worried about
   *  pathological behavior near the precision limit.
   */
   double max_power = (log(4294967295) + 1e-13)/log(base);
   uint32_t powers[32]; // max uint32_t is 2 ** 32 - 1
   for (int i = 0; i < max_power; i++) {
      powers[i] = p;
      p *= base;
   }

   // algo time
   int pos = 0; // track position in number
   int pow = (int)((log(n) + 1e-13)/log(base)); // opposite of position, kind of. will decrement to 0
   int bigpower = pow; // the largest power less than n
   int subcount = 0; // counts multiples subtracted
   char result[33]; // will store result
   // prefill result with 0 (except terminator)
   for (int i = 0; i < 33; i++) result[i] = '0';
   result[33] = '\0'; // just in case
   while (n != 0) {
      // find biggest power less than n
      bigpower = (int)((log(n) + 1e-13)/log(base));
      // if we would do no subtractions, skip ahead
      if (bigpower < pow) {
         pos += (pow - bigpower);
         pow = bigpower;
      }
      // repeatedly subtract biggest power
      subcount = 0;
      while (n >= powers[pow]) {
         n -= powers[pow];
         subcount++;
      }
      // digit to character
      if (subcount < 10)
         result[pos++] = '0' + subcount;
      else
         result[pos++] = 'A' + (subcount - 10);
      // decrement pow
      pow--;
   }
   // is pow still bigger than 0? add some place value padding
   while (pow >= 0) {
      result[pos++] = '0';
      pow--;
   }
   result[pos] = '\0'; // terminate result string
   strcpy(out, result);
}

void print_tables(uint32_t n) {
   /*
   # Expected format for each operation (3 lines total):
   # Original: Binary=<binary> Octal=<octal> Decimal=<decimal> Hex=<hex>
   # Left Shift by 3: Binary=<binary> Octal=<octal> Decimal=<decimal> Hex=<hex>
   # AND with 0xFF: Binary=<binary> Octal=<octal> Decimal=<decimal> Hex=<hex>
   */

   // there's no *out. i guess we're doing printf().
   uint32_t original = n;
   uint32_t lshift3 = n << 3;
   uint32_t and0xFF = n & 0xFF;
   char* mybin = malloc(32+1);
   char* myoct = malloc(32+1);
   char* mydec = malloc(32+1);
   char* myhex = malloc(32+1);
   // i suspect div_convert is faster.
   div_convert(original, 2, mybin); div_convert(original, 8, myoct); div_convert(original, 10, mydec); div_convert(original, 16, myhex);
   printf("Original: Binary=%s Octal=%s Decimal=%s Hex=%s\n", mybin, myoct, mydec, myhex);
   div_convert(lshift3, 2, mybin); div_convert(lshift3, 8, myoct); div_convert(lshift3, 10, mydec); div_convert(lshift3, 16, myhex);
   printf("Left Shift by 3: Binary=%s Octal=%s Decimal=%s Hex=%s\n", mybin, myoct, mydec, myhex);
   div_convert(and0xFF, 2, mybin); div_convert(and0xFF, 8, myoct); div_convert(and0xFF, 10, mydec); div_convert(and0xFF, 16, myhex);
   printf("AND with 0xFF: Binary=%s Octal=%s Decimal=%s Hex=%s", mybin, myoct, mydec, myhex);
}