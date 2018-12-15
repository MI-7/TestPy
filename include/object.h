#ifndef Py_OBJECT_H
#define Py_OBJECT_H

#define Py_ssize_t int

#define PyObject_HEAD \
	Py_ssize_t ob_refcnt;    \
	struct _typeobject *ob_type;
/* Must use "struct _typeobject *ob_type" instead of "PyTypeObject *ob_type"
   Because #define won't replace PyTypeObject with struct _typeobject
*/

#define PyObject_VAR_HEAD \
	PyObject_HEAD \
	Py_ssize_t ob_size;

typedef struct _object {
	PyObject_HEAD
} PyObject;

typedef struct _varobject {
	PyObject_VAR_HEAD
} PyVarObject;

typedef PyObject * (*unaryfunc)(PyObject *);
typedef PyObject * (*binaryfunc)(PyObject *, PyObject *);
typedef void (*destructor)(PyObject *);
typedef void (*printfunc)(PyObject *);

typedef void (*insertfunc) (PyObject *, int, PyObject *);
typedef PyObject * (*getfunc) (PyObject *, int);

typedef struct {
	//unaryfunc nb_negative;
	//unaryfunc nb_positive;
	//unaryfunc nb_absolute;

	binaryfunc nb_add;
	//binaryfunc nb_subtract;
	//binaryfunc nb_multiply;
	//binaryfunc nb_divide;
} PyNumberMethods;

typedef struct {
	insertfunc lb_insertitem;
	insertfunc lb_setitem;

	getfunc lb_getitem;

} PyListMethods;

/* Why there is a PyObject_VAR_HEAD in the type object? 
   Sometimes the ob_type object is used as pointer to link
   other objects, such as the free_list of int block.

   So ob_type has to be either NULL or pointing to another PyIntObject
   */
typedef struct _typeobject {
	PyObject_VAR_HEAD
	const char *tp_name;
	// Py_ssize_t tp_basicsize, tp_itemsize;
	
	destructor tp_dealloc;

	printfunc tp_print;

	/* lots of other attributes ...*/
	PyNumberMethods *tp_as_number;

	/* list functions */
	PyListMethods *tp_as_list;
} PyTypeObject;

#define PyType_Init(op) \
	1, op

PyTypeObject PyType_Type;


/* some function definitions */
void _Py_NewReference(PyObject *op);

#endif /* Py_OBJECT_H */