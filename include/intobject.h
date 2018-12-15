#ifndef Py_INTOBJECT_H
#define Py_INTOBJECT_H

typedef struct {
	PyObject_HEAD
	long ob_ival;
} PyIntObject;

PyTypeObject PyInt_Type;
#define PyInt_CheckExact(op) ((op)->ob_type == &PyInt_Type)

PyObject* PyInt_From_Long(long);
int PyInt_Init();

#endif /* Py_INTOBJECT_H */