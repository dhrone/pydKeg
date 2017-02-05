#include <Python.h>
#include <numpy/arrayobject.h>
#include "winstar_weg.h"

/* Docstrings */
static char module_docstring[] = "This module provides an interface to the winstar weg graphics display.";
static char init_docstring[] = "Initialize display";
static char init_gpio_docstring[] = "Initialize GPIO";
static char updateframe_docstring[] = "Send frame to display";
static char printframe_docstring[] = "Send frame to stdout";
static char clear_docstring[] = "Clear display";

/* Available functions */
static PyObject *winstar_weg_init(PyObject *self, PyObject *args);
static PyObject *winstar_weg_init_gpio(PyObject *self, PyObject *args);
static PyObject *winstar_weg_updateframe(PyObject *self, PyObject *args);
static PyObject *winstar_weg_printframe(PyObject *self, PyObject *args);
static PyObject *winstar_weg_clear(PyObject *self, PyObject *args);

/* Module specification */
static PyMethodDef module_methods[] = {
		{"init", winstar_weg_init, METH_VARARGS, init_docstring},
		{"initgpio", winstar_weg_init_gpio, METH_VARARGS, init_gpio_docstring},
		{"updateframe", winstar_weg_updateframe, METH_VARARGS, updateframe_docstring},
		{"printframe", winstar_weg_printframe, METH_VARARGS, printframe_docstring},
		{"clear", winstar_weg_clear, METH_VARARGS, clear_docstring},
		{NULL, NULL, 0, NULL}
};

/* Initialize the module */
PyMODINIT_FUNC init_winstar_weg(void)
{
		PyObject *m = Py_InitModule3("_winstar_weg", module_methods, module_docstring);
		if (m == NULL)
				return;

		/* Load `numpy` functionality. */
		import_array();
}

static PyObject *winstar_weg_init_gpio(PyObject *self, PyObject *args)
{
		int rs, e, d4, d5, d6, d7;
		/* Parse the input tuple */
		if (!PyArg_ParseTuple(args, "iiiiii", &rs, &e, &d4, &d5, &d6, &d7)) return NULL;

		/* Call the external C function to initialize the display */
		init_gpio(rs, e, d4, d5, d6, d7);
		Py_RETURN_NONE;
}

static PyObject *winstar_weg_init(PyObject *self, PyObject *args)
{
		int rs, e, d4, d5, d6, d7;
		/* Parse the input tuple */
		if (!PyArg_ParseTuple(args, "iiiiii", &rs, &e, &d4, &d5, &d6, &d7)) return NULL;

		/* Call the external C function to initialize the display */
		init(rs, e, d4, d5, d6, d7);
		Py_RETURN_NONE;
}

static PyObject *winstar_weg_clear(PyObject *self, PyObject *args)
{
		int rs, e, d4, d5, d6, d7;
		/* Parse the input tuple */
		if (!PyArg_ParseTuple(args, "iiiiii", &rs, &e, &d4, &d5, &d6, &d7)) return NULL;

		/* Call the external C function to initialize the display */
		clear(rs, e, d4, d5, d6, d7);
		Py_RETURN_NONE;
}

static PyObject *winstar_weg_updateframe(PyObject *self, PyObject *args)
{
		int rs, e, d4, d5, d6, d7;
		PyObject *f_obj;

		/* Parse the input tuple */
		if (!PyArg_ParseTuple(args, "iiiiiiO", &rs, &e, &d4, &d5, &d6, &d7, &f_obj))
				return NULL;

		/* Interpret the frame object as a numpy array. */
		PyObject *f_array = PyArray_FROM_OTF(f_obj, NPY_INT, NPY_IN_ARRAY);

		/* If that didn't work, throw an exception. */
		if (f_array == NULL) {
				Py_XDECREF(f_array);
				PyErr_SetString(PyExc_RuntimeError,
										"Couldn't retrieve frame.");
				return NULL;
		}

		/* Check to make sure that the frame is a two dimension array */
		int ND = (int) PyArray_NDIM(f_array);
		if (ND != 2) {
				PyErr_SetString(PyExc_RuntimeError,
					"Frame is the wrong shape. Must be two dimensional array.");
				return NULL;
		}

		/* How many data points are there? */
		int rows = (int)PyArray_DIM(f_array, 0);
		int cols = (int)PyArray_DIM(f_array, 1);

		/* Get pointers to the data as C-types. */
		int *f = (int*)PyArray_DATA(f_array);

		/* Call the external C function. */
		updateframe(rs, e, d4, d5, d6, d7, cols, rows, f);

		/* Clean up. */
		Py_DECREF(f_array);

		Py_RETURN_NONE;

}


static PyObject *winstar_weg_printframe(PyObject *self, PyObject *args)
{
		PyObject *f_obj;

		/* Parse the input tuple */
		if (!PyArg_ParseTuple(args, "O", &f_obj))
				return NULL;

		/* Interpret the frame object as a numpy array. */
		PyObject *f_array = PyArray_FROM_OTF(f_obj, NPY_INT, NPY_IN_ARRAY);

		/* If that didn't work, throw an exception. */
		if (f_array == NULL) {
				Py_XDECREF(f_array);
				PyErr_SetString(PyExc_RuntimeError,
					"Couldn't retrieve frame.");
				return NULL;
		}

		/* Check to make sure that the frame is a two dimension array */
		int ND = (int) PyArray_NDIM(f_array);
		if (ND != 2) {
				PyErr_SetString(PyExc_RuntimeError,
					"Frame is the wrong shape. Must be two dimensional array.");
				return NULL;
		}

		/* How many data points are there? */
		int rows = (int)PyArray_DIM(f_array, 0);
		int cols = (int)PyArray_DIM(f_array, 1);

		/* Get pointers to the data as C-types. */
		int *f = (int*)PyArray_DATA(f_array);

		/* Call the external C function. */
		printframe(cols, rows, f);

		/* Clean up. */
		Py_DECREF(f_array);

		Py_RETURN_NONE;
}


// static PyObject *chi2_chi2(PyObject *self, PyObject *args)
// {
// 		double m, b;
// 		PyObject *x_obj, *y_obj, *yerr_obj;
//
// 		/* Parse the input tuple */
// 		if (!PyArg_ParseTuple(args, "ddOOO", &m, &b, &x_obj, &y_obj,
// 																				 &yerr_obj))
// 				return NULL;
//
// 		/* Interpret the input objects as numpy arrays. */
// 		PyObject *x_array = PyArray_FROM_OTF(x_obj, NPY_DOUBLE, NPY_IN_ARRAY);
// 		PyObject *y_array = PyArray_FROM_OTF(y_obj, NPY_DOUBLE, NPY_IN_ARRAY);
// 		PyObject *yerr_array = PyArray_FROM_OTF(yerr_obj, NPY_DOUBLE,
// 																						NPY_IN_ARRAY);
//
// 		/* If that didn't work, throw an exception. */
// 		if (x_array == NULL || y_array == NULL || yerr_array == NULL) {
// 				Py_XDECREF(x_array);
// 				Py_XDECREF(y_array);
// 				Py_XDECREF(yerr_array);
// 				return NULL;
// 		}
//
// 		/* How many data points are there? */
// 		int N = (int)PyArray_DIM(x_array, 0);
//
// 		/* Get pointers to the data as C-types. */
// 		double *x    = (double*)PyArray_DATA(x_array);
// 		double *y    = (double*)PyArray_DATA(y_array);
// 		double *yerr = (double*)PyArray_DATA(yerr_array);
//
// 		/* Call the external C function to compute the chi-squared. */
// 		double value = chi2(m, b, x, y, yerr, N);
//
// 		/* Clean up. */
// 		Py_DECREF(x_array);
// 		Py_DECREF(y_array);
// 		Py_DECREF(yerr_array);
//
// 		if (value < 0.0) {
// 				PyErr_SetString(PyExc_RuntimeError,
// 										"Chi-squared returned an impossible value.");
// 				return NULL;
// 		}
//
// 		/* Build the output tuple */
// 		PyObject *ret = Py_BuildValue("d", value);
// 		return ret;
// }
