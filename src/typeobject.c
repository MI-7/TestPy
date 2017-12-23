#include "python.h"

PyTypeObject PyType_Type =
{
	PyType_Init(&PyType_Type),
	0,
	"type",
	0,
	0,
	0,
};