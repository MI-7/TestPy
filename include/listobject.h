#ifndef PY_LISTOBJECT_H
#define PY_LISTOBJECT_H

typedef struct {
	PyObject_VAR_HEAD
	PyObject **ob_items;
	int allocated;
} PyListObject;

PyObject* PyList_New(int size);

#endif