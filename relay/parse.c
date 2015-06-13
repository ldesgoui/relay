/*!
**! Relay.parse
**! ~~~~~~~~~~~
**!
**! This is a C extension due to the fact that I have more experience
**! manipulating strings with C.
**! It was also a good excuse to learn how to create a C module for Python.
**!
**! Copyright (c) 2015, ldesgoui <relay at ldesgoui dot xyz>
**! See LICENSE for more informations.
*/

#include <Python.h>

static size_t
charspan(const char *str, const char c)
{
	const char	cs[] = {c, '\0'};

	return strcspn(str, cs);
}

static char *
match(const char *pattern, const char *data,
		PyObject *results, PyObject *mapped_results)
{
	Py_ssize_t		i, x, k, v;

	i = 0; x = 0;
	while (pattern[i] && data[x])
	{
		if (pattern[i] == '{' && pattern[i + 1] == '{')
			++i;
		else if (pattern[i] == '}' && pattern[i + 1] == '}')
			++i;
		else if (pattern[i] == '{')
		{
			k = charspan(&pattern[i], '}');
			if (pattern[i + k] != '}')
				return "end of string while looking for end of qualifier";
			v = charspan(&data[x], pattern[i + k + 1]);
			if (data[x + v] != pattern[i + k + 1])
				return "end of string while matching";
			if (k == 1)
				PyList_Append(results, Py_BuildValue("s#", &data[x], v));
			else
				PyDict_SetItem(mapped_results,
						Py_BuildValue("s#", &pattern[i + 1], k - 1),
						Py_BuildValue("s#", &data[x], v));
			i += k;
			x += v - 1;
		}
		else if (pattern[i] != data[x])
			break ;
		++i; ++x;
	}
	if (pattern[i] != data[x])
		return "mis-match in values";
	return NULL;
}


static const char parse_match_docstring[] =
"Match the pattern from the first argument with the data from the second "
"argument, both being strings. Returns tuple(tuple, dictionary).";

static PyObject *
parse_match(PyObject *self, PyObject *args)
{
	const char		*pattern;
	const char		*data;
	PyObject		*results;
	PyObject		*mapped_results;
	const char		*error;

	if (!PyArg_ParseTuple(args, "ss", &pattern, &data))
	{
		PyErr_SetString(PyExc_TypeError, "arguments must be 2 strings");
		return NULL;
	}
	results = PyList_New(0);
	mapped_results = PyDict_New();
	error = match(pattern, data, results, mapped_results);
	if (error)
	{
		// TODO: what happens in the case of an exception when both objects are initialized ?
		// Do they leak?
		PyErr_SetString(PyExc_ValueError, error);
		return NULL;
	}
	return Py_BuildValue("(OO)", PyList_AsTuple(results), mapped_results);
	// TODO: would the AsTuple leak the original results ?
}


/*
** Module Initialization
*/

static const char parse_module_docstring[] =
"Relay.parse\n"
"~~~~~~~~~~~\n\n"
"This is a C extension due to the fact that I have more experience"
"manipulating strings with C.\n"
"It was also a good excuse to learn how to create a C module for Python.\n\n"
"Copyright (c) 2015, ldesgoui <relay at ldesgoui dot xyz>\n"
"See LICENSE for more informations.";

static PyMethodDef parse_methods[] = {
	{"match", parse_match, METH_VARARGS, parse_match_docstring},
	{NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION >= 3
static struct PyModuleDef parse_module = {
	PyModuleDef_HEAD_INIT,
	"parse",
	parse_module_docstring,
	-1,
	parse_methods,
};

PyMODINIT_FUNC
PyInit_parse(void)
{
	return PyModule_Create(&parse_module);
}
#else
PyMODINIT_FUNC
initparse(void)
{
	Py_InitModule3("parse", parse_methods, parse_module_docstring);
}
#endif
