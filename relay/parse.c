/*
** Relay.parse
** ~~~~~~~~~~~
**
** This is a C extension due to the fact that I have more experience
** manipulating strings with C.
** It was also a good excuse to learn how to create a C module for Python.
**
** Copyright (c) 2015, ldesgoui <relay at ldesgoui dot xyz>
** See LICENSE for more informations.
*/

#include <Python.h>

/*
** parse.match(pattern: str, data: str) -> None | (tuple, dict)
*/
static PyObject *
parse_match(PyObject *self, PyObject *args)
{
	const char		*pattern;
	const char		*data;
	PyObject		*r_args = PyList_New(0);
	PyObject		*r_kwargs = PyDict_New();
	int				i;
	int				x;
	const char		*key;
	int				keylen;
	const char		*value;
	int				valuelen;


	if (!PyArg_ParseTuple(args, "ss", &pattern, &data))
		return Py_None;

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
						return Py_None;
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
					return Py_None;
				}
				valuelen++;
			}
			x += valuelen;
			if (key == NULL)
			{
				PyList_Append(r_args, Py_BuildValue("s#", value, valuelen));
			}
			else
			{
				PyDict_SetItem(r_kwargs, Py_BuildValue("s#", key, keylen),
						Py_BuildValue("s#", value, valuelen));
			}
		}
		else if (pattern[i] != data[x])
		{
			return Py_None;
		}
		i++;
		x++;
	}
	return Py_BuildValue("(OO)", r_args, r_kwargs);
}

//static char *ParseDocstring = "Relay.parse\n~~~~~~~~~~~\n\n"
//"This is a C extension due to the fact that I have more experience\n"
//"manipulating strings with C.\n"
//"It was also a good excuse to learn how to create a C module for Python.\n\n"
//"Copyright (c) 2015, ldesgoui <relay at ldesgoui dot xyz>\n"
//"See LICENSE for more informations.";

static PyMethodDef ParseMethods[] = {
	{"match", parse_match, METH_VARARGS, "Match the pattern from the first "
		"argument with the data from the second argument. "
			"Returns either None or a tuple and a dict in a tuple."},
	{NULL, NULL, 0, NULL}
};

static struct PyModuleDef parsemodule = {
	PyModuleDef_HEAD_INIT,
	"parse",
	NULL,
	-1,
	ParseMethods,
};

PyMODINIT_FUNC
PyInit_parse(void)
{
	return PyModule_Create(&parsemodule);
}
