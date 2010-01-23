/* Copyright (c) Ben Smith, 2010 */
/* Skeleton taken from winlisp.c and winlisp.h */
/* NetHack may be freely redistributed.  See license for details. */
#ifndef WINPYTHON_H
#define WINPYTHON_H

/* I'm sorry, this is probably bad of me to do - it works around a multiple macro definition for yn 
  local and in mathcall.h
*/
#undef __USE_XOPEN
#undef __USE_MISC

#include "Python.h"

#define __USE_XOPEN
#define __USE_MISC

#ifndef E
#define E extern
#endif

#if defined(BOS) || defined(NHSTDC)
#define DIMENSION_P int
#else
# ifdef WIDENED_PROTOTYPES
#define DIMENSION_P unsigned int
# else
#define DIMENSION_P Dimension
# endif
#endif

char * python_script;
extern struct window_procs python_procs;

typedef struct {
    PyObject_HEAD
    /* nothing to see here */
    int takeUpSpace;
} nethack_NethackProcs;


PyObject *Py_init_nhwindows(PyObject *self, PyObject *args);
PyObject *Py_player_selection(PyObject *self, PyObject *args);
PyObject *Py_exit_nhwindows(PyObject *self, PyObject *args);
PyObject *Py_suspend_nhwindows(PyObject *self, PyObject *args);
PyObject *Py_resume_nhwindows(PyObject *self, PyObject *args);
PyObject *Py_create_nhwindow(PyObject *self, PyObject *args);
PyObject *Py_clear_nhwindow(PyObject *self, PyObject *args);
PyObject *Py_display_nhwindow(PyObject *self, PyObject *args);
PyObject *Py_destroy_nhwindow(PyObject *self, PyObject *args);
PyObject *Py_curs(PyObject *self, PyObject *args);
PyObject *Py_putstr(PyObject *self, PyObject *args); 
PyObject *Py_display_file(PyObject *self, PyObject *args);
PyObject *Py_start_menu(PyObject *self, PyObject *args);
PyObject *Py_add_menu(PyObject *self, PyObject *args);
PyObject *Py_end_menu(PyObject *self, PyObject *args);
PyObject *Py_select_menu(PyObject *self, PyObject *args);
PyObject *Py_update_inventory(PyObject *self, PyObject *args);
PyObject *Py_mark_synch(PyObject *self, PyObject *args);
PyObject *Py_wait_synch(PyObject *self, PyObject *args);
PyObject *Py_cliparound(PyObject *self, PyObject *args);
PyObject *Py_update_positionbar(PyObject *self, PyObject *args);
PyObject *Py_print_glyph(PyObject *self, PyObject *args);
PyObject *Py_raw_print(PyObject *self, PyObject *args);
PyObject *Py_raw_print_bold(PyObject *self, PyObject *args);
PyObject *Py_nhgetch(PyObject *self, PyObject *args);
PyObject *Py_nh_poskey(PyObject *self, PyObject *args);
PyObject *Py_nhbell(PyObject *self, PyObject *args);
PyObject *Py_doprev_message(PyObject *self, PyObject *args);
PyObject *Py_yn_function(PyObject *self, PyObject *args);
PyObject *Py_getlin(PyObject *self, PyObject *args);
PyObject *Py_get_ext_cmd(PyObject *self, PyObject *args);
PyObject *Py_number_pad(PyObject *self, PyObject *args);
PyObject *Py_delay_output(PyObject *self, PyObject *args);
PyObject *Py_start_screen(PyObject *self, PyObject *args);
PyObject *Py_end_screen(PyObject *self, PyObject *args);
PyObject *Py_outrip(PyObject *self, PyObject *args);
PyObject *Py_get_nh_event(PyObject *self, PyObject *args);
PyObject *Py_askname(PyObject *self, PyObject *args);

static PyMethodDef nethack_methods[] = {
    {"init_nhwindows", (PyCFunction)Py_init_nhwindows, METH_VARARGS,
        "init_nhwindows"},
    {"exit_nhwindows", (PyCFunction)Py_exit_nhwindows, METH_VARARGS,
        "exit_nhwindows"},
    {"suspend_nhwindows", (PyCFunction)Py_suspend_nhwindows, METH_VARARGS,
        "suspend_nhwindows"},
    {"askname", (PyCFunction)Py_askname, METH_VARARGS,
        "askname"},
	
    {"resume_nhwindows", (PyCFunction)Py_resume_nhwindows, METH_VARARGS,
        "resume_nhwindows"},
    {"create_nhwindow", (PyCFunction)Py_create_nhwindow, METH_VARARGS,
        "create_nhwindow"},
    {"clear_nhwindow", (PyCFunction)Py_clear_nhwindow, METH_VARARGS,
        "clear_nhwindow"},
    {"display_nhwindow", (PyCFunction)Py_display_nhwindow, METH_VARARGS,
        "display_nhwindow"},
    {"destroy_nhwindow", (PyCFunction)Py_destroy_nhwindow, METH_VARARGS,
        "destroy_nhwindow"},
    {"curs", (PyCFunction)Py_curs, METH_VARARGS,
        "curs"},
    {"putstr", (PyCFunction)Py_putstr, METH_VARARGS,
        "putstr"},
    {"display_file", (PyCFunction)Py_display_file, METH_VARARGS,
        "display_file"},
    {"start_menu", (PyCFunction)Py_start_menu, METH_VARARGS,
        "start_menu"},
    {"add_menu", (PyCFunction)Py_add_menu, METH_VARARGS,
        "add_menu"},
    {"end_menu", (PyCFunction)Py_end_menu, METH_VARARGS,
        "end_menu"},
    {"select_menu", (PyCFunction)Py_select_menu, METH_VARARGS,
        "select_menu"},
    {"update_inventory", (PyCFunction)Py_update_inventory, METH_VARARGS,
        "update_inventory"},
    {"mark_synch", (PyCFunction)Py_mark_synch, METH_VARARGS,
        "mark_synch"},
    {"wait_synch", (PyCFunction)Py_wait_synch, METH_VARARGS,
        "wait_synch"},
    {"cliparound", (PyCFunction)Py_cliparound, METH_VARARGS,
        "cliparound"},
    {"update_positionbar", (PyCFunction)Py_update_positionbar, METH_VARARGS,
        "update_positionbar"},
    {"print_glyph", (PyCFunction)Py_print_glyph, METH_VARARGS,
        "print_glyph"},
    {"raw_print", (PyCFunction)Py_raw_print, METH_VARARGS,
        "raw_print"},
    {"raw_print_bold", (PyCFunction)Py_raw_print_bold, METH_VARARGS,
        "raw_print_bold"},
    {"nhgetch", (PyCFunction)Py_nhgetch, METH_VARARGS,
       	"nhgetch"},
    {"nh_poskey", (PyCFunction)Py_nh_poskey, METH_VARARGS,
        "nh_poskey"},
    {"nhbell", (PyCFunction)Py_nhbell, METH_VARARGS,
        "nhbell"},
    {"doprev_message", (PyCFunction)Py_doprev_message, METH_VARARGS,
        "doprev_message"},
    {"yn_function", (PyCFunction)Py_yn_function, METH_VARARGS,
        "yn_function"},
    {"getlin", (PyCFunction)Py_getlin, METH_VARARGS,
        "getlin"},
    {"get_ext_cmd", (PyCFunction)Py_get_ext_cmd, METH_VARARGS,
        "get_ext_cmd"},
    {"number_pad", (PyCFunction)Py_number_pad, METH_VARARGS,
        "number_pad"},
    {"delay_output", (PyCFunction)Py_delay_output, METH_VARARGS,
        "delay_output"},
    {"start_screen", (PyCFunction)Py_start_screen, METH_VARARGS,
        "start_screen"},
    {"end_screen", (PyCFunction)Py_end_screen, METH_VARARGS,
        "end_screen"},
    {"outrip", (PyCFunction)Py_outrip, METH_VARARGS,
        "outrip"},
    {"player_selection", (PyCFunction)Py_player_selection, METH_VARARGS,
	"player_selection"},
    {"get_nh_event", (PyCFunction)Py_get_nh_event, METH_VARARGS,
	"get_nh_event"},
    {NULL}  /* Sentinel */
};

static PyTypeObject nethack_NethackProcType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "nethack.NethackProcs",    /*tp_name*/
    sizeof(nethack_NethackProcs), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
    "Abstract implementation of NetHack callback functions.",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    nethack_methods,             /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    0,                 /* tp_new */
};

PyObject *set_interface(PyObject *self, PyObject *args);
PyObject *check_player_setup(PyObject *self, PyObject *args);
PyObject *get_interface(PyObject *self, PyObject *args);

static PyMethodDef PyNhMethods[] = {
    {"set_interface", set_interface, METH_VARARGS,
    "Set the instance of the class derived from NethackProcs that you want to use to control NetHack."},
    {"check_player_setup", check_player_setup, METH_VARARGS,
     "check and see that role/race/gender/alignment are legitimate values."},
    {"get_interface", get_interface, METH_VARARGS,
    "get the instance of the class derived from NethackProcs that's controlling NetHack."},
    {NULL, NULL, 0, NULL}
};
        
/* ### winpython.c ### */
E void NDECL(win_python_init);
E void FDECL(python_init_nhwindows, (int *, char **));
E void NDECL(python_player_selection);
E void NDECL(python_askname);
E void NDECL(python_get_nh_event) ;
E void FDECL(python_exit_nhwindows, (const char *));
E void FDECL(python_suspend_nhwindows, (const char *));
E void NDECL(python_resume_nhwindows);
E winid FDECL(python_create_nhwindow, (int));
E void FDECL(python_clear_nhwindow, (winid));
E void FDECL(python_display_nhwindow, (winid, BOOLEAN_P));
E void FDECL(python_destroy_nhwindow, (winid));
E void FDECL(python_curs, (winid,int,int));
E void FDECL(python_putstr, (winid, int, const char *));
E void FDECL(python_display_file, (const char *, BOOLEAN_P));
E void FDECL(python_start_menu, (winid));
E void FDECL(python_add_menu, (winid,int,const ANY_P *,
			CHAR_P, CHAR_P, int, const char *, BOOLEAN_P));
E void FDECL(python_end_menu, (winid, const char *));
E int FDECL(python_select_menu, (winid, int, MENU_ITEM_P **));
E char FDECL(python_message_menu, (char, int, const char *mesg));
E void NDECL(python_update_inventory);
E void NDECL(python_mark_synch);
E void NDECL(python_wait_synch);
#ifdef CLIPPING
E void FDECL(python_cliparound, (int, int));
#endif
#ifdef POSITIONBAR
E void FDECL(python_update_positionbar, (char *));
#endif
E void FDECL(python_print_glyph, (winid,XCHAR_P,XCHAR_P,int));
E void FDECL(python_raw_print, (const char *));
E void FDECL(python_raw_print_bold, (const char *));
E int NDECL(python_nhgetch);
E int FDECL(python_nh_poskey, (int *, int *, int *));
E void NDECL(python_nhbell);
E int NDECL(python_doprev_message);
E char FDECL(python_yn_function, (const char *, const char *, CHAR_P));
E void FDECL(python_getlin, (const char *,char *));
E int NDECL(python_get_ext_cmd);
E void FDECL(python_number_pad, (int));
E void NDECL(python_delay_output);

/* other defs that really should go away (they're tty specific) */
E void NDECL(python_start_screen);
E void NDECL(python_end_screen);

E void FDECL(python_outrip, (winid,int));

#endif /* WINPYTHON_H */
