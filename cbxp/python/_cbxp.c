#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <stdbool.h>
#include <stdlib.h>

#include "cbxp.h"
#include "cbxp_result.h"

// Entry point to the call_cbxp() function
static PyObject* call_cbxp(PyObject* self, PyObject* args, PyObject* kwargs) {
  PyObject* result_dictionary;
  PyObject* debug_pyobj;
  const char* p_control_block;
  const char* p_includes_string;
  const char* p_filters_string;
  Py_ssize_t request_length;
  bool debug            = false;

  static char* kwlist[] = {"request", "include", "p_filters_string", "debug",
                           NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s|ssO", kwlist,
                                   &p_control_block, &p_includes_string,
                                   &p_filters_string, &debug_pyobj)) {
    return NULL;
  }

  debug = PyObject_IsTrue(debug_pyobj);

  cbxp_result_t* p_cbxp_result =
      cbxp(p_control_block, p_includes_string, p_filters_string, debug);

  result_dictionary =
      Py_BuildValue("{s:s#, s:i}", "result_json", p_cbxp_result->result_json,
                    p_cbxp_result->result_json_length, "return_code",
                    p_cbxp_result->return_code);

  cbxp_free(p_cbxp_result);

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
