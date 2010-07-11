#include <Python.h>
#include "strfry.h"

static PyObject *normalize = NULL;

static PyObject * strfry_jaro_winkler(PyObject *self, PyObject *args,
    PyObject *keywds)
{
    const char *s1, *s2;
    double result;
    bool ignore_case = true;

    static char *kwlist[] = {"string1", "string2", "ignore_case", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "ss|B", kwlist, &s1, &s2,
                                     &ignore_case)) {
        return NULL;
    }

    result = jaro_winkler(s1, s2, ignore_case, false);

    return Py_BuildValue("d", result);
}

static PyObject * strfry_jaro_distance(PyObject *self, PyObject *args,
    PyObject *keywds)
{
    const char *s1, *s2;
    double result;
    bool ignore_case = true;

    static char *kwlist[] = {"string1", "string2", "ignore_case", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "ss|B", kwlist, &s1, &s2,
                                     &ignore_case)) {
        return NULL;
    }

    result = jaro_distance(s1, s2, ignore_case);

    return Py_BuildValue("d", result);
}

static PyObject * strfry_hamming_distance(PyObject *self, PyObject *args,
                                          PyObject *keywds)
{
    const char *s1, *s2;
    unsigned result;
    bool ignore_case = true;

    static char *kwlist[] = {"string1", "string2", "ignore_case", NULL};

    if (!PyArg_ParseTupleAndKeywords(args, keywds, "ss|B", kwlist, &s1, &s2,
                                     &ignore_case)) {
        return NULL;
    }

    result = hamming_distance(s1, s2, ignore_case);

    return Py_BuildValue("I", result);
}

static PyObject* strfry_levenshtein_distance(PyObject *self, PyObject *args)
{
    const char *s1, *s2;
    unsigned result;

    if (!PyArg_ParseTuple(args, "ss", &s1, &s2)) {
        return NULL;
    }

    result = levenshtein_distance(s1, s2);

    return Py_BuildValue("I", result);
}

static PyObject* strfry_soundex(PyObject *self, PyObject *args)
{
    PyObject *pystr;
    PyObject *normalized;
    PyObject* ret;
    char *result;

    if (!PyArg_ParseTuple(args, "O", &pystr)) {
        return NULL;
    }

    if(PyString_Check(pystr)) {
        result = soundex(PyString_AS_STRING(pystr));
    } else if(PyUnicode_Check(pystr)) {
        normalized = PyObject_CallFunction(normalize, "sO", "NFKD", pystr);
        pystr = PyUnicode_AsUTF8String(normalized);
        result = soundex((const char*)PyString_AS_STRING(pystr));
        Py_DECREF(pystr);
        Py_DECREF(normalized);
    } else {
        PyErr_SetString(PyExc_TypeError, "expected a str or unicode");
        return NULL;
    }

    ret = Py_BuildValue("s", result);
    free(result);

    return ret;
}

static PyObject* strfry_metaphone(PyObject *self, PyObject *args)
{
    PyObject *pystr;
    PyObject *normalized;
    PyObject *ret;
    char *result;

    if (!PyArg_ParseTuple(args, "O", &pystr)) {
        return NULL;
    }

    if (PyString_Check(pystr)) {
        result = metaphone(PyString_AsString(pystr));
    } else if (PyUnicode_Check(pystr)) {
        normalized = PyObject_CallFunction(normalize, "sO", "NFKD", pystr);
        pystr = PyUnicode_AsUTF8String(normalized);
        result = metaphone((const char*)PyString_AS_STRING(pystr));
        Py_DECREF(pystr);
        Py_DECREF(normalized);
    } else {
        PyErr_SetString(PyExc_TypeError, "expected a str or unicode");
        return NULL;
    }

    ret = Py_BuildValue("s", result);
    free(result);

    return ret;
}

static PyMethodDef StrfryMethods[] = {
    {"jaro_winkler", strfry_jaro_winkler, METH_VARARGS | METH_KEYWORDS,
     "jaro_winkler(string1, string2, ignore_case=True)\n\n"
     "Do a Jaro-Winkler string comparison between string1 and string2."},

    {"jaro_distance", strfry_jaro_distance, METH_VARARGS | METH_KEYWORDS,
     "jaro_distance(string1, string2, ignore_case=True)\n\n"
     "Get a Jaro string distance metric for string1 and string2."},

    {"hamming_distance", strfry_hamming_distance, METH_VARARGS | METH_KEYWORDS,
     "hamming_distance(string1, string2, ignore_case=True)\n\n"
     "Compute the Hamming distance between string1 and string2."},

    {"levenshtein_distance", strfry_levenshtein_distance, METH_VARARGS,
     "levenshtein_distance(string1, string2)\n\n"
     "Compute the Levenshtein distance between string1 and string2."},

    {"soundex", strfry_soundex, METH_VARARGS,
     "soundex(string)\n\n"
     "Calculate the soundex code for a given name."},

    {"metaphone", strfry_metaphone, METH_VARARGS,
     "metaphone(string)\n\n"
     "Calculate the metaphone representation of a given string."},

    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initstrfry(void)
{
    PyObject *unicodedata;
    unicodedata = PyImport_ImportModule("unicodedata");
    normalize = PyObject_GetAttrString(unicodedata, "normalize");
    Py_DECREF(unicodedata);

    (void)Py_InitModule("strfry", StrfryMethods);
}
