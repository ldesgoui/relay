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

static int
match(const char *pattern, const char *data,
		PyObject *results, PyObject *mapped_results);

static const char *parse_match_docstring =
"Match the pattern from the first argument with the data from the second "
"argument, both being strings. Returns a tuple of a list and a tuple."

static PyObject *
parse_match(PyObject *self, PyObject *args)
{
	const char		*pattern;
	const char		*data;
	PyObject		*results;
	PyObject		*mapped_results;

	if (!PyArg_ParseTuple(args, "ss", &pattern, &data))
	{
		PyErr_SetString(PyExc_TypeError, "Arguments must be strings");
		return Py_None;
	}
	results = PyList_New(0);
	mapped_results = PyDict_New();
	if (match(pattern, data, results, mapped_results) != 0)
	{
		PyErr_SetString(PyExc_ValueError, "Parsing went wrong");
		// TODO: what happens in the case of an exception when both objects are initialized ?
		// Do they leak?
		return Py_None;
	}
	return Py_BuildValue("(OO)", PyList_AsTuple(results), mapped_results);
	// TODO: would the AsTuple leak the original results ?
}

/*! Match:
**! {name:format}
**!
**! Working:
**! "hello {target}!" "hello world!" -> () {'target': 'world'}
**! "hey {}" "hey :)" -> (":)") {}
**!
**! To be added:
**! ":{sender:@'{nick}@{host}'} PRIVMSG {target} :{message}"  ":sup@example.com PRIVMSG #chan :sup"
**! -> () {'sender': {'data': 'sup@example.com', 'nick': 'sup', 'host': 'example.com'}, 'target': '#chan', 'message': 'sup'}
*/
static int
match(const char *pattern, const char *data,
		PyObject *results, PyObject *mapped_results)
{
	int i;
	int x;
	const char *key;
	int keylen;
	const char *value;
	int valuelen;

	i = 0;
	x = 0;
	while (pattern[i] && data[x])
	{
		if (pattern[i] == '{')
		{
			if (pattern[i + 1] == '}')
			{
				key = NULL;
				keylen = 1;
			}
			else
			{
				key = &pattern[i + 1];
				keylen = 0;
				while (key[keylen] != '}')
				{
					if (key[keylen] == '\0')
					{
						return (1);
					}
					keylen++;
				}
			}
			i += keylen + 2;
			value = &data[x];
			valuelen = 0;
			while (value[valuelen] != pattern[i])
			{
				if (value[valuelen] == '\0')
				{
					return (1);
				}
				valuelen++;
			}
			x += valuelen;
			if (key == NULL)
			{
				PyList_Append(results, Py_BuildValue("s#", value, valuelen));
			}
			else
			{
				PyDict_SetItem(mapped_results, Py_BuildValue("s#", key, keylen),
						Py_BuildValue("s#", value, valuelen));
			}
		}
		else if (pattern[i] != data[x])
		{
			return (1);
		}
		i++;
		x++;
	}
	return (0);
}


/*
** Module Initialization
*/

static const char *parse_module_docstring =
"Relay.parse\n"
"~~~~~~~~~~~\n\n"
"This is a C extension due to the fact that I have more experience"
"manipulating strings with C.\n"
"It was also a good excuse to learn how to create a C module for Python.\n\n"
"Copyright (c) 2015, ldesgoui <relay at ldesgoui dot xyz>\n"
"See LICENSE for more informations.";

static PyMethodDef parse_methods[] = {
	{"match", parse_match, METH_VARARGS, parse_match_docstring,},
	{NULL, NULL, 0, NULL}
};

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
	return PyModule_Create(&parsemodule);
}
