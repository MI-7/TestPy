#include "python.h"

/* the upper and lower bounday of small int cache
range = -5 to 256 */
#define NSMALLINTMIN 5
#define NSMALLINTMAX 257

/* size of each free block when they are initialized */
#define BLOCKSIZE 1000

/* each block has a pointer, pointing to the next block */
#define BLOCKHEAD 8

/* number of object each block can contain: (size - pointer_to_next) / sizeof pyintobj */
#define NOBJECT (BLOCKSIZE - BLOCKHEAD) / sizeof(PyIntObject)

/* small integer cache */
PyIntObject* small_ints[-NSMALLINTMIN+NSMALLINTMAX];

/* structure of a free block */
typedef struct _int_block {
	/* points to next block */
	struct _int_block * next;

	/* array of pyint objects */
	PyIntObject objects[NOBJECT];
}PyInt_Block;

/* head of the free blocks */
struct _int_block * block_head = NULL;

/* head of free list */
/* when int objects are de-allocated, the space will be
   linked by free list */
PyIntObject * free_list = NULL;

static void
int_dealloc(PyIntObject* v) {
	if (PyInt_CheckExact(v)) {
		/* if the object is of int type, we shoule recycle it
			from the block, and put it into free list */
		v->ob_type = (PyTypeObject*) free_list;
		free_list = v;
	}
	else {
		v->ob_type->tp_dealloc((PyObject*)v);
	}
}

static PyObject*
int_add(PyIntObject *v, PyIntObject *w) {
	register long x, i, j;

	i = v->ob_ival;
	j = w->ob_ival;

	x = i + j;

	/* Check for overflow */
	/* ^ = Bitwise XOR 
	   A bitwise XOR takes two bit patterns of equal length and performs
	   the logical exclusive OR operation on each pair of corresponding bits.
	   The result in each position is 1 if only the first bit is 1 or only
	   the second bit is 1, but will be 0 if both are 0 or both are 1.
	   
	   In this we perform the comparison of two bits, being 1 if the two bits
	   are different, and 0 if they are the same.
	   
	   For example:
	       0101 (decimal 5)
       XOR 0011 (decimal 3)
         = 0110 (decimal 6)

	   The bitwise XOR may be used to invert selected bits in a register
	   (also called toggle or flip). Any bit may be toggled by XORing it
	   with 1. For example, given the bit pattern 0010 (decimal 2) the second
	   and fourth bits may be toggled by a bitwise XOR with a bit pattern
	   containing 1 in the second and fourth positions:

	       0010 (decimal 2)
	   XOR 1010 (decimal 10)
	     = 1000 (decimal 8)
	   
	   0^1 -> 1; 1^1 -> 0; (bits are Flipped while ^ with 1)
	*/

	/* Check for OverFlow http://www.fefe.de/intof.html
	   Since the addition operation in the CPU is agnostic to whether the integer
	   is signed or unsigned, the same goes for signed integers. For 32-bit
	   signed integers, the minimum value is 0x80000000 (-2147483648) and the
	   maximum value is 0x7fffffff (2147483647). Note that there is no value than
	   can hold 2147483648, so if you negate (int)0x80000000, you get
	   (int)0x80000000 again. That is something to look out for, because it means
	   abs() returns a negative value when fed -2147483648.

       So, if you want to add two integers a and b (b >= 0), the easiest way to
	   find out if that overflows is to check whether a+b<a (or b; some people
	   think you have to check against min(a,b), but that is unnecessary).

	   If you add a positive integer to another positive integer, the result is
	   truncated. Technically, if you add two 32-bit numbers, the result has 33 bits.

On the CPU level, if you add two 32-bit integers, the lower 32 bits of the result
are written to the destination, and the 33rd bit is signalled out in some other way,
usually in the form of a "carry flag".

Unfortunately, there is no way to access this carry bit from C directly.

There are two ways to get around this:

1.Cast the numbers to a bigger integer type, then do the addition there, and check
if the result is in the right range.

2.Do the addition normally, then check the result (e.g. if (a+23<23) overflow).

3.Check before adding whether the number you add can fit (requires knowledge of the
maximum value for that data type)

Solution 1 does not work in general, because there might not be a bigger integer
type, and solution 2 does not work, because in the C language, when adding a
number to a pointer or to a signed integer, overflow is undefined. Unfortunately,
gcc as of version 4.1 abuses this and optimizes away checks that would only be
true if there was an overflow, in particular checks that check for an overflow.
Here is an example program that demonstrates the problem.

#include <assert.h>

int foo(int a) {
assert(a+100 > a);
printf("%d %d\n",a+100,a);
return a;
}

int main() {
foo(100);
foo(0x7fffffff);
}

That leaves us with approach 3. The general idea is given in the answer to
question 20.6b of the C faq. Unfortunately, the example code has several issues:

http://c-faq.com/misc/intovf.html
int
chkadd(int a, int b)
{
if(INT_MAX - b < a) {
fputs("int overflow\n", stderr);
return INT_MAX;
}
return a + b;
}

It only works for ints.
We cannot easily generalize it, because we would need a function that returns
INT_MAX for a given type.  Also, it does not catch adding -5 to INT_MIN.
	*/

/*
If x overflows, x will become 1xxx xxxx xxxx xxxx, while i and j (assume both
of them is within range) remains 0xxx xxxx xxxx xxxx.

When 0^1, 0 is flipped to 1.  So the result will be negative.

It means, if x^i or x^j is negative, x is overflowed.

BUT! if both i and j were overflowed already. will it be a problem to python?
所以python已经确保了这种情况不会发生？

Python detects overflow and converted the results into Long type:
>>> a = 2147483647 + 2147483647
[25869 refs]
>>> print a.__class__
<type 'long'>
[25869 refs]
>>>

>>> a = 2147483648 + 2147483648
[25856 refs]
>>> print a.__class__
<type 'long'>
[25856 refs]
>>> print a
4294967296
[25856 refs]
>>>


>>> a = 2147483648999
[25856 refs]
>>> print a.__class__
<type 'long'>
[25856 refs]
>>>

所以说，python已经自动关联好了类型，int放不下的，就变成Long
Long放不下的，又变成什么呢？...........-.-
*/
	if ((x^i) >= 0 && (x^j) >= 0) {
		return PyInt_From_Long(x);
	}
	else {
		return PyInt_From_Long(-999);
	}
}

/*
  register keyword
  It's a hint to the compiler that the variable will be heavily used and
  that you recommend it be kept in a processor register if possible.
  Most modern compilers do that automatically, and are better at picking
  them than us humans.

  ANSI C does not allow for taking the address of a register object; this
  restriction does not apply to C++. However, if the address-of operator (&)
  is used on an object, the compiler must put the object in a location for
  which an address can be represented. In practice, this means in memory
  instead of in a register.

  * A good reason not to use 'register': you can't take the address of
    a variable declared 'register'.  So if you know in advance that the
	& is not going to be used, use register would be a good reason.
  * Note that some/many compilers will completely ignore the register
    keyword (which is perfectly legal).

*/
static int
int_compare(PyIntObject *v, PyIntObject *w) {
	register long i = v->ob_ival;
	register long j = w->ob_ival;

	return i < j ? -1 : (i > j ? 1 : 0);
}

void
int_print(PyIntObject *v) {
	printf(" address @%p\n", v);
	printf("blocklist@%p\n", block_head);
	printf(" freelist@%p\n", free_list);
	printf("int value@%ld\n", v->ob_ival);
}

PyNumberMethods int_number_op = {
	(binaryfunc)int_add
};

PyTypeObject PyInt_Type =
{
	PyType_Init(&PyType_Type),
	0,
	"int",
	(destructor)int_dealloc,
	(printfunc)int_print,
	&int_number_op,
	0
};

PyIntObject*
fill_free_list()
{
	PyIntObject *p, *q;
	PyInt_Block* v = (PyInt_Block*) malloc(sizeof(PyInt_Block));

	v->next = block_head;
	block_head = v;

	/* link objects */
	p = &block_head->objects[0];
	q = p + NOBJECT;
	while (--q > p) {
		q->ob_type = (PyTypeObject*) (q-1);
	}
	q->ob_type = NULL;
	return q + NOBJECT - 1;
}

PyObject*
PyInt_From_Long(long ival)
{
	PyIntObject* v;

	/* use small int object pool */
	/* negative value is converted to index by adding NSMALLINTMIN */
	if (-NSMALLINTMIN+NSMALLINTMAX > 0 &&
		-NSMALLINTMIN <= ival && ival < NSMALLINTMAX) {
		v = small_ints[ival + NSMALLINTMIN];
		v->ob_refcnt ++;
		return (PyObject*) v;
	}

	/* request new block if there are no space left in free_list */
	if (free_list == NULL) {
		if ((free_list = fill_free_list()) == NULL) {
			return NULL;
		}
	}

	/* link free slots */
	v = free_list;
	free_list = (PyIntObject*) v->ob_type;
	v->ob_refcnt = 1;
	v->ob_ival = ival;
	v->ob_type = &PyInt_Type;

	return (PyObject*) v;
}

int
PyInt_Init()
{
	int i;
	PyIntObject* v;

	for (i = -NSMALLINTMIN; i < NSMALLINTMAX; i++) {
		if (!free_list && ((free_list = fill_free_list()) == NULL)) {
			return 0;
		}

		v = free_list;
		free_list = (PyIntObject*) v->ob_type;
		v->ob_refcnt=1;
		v->ob_ival=i;
		v->ob_type=&PyInt_Type;
		small_ints[i+NSMALLINTMIN] = v;
	}

	return 1;
}