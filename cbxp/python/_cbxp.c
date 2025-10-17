#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#include "cbxp.h"
#include "cbxp_result.h"

pthread_mutex_t cbxp_mutex = PTHREAD_MUTEX_INITIALIZER;

// Entry point to the call_cbxp() function
static PyObject* call_cbxp(PyObject* self, PyObject* args, PyObject* kwargs) {
  PyObject* result_dictionary;
  PyObject* debug_pyobj;
  const char* control_block;
  const char* includes_string;
  Py_ssize_t request_length;
  bool debug            = false;

  static char* kwlist[] = {"request", "include", "debug", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|sO", kwlist, &control_block,
                                   &includes_string, &debug_pyobj)) {
    return NULL;
  }

  debug = PyObject_IsTrue(debug_pyobj);

  // Since cbxp manages cbxp_result_t as a static structure,
  // we need to use a mutex to make this thread safe.
  // Technically we shouldn't need this because the Python GIL,
  // but we will set this up anyways to be safe.
  pthread_mutex_lock(&cbxp_mutex);

  cbxp_result_t* result = cbxp(control_block, includes_string, debug);

  result_dictionary     = Py_BuildValue(
      "{s:s#, s:i}", "result_json", result->result_json,
      result->result_json_length, "return_code", result->return_code);

  pthread_mutex_unlock(&cbxp_mutex);

  return result_dictionary;
}

// Method definition
static PyMethodDef _C_methods[] = {
    {"call_cbxp", (PyCFunction)call_cbxp, METH_VARARGS | METH_KEYWORDS,
     "A unified and standardized interface for extracting z/OS control block "
     "data."},
    {NULL}
};

// Module definition
static struct PyModuleDef _C_module_def = {
    PyModuleDef_HEAD_INIT, "_C",
    "A unified and standardized interface for extracting z/OS control block "
    "data.",
    -1, _C_methods};

// Module initialization function
// 'unusedFunction' is a false positive since 'PyInit__C()' is used by the
// Python interpreter
// cppcheck-suppress unusedFunction
PyMODINIT_FUNC PyInit__C(void) {
  Py_Initialize();
  return PyModule_Create(&_C_module_def);
}
