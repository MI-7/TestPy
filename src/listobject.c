#include "python.h"

#define MAX_LIST_CACHE 80
static PyListObject * list_cache[MAX_LIST_CACHE];
static int free_list_number = 0;

static void
list_dealloc(PyObject *op) {
	printf("list cache size:%d\n", free_list_number);

	/* should minus 1 for each object holding in the list */
	/* ... */

	/* free the list member anyway, keep only the memory of list obj */
	free(((PyListObject*)op) ->ob_items);

	if (free_list_number > MAX_LIST_CACHE) {
		free(op);
	}
	else {
		memset(((PyListObject *)op)->ob_items, 0, 
			(size_t) (((PyListObject *)op)->ob_size * sizeof(PyObject*)));
		list_cache[free_list_number] = (PyListObject*)op;
		free_list_number ++;
	}
	printf("list cache size:%d\n", free_list_number);
}

static void
list_print(PyObject *op) {
	int i = 0;
	PyListObject *l = (PyListObject *) op;
	printf("******\n");
	printf("Size=%d, Allocated=%d\n", l->ob_size, l->allocated);
	printf("Addr of Items:%p\n", l->ob_items);
	for (i = 0; i < l->ob_size; i++) {
		printf("_%p_", l->ob_items[i]);
	}
	printf("\n******\n");
}

PyObject*
list_add(PyObject *list1, PyObject *list2) {
	return NULL;
}

void
list_insert(PyObject *list, int loc, PyObject *item) {
	PyListObject *l = (PyListObject *) list;
	int size = l->ob_size;
	int i = 0;

	if (l == NULL) {
		return;
	}

	if (loc < 0) {
		loc += size;
	}

	if (loc < 0) {
		loc = 0;
	}

	if (loc > size) {
		loc = size;
	}
	
	/* the logic should be:
	   if allocated > size, the memory should be enough to hold the new object pointer
	   if allocated < size, then we should allocate more memory to hold the new obj */
	realloc(l->ob_items, (size_t) ((size + 1) * sizeof(PyObject *)));
	l->ob_size ++;

	for (i=size-1; i>=loc; i--) {
		l->ob_items[i+1] = l->ob_items[i];
	}
	l->ob_items[loc] = item;
	item->ob_refcnt ++;
}

void
list_setitem(PyObject *list, int loc, PyObject *item) {
	PyListObject *l = (PyListObject *) list;
	PyObject ** items;
	PyObject *  olditem;

	int size = l->ob_size;
	
	if (loc < 0 || loc >= size) {
		/* should raise an error, index out of range */
		return;
	}

	items = l->ob_items;
	olditem = items[loc];

	if (olditem != NULL) {
		olditem->ob_refcnt --;
	}
	items[loc] = item;
}

PyObject*
list_getitem(PyObject *l, int loc) {
	int size = ((PyListObject*)l) ->ob_size;

	if (loc < 0 || loc >= size) {
		/* should raise error, index out of range */
		return NULL;
	}

	return ((PyListObject*) l)->ob_items[loc];
}

PyNumberMethods list_number_op = {
	(binaryfunc)list_add
};

PyListMethods list_list_op = {
	(insertfunc) list_insert,
	(insertfunc) list_setitem,
	(getfunc) list_getitem
};

PyTypeObject PyList_Type =
{
	PyType_Init(&PyType_Type),
	0,
	"list",
	(destructor)list_dealloc,
	(printfunc)list_print,
	&list_number_op,
	&list_list_op,
};

PyObject*
PyList_New(int size) {
	PyListObject *op;
	size_t nbytes;

	nbytes = size * sizeof(PyObject *);
	if (nbytes / sizeof(PyObject *) != (size_t) size) {
		/* memory error */
	}

	/* if cache is available, use the cache */
	if (free_list_number) {
		free_list_number--;
		op = list_cache[free_list_number];
		_Py_NewReference((PyObject *) op);
	} else {
		/* op = PyObject_GC_New(PyListObject, &PyList_Type); */
		op = (PyListObject *) malloc(sizeof(PyListObject));
		op->ob_type = &PyList_Type;
		op->ob_refcnt = 1;
	}

	if (size <= 0) {
		op->ob_items = NULL;
	} else {
		op->ob_items = (PyObject **) malloc(nbytes);
		memset(op->ob_items, 0, nbytes);
	}

	op->ob_size = size;
	op->allocated = size;

	printf("New List Created:%p, Item Address:%p\n", op, op->ob_items);

	return (PyObject *) op;
}