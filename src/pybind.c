#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "trisort.h"

static PyObject* trisort_inplace(PyObject *self, PyObject *obj)
{
    Py_buffer view;
    if (PyObject_GetBuffer(obj, &view, PyBUF_WRITABLE | PyBUF_FORMAT) < 0)
        return NULL;
    if (view.itemsize != sizeof(int) || view.ndim != 1) {
        PyBuffer_Release(&view);
        PyErr_SetString(PyExc_TypeError, "Need 1-D int32/64 buffer");
        return NULL;
    }
    trisort_plus_c((int*)view.buf, view.len / sizeof(int));
    PyBuffer_Release(&view);
    Py_RETURN_NONE;
}
static PyMethodDef Methods[] = {
    {"trisort_inplace", trisort_inplace, METH_O,
     "In-place TriSort+ for Python buffer objects"},
    {NULL, NULL, 0, NULL}};
static struct PyModuleDef mod = {PyModuleDef_HEAD_INIT,
                                 "trisortc", NULL, -1, Methods};
PyMODINIT_FUNC PyInit_trisortc(void) { return PyModule_Create(&mod); }
