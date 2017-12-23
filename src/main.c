#include "python.h"

void testIntAndList() {
	/* for the list */
	PyObject *l1, *l2 = NULL;
	PyObject *u1, *u2, *v, *w = NULL;

	PyInt_Init();
	v = PyInt_From_Long(10);
	v->ob_type->tp_print(v);

	w = PyInt_From_Long(10);
	w->ob_type->tp_print(w);

	u1 = PyInt_From_Long(999);
	u1->ob_type->tp_print(u1);
	u2 = PyInt_From_Long(9999);
	u2->ob_type->tp_print(u2);

	// test add function
	v = v->ob_type->tp_as_number->nb_add(v, w);
	v->ob_type->tp_print(v);

	/* list */
	l1 = PyList_New(10);
	l1->ob_type->tp_print(l1);
	l1->ob_type->tp_as_list->lb_setitem(l1, 0, v);
	l1->ob_type->tp_print(l1);
	l1->ob_type->tp_as_list->lb_setitem(l1, 9, w);
	l1->ob_type->tp_print(l1);
	l1->ob_type->tp_as_list->lb_setitem(l1, 3, u1);
	l1->ob_type->tp_print(l1);
	l1->ob_type->tp_as_list->lb_insertitem(l1, 3, u2);
	l1->ob_type->tp_print(l1);

	printf("Get Item 0=%p, 10=%p\n", l1->ob_type->tp_as_list->lb_getitem(l1, 0), l1->ob_type->tp_as_list->lb_getitem(l1, 10));

	l1->ob_type->tp_dealloc(l1);

	l2 = PyList_New(15);
	l2->ob_type->tp_print(l2);
	l2->ob_type->tp_as_list->lb_setitem(l2, 0, v);
	l2->ob_type->tp_print(l2);
	l2->ob_type->tp_as_list->lb_setitem(l2, 9, w);
	l2->ob_type->tp_print(l2);
	l2->ob_type->tp_as_list->lb_setitem(l2, 3, u1);
	l2->ob_type->tp_print(l2);
	l2->ob_type->tp_as_list->lb_insertitem(l2, 3, u2);
	l2->ob_type->tp_print(l2);
}

void testOverFlow() {
	/* 2**31 - 1 */
	int i = 2147483647;
	int j = 3;
	int x = i + j;
	int a = x^i;
	int b = x^j;

	printf("%d", x);
}

int main(char * args[])
{
	// testIntAndList();

	testOverFlow();

	return 0;
}