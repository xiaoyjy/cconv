#include <Python.h>
#include <cconv.h>

PyObject* py_cconv(PyObject* self, PyObject* args)
{
	char *from,
	     *to  ,
	     *instring  = NULL;

	char *outptr    = NULL,
	     *outstring = NULL;

	size_t size;
	size_t inlen  = 0,
	       outlen = 0;

	PyObject* cm;
	cconv_t conv;

	if(!PyArg_ParseTuple(args, "sss#", &from, &to, &instring, (int *)&inlen))
		return NULL;

	outlen = inlen*3;
	outptr = outstring = (char *)PyMem_Malloc(outlen);
	if(outptr == NULL)
		return PyErr_NoMemory();

	memset(outptr, 0x0, outlen);
 	if((conv = cconv_open(to, from)) == (cconv_t)(-1))
	{
		fprintf(stderr, "Not support character code set.\n");
		Py_INCREF(Py_None);
		return Py_None;
	}

	size = cconv(conv, &instring, &inlen, &outptr, &outlen);

	cconv_close(conv);
	if(size == (size_t)(-1))
	{
		fprintf(stderr, "cconv: %s\n", strerror(errno));
		PyMem_Del(outstring);
		Py_INCREF(Py_None);
		return Py_None;
	}

	cm = PyString_FromString(outstring);
	PyMem_Del(outstring);
	return cm;
}

static PyMethodDef cconvMethods[] = 
{
	{"conv", py_cconv, METH_VARARGS, "convert simplified & tranditional Chinese."},
	{NULL, NULL}
};

void initcconv() 
{
	Py_InitModule("cconv", cconvMethods);
}

