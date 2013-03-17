#include <Python.h>
#include <dconf.h>

static PyObject *DConfTypeError;
static DConfClient *client;

static PyObject *
tinydconf_list(PyObject *self, PyObject *args)
{
    PyObject *dir_list;
    gchar *dir;
    
    if(!PyArg_ParseTuple(args, "s", &dir))
        return NULL;

    gint size;
    gchar **cc;
    int i;
    cc = dconf_client_list(client, dir, &size);

    dir_list = PyList_New(size);
    if(!dir_list || PyErr_Occurred()) {
        Py_XDECREF(dir_list);
        return NULL;
    }
    Py_INCREF(dir_list);

    for (i = 0; i < size; i++) {
        if(PyList_SetItem(dir_list, i, PyString_FromString(cc[i])) ) {
            return NULL;
        }       
    }

    g_strfreev(cc);
    
    return dir_list;
}

static PyObject *
convert_g_variant(GVariant *gv)
{
    PyObject *value;
    if (!gv) {
        Py_INCREF(Py_None);
        return Py_None;
    }
    if (g_variant_is_of_type(gv, G_VARIANT_TYPE_BOOLEAN)) {
    
        value = PyBool_FromLong(g_variant_get_boolean(gv));
    
    } else if (g_variant_is_of_type(gv, G_VARIANT_TYPE_BYTE)) {
    
        value = PyInt_FromLong(g_variant_get_byte(gv)); 
    
    } else if (g_variant_is_of_type(gv, G_VARIANT_TYPE_INT16)) {
    
        value = PyInt_FromLong(g_variant_get_int16(gv));
    
    } else if (g_variant_is_of_type(gv, G_VARIANT_TYPE_UINT16)) {
    
        value = PyInt_FromLong(g_variant_get_uint16(gv));
    
    } else if (g_variant_is_of_type(gv, G_VARIANT_TYPE_INT32)) {
    
        value = PyInt_FromLong(g_variant_get_int32(gv));
    
    } else if (g_variant_is_of_type(gv, G_VARIANT_TYPE_UINT32)) {
    
        value = PyInt_FromLong(g_variant_get_uint32(gv));
    
    } else if (g_variant_is_of_type(gv, G_VARIANT_TYPE_INT64)) {
    
        value = PyInt_FromLong(g_variant_get_int64(gv));
    
    } else if (g_variant_is_of_type(gv, G_VARIANT_TYPE_UINT64)) {
    
        value = PyInt_FromLong(g_variant_get_uint64(gv));
    
    } else if (g_variant_is_of_type(gv, G_VARIANT_TYPE_HANDLE)) {
    
        value = PyInt_FromLong(g_variant_get_handle(gv));
    
    } else if (g_variant_is_of_type(gv, G_VARIANT_TYPE_DOUBLE)) {
    
        value = PyFloat_FromDouble(g_variant_get_double(gv));
    
    } else if (g_variant_is_of_type(gv, G_VARIANT_TYPE_STRING) ||
               g_variant_is_of_type(gv, G_VARIANT_TYPE_OBJECT_PATH) ||
               g_variant_is_of_type(gv, G_VARIANT_TYPE_SIGNATURE)) {
    
        value = PyUnicode_FromString(g_variant_get_string(gv, NULL));
    
    } else if (g_variant_is_of_type(gv, G_VARIANT_TYPE_ARRAY)) {
        
        gsize array_size = g_variant_n_children(gv);
        gsize i;
        PyObject *variant_list = PyList_New(array_size);
        
        if(!variant_list) {
            return NULL;
        }
        
        Py_INCREF(variant_list);
        
        for (i = 0; i < array_size; i++) {
            GVariant *gv_child;
            gv_child = g_variant_get_child_value(gv, i);
            PyObject *obj = convert_g_variant(gv_child);
            if (PyList_SetItem(variant_list, i, obj)) {
                return NULL;
            }
            g_variant_unref(gv_child);
        }
        value = variant_list;
    
    } else if (g_variant_is_of_type(gv, G_VARIANT_TYPE_TUPLE)) {
        gsize tuple_size = g_variant_n_children(gv);
        gsize i;
        PyObject *variant_tuple = PyTuple_New(tuple_size);

        if(!variant_tuple) {
            return NULL;
        }

        Py_INCREF(variant_tuple);

        for (i = 0; i < tuple_size; i++) {
            GVariant *gv_child;
            gv_child = g_variant_get_child_value(gv, i);
            PyObject *obj = convert_g_variant(gv_child);
            PyTuple_SET_ITEM(variant_tuple, i, obj);
            g_variant_unref(gv_child);
        }
        value = variant_tuple;
    
    } else {
        PyErr_SetString(DConfTypeError,
                        "Could not get value. Type not supported.");
        return NULL;
    }
    Py_INCREF(value);
    return value;
}

static PyObject *
tinydconf_read(PyObject *self, PyObject *args)
{
    gchar *key;
    PyObject *value;

    if(!PyArg_ParseTuple(args, "s", &key))
        return NULL;

    GVariant *gv = dconf_client_read(client, key);
    if (!gv) 
        return Py_None;

    value = convert_g_variant(gv);
    g_variant_unref(gv);
    Py_INCREF(value);
    
    return value;
}

static PyMethodDef DconfMethods[] = {
    {"list", tinydconf_list, METH_VARARGS,
        PyDoc_STR("list(path) -> list\n\n"
                   "List the contents of a dir")},
    {"read", tinydconf_read, METH_VARARGS,
        PyDoc_STR("read(path) -> value\n\n"
                   "Read the value of a key")},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC
inittinydconf(void)
{
    PyObject *m;
    
    m = Py_InitModule("tinydconf", DconfMethods);
    if (m == NULL)
        return;
    
    g_type_init();
    // client = dconf_client_new(NULL, dconf_watch_func, NULL, NULL);
    client = dconf_client_new();
    
    DConfTypeError = PyErr_NewException("tinydconf.DConfTypeError", NULL, NULL);
    Py_INCREF(DConfTypeError);
    PyModule_AddObject(m, "error", DConfTypeError);
}
