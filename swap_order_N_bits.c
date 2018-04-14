#include <stdio.h>

unsigned int swap_order_N_bits(unsigned int Hex_data,unsigned int N_bits)
{
  unsigned int temp = 0,i;
for(i=0;i<N_bits;i++)
    if ((Hex_data >> i) & 1) 
       temp |= 1 << (N_bits - 1 - i);
    return temp;}

int main()
{
unsigned int x=0xa1;
printf("x=%02X\n",x);
printf("x=%02X\n",swap_order_N_bits(x,8));

}
