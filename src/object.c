#include "python.h"

void
_Py_NewReference(PyObject *op) {
	op->ob_refcnt = 1;
	/* add to all objects ??? */
}