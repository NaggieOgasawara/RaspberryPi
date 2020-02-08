/************************************************************************/
/*      GCC/LLCM ARM Baremetal Software / (C) Toshio MURAI  2012-2016   */
/*      This is Free Software.   You can redistribute this software     */
/*      and/or modify it ,under the terms of the GNU General Public     */
/*      License version 3, or (at your option) any later version.       */
/*      or under the terms of the BSD 3-Clause License.	(Dual Licence)  */
/************************************************************************/
#ifdef	SELF
#include	<stdio.h>
#define	tfp_printf	printf
#else
void tfp_printf(char *fmt, ...);
#endif

#define M	250		// 251 250
#define N	210		// 209 225

typedef unsigned short	int16;	
typedef unsigned short	uint16;
typedef unsigned int	uint32;
uint16 a[N+1], b[N+1], c[N+1]; 

//	c[] = a[] + b[]
int add(uint16 a[], uint16 b[], uint16 c[])
{
	int i, r;

	for (i = N, r = 0; i >= 0; i--) {
		r += a[i] + b[i];
		c[i] = r & 0xffff; r >>= 16;
	}
	return r;
}

//	c[] = a[] -b[]
int sub(uint16 a[], uint16 b[], uint16 c[])
{
	int i, r;

	for (i = N, r = 0; i >= 0; i--) {
		r = a[i] - b[i] - r;
		c[i] = r & 0xffff;
		r = (r >> 16) & 1;
	}
	return r;
}

//	c[] = a[] * b
int mul(uint16 a[], int16 b, uint16 c[])
{
	int i;
	uint32 r;

	for (i = N, r= 0; i >= 0; i--) {
		r += (uint32) a[i] * (uint32)b;
		c[i] = r & 0xffff; r >>= 16;
	}
	return r;
}

//	c[] = a[] / b
int div(uint16 a[], int b, uint16 c[])
{
	int i;
	uint32 r;
	int z = 0;

	for (i = 0, r = 0; i <= N; i++) {
		r = (r << 16) + a[i];
//		z |= (c[i] = r / b); r %= b;
		if(r) {z |= (c[i] = r / b); r %= b;}
		else {c[i]=0;}
	}
	if (2 * r >= b)	{
		for (i = N; c[i] == 0 ; i--){ c[i] ++ ; }
		z = 1;
	}
	return z;
}

void print(uint16 a[])
{
	int i;

	tfp_printf("%5u.", a[0]);
	for (i = 0; i < M; i++) {
		a[0] = 0;
		mul(a, 10000, a);
		tfp_printf("%04u", a[0]);
	}
	tfp_printf("\n");
}

//
//  e = 1 + 1/1! + 1/2! + 1/3! + .... + 1/n! .......
//

void e(void) 
{
	int k;

	for (k = 0; k <= M; k++) {a[k] = b[k] = 0;}	// a = b = 0
	a[0] = 1; b[0] =1;				// a = 1.0 , b = 1.0
	for (k = 1; k< 0x10000 ; k++) {
		if(!div(b, k, b))			// b = b / k
			break;
		add(a, b, a);				// a = a + b
	}
	print(a);
}

// PI = 16 arctan(1/5) - 4 arctan(1/239)
// arctan(x) = x -x^3/3 + x^5/5 -x^7/7 + x^9/9 - ... -x^n/n + x^(n+1)/(n+1)
//
void pi(void)	// PI Machin Formula
{
	int i;

	// 16 * arctan (1/5)
	b[0] = 16; for (i = 1; i <= N; i++) {b[i] = 0;}	// b = 16
	div(b, 5, b);					// b = b / 5
	for (i = 0; i <= N; i++) {a[i] = b[i];}		// a = b

	for (i = 3 ; i < 0x10000 ;i +=2 ) {
		if (!div(b, 5*5, b))
			break;	// b = b / 25
		if (!div(b, i, c) )
			break;				// c = b / k
		if (i & 0x02) {
			sub(a, c, a);			// a = a - c
		} else {
			add(a, c, a);			// a = a + c
		}
	}

	// 4 * arctan (1/239)
	b[0] = 4; for (i = 1; i <= N; i++) {b[i] = 0;}	// b = 4
	div(b, 239, b);					// b = b / 239
	sub(a, b, a);					// a = a - b

	for (i =3 ; i < 0x10000; i += 2 ) {
		if (!div(b, 239*239, b))
			break;				// b = b / 239 *239
		if (!div(b, i, c))
			 break;				// c = b / k
		if (i & 0x02) {
			add(a, c, a) ;			// a = a + c
		} else {
			sub(a, c, a);			// a = a - c
		}
	}
	print(a);
}


void pie()
{
		pi();
	 	e();
}

#ifdef SELF
int main ()
{
	int i;
	for(i= 0 ; i <100; i++) {
		pie();
	}
}
#endif
