#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <stdbool.h>
#include <stdlib.h>

#include "cbxp.h"

// Entry point to the call_cbxp_extract() function
static PyObject* call_cbxp_extract(PyObject* self, PyObject* args,
                                   PyObject* kwargs) {
  PyObject* result_dictionary;
  PyObject* debug_pyobj;
  const char* p_control_block;
  const char* p_includes_string;
  const char* p_filters_string;
  Py_ssize_t py_control_block_length, py_includes_length, py_filters_length;
  bool debug            = false;

  static char* kwlist[] = {"control_block", "includes_string", "filters_string",
                           "debug", NULL};

  if (!PyArg_ParseTupleAndKeywords(
          args, kwargs, "s#s#s#|O", kwlist, &p_control_block,
          &py_control_block_length, &p_includes_string, &py_includes_length,
          &p_filters_string, &py_filters_length, &debug_pyobj)) {
    return NULL;
  }

  debug = PyObject_IsTrue(debug_pyobj);

  int control_block_length =
      Py_SAFE_DOWNCAST(py_control_block_length, Py_ssize_t, int);
  int includes_length = Py_SAFE_DOWNCAST(py_includes_length, Py_ssize_t, int);
  int filters_length  = Py_SAFE_DOWNCAST(py_filters_length, Py_ssize_t, int);

  cbxp_result_t* p_cbxp_result =
      cbxp_extract(p_control_block, control_block_length, p_includes_string,
                   includes_length, p_filters_string, filters_length, debug);

  result_dictionary =
      Py_BuildValue("{s:s#, s:i}", "result_json", p_cbxp_result->result_json,
                    p_cbxp_result->result_json_length, "return_code",
                    p_cbxp_result->return_code);

  cbxp_free(p_cbxp_result, debug);

  return result_dictionary;
}

// Entry point to the call_cbxp_format() function
static PyObject* call_cbxp_format(PyObject* self, PyObject* args,
                                  PyObject* kwargs) {
  PyObject* result_dictionary;
  PyObject* debug_pyobj;
  const char* p_control_block;
  const char* p_data_buffer;
  Py_ssize_t py_buffer_length, py_control_block_length;
  unsigned int offset   = 0;
  bool debug            = false;

  static char* kwlist[] = {"control_block", "data", "offset", "debug", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s#s#|IO", kwlist,
                                   &p_control_block, &py_control_block_length,
                                   &p_data_buffer, &py_buffer_length, &offset,
                                   &debug_pyobj)) {
    return NULL;
  }

  debug = PyObject_IsTrue(debug_pyobj);
  int control_block_length =
      Py_SAFE_DOWNCAST(py_control_block_length, Py_ssize_t, int);
  int buffer_length = Py_SAFE_DOWNCAST(py_buffer_length, Py_ssize_t, int);

  p_data_buffer += offset;
  buffer_length -= offset;

  cbxp_result_t* p_cbxp_result =
      cbxp_format(p_control_block, control_block_length, p_data_buffer,
                  buffer_length, debug);

  result_dictionary =
      Py_BuildValue("{s:s#, s:i}", "result_json", p_cbxp_result->result_json,
                    p_cbxp_result->result_json_length, "return_code",
                    p_cbxp_result->return_code);

  cbxp_free(p_cbxp_result, debug);

  return result_dictionary;
}

// Method definition
static PyMethodDef _C_methods[] = {
    {"call_cbxp_extract", (PyCFunction)call_cbxp_extract,
     METH_VARARGS | METH_KEYWORDS,
     "Extract z/OS control block data from live memory."},
    {"call_cbxp_format", (PyCFunction)call_cbxp_format,
     METH_VARARGS | METH_KEYWORDS, "Format user provided control block data."},
    {NULL}
};

// Module definition
static struct PyModuleDef _C_module_def = {
    PyModuleDef_HEAD_INIT, "_C",
    "A unified and standardized interface for extracting and formatting "
    "z/OS control block data.",
    -1, _C_methods};

// Module initialization function
// 'unusedFunction' is a false positive since 'PyInit__C()' is used by the
// Python interpreter
// cppcheck-suppress unusedFunction
PyMODINIT_FUNC PyInit__C(void) {
  Py_Initialize();
  return PyModule_Create(&_C_module_def);
}
