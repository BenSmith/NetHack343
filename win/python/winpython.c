/* Copyright (c) Ben Smith, 2010 */
/* Skeleton taken from winlisp.c and winlisp.h */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "winpython.h"

#include "decl.h" // for plname
#include "flag.h" // for player_selection



/* from tile.c */
extern short glyph2tile[];
extern int total_tiles_used;
int PYTHON_INITIALIZED = 0;

PyObject *interface_object = NULL;

struct window_procs python_procs = {
	"python",
	WC_COLOR|WC_HILITE_PET,
	0L,
	python_init_nhwindows,
	python_player_selection,
	python_askname,
	python_get_nh_event,
	python_exit_nhwindows,
	python_suspend_nhwindows,
	python_resume_nhwindows,
	python_create_nhwindow,
	python_clear_nhwindow,
	python_display_nhwindow,
	python_destroy_nhwindow,
	python_curs,
	python_putstr,
	python_display_file,
	python_start_menu,
	python_add_menu,
	python_end_menu,
	python_select_menu,

/*
char message_menu(char let, int how, const char *mesg)
    -- tty-specific hack to allow single line context-sensitive
        help to behave compatibly with multi-line help menus.
    -- This should only be called when a prompt is active; it
        sends `mesg' to the message window.  For tty, it forces
        a --More-- prompt and enables `let' as a viable keystroke
        for dismissing that prompt, so that the original prompt
        can be answered from the message line "help menu".
    -- Return value is either `let', '\0' (no selection was made),
        or '\033' (explicit cancellation was requested).
    -- Interfaces which issue prompts and messages to separate
        windows typically won't need this functionality, so can
        substitute genl_message_menu (windows.c) instead.
*/
/* TODO: Provide python interface. */
        genl_message_menu,
	python_update_inventory,
	python_mark_synch,
	python_wait_synch,
#ifdef CLIPPING
	python_cliparound,
#endif
#ifdef POSITIONBAR
	python_update_positionbar,
#endif
	python_print_glyph,
	python_raw_print,
	python_raw_print_bold,
	python_nhgetch,
	python_nh_poskey,
	python_nhbell,
	python_doprev_message,
	python_yn_function,
	python_getlin,
	python_get_ext_cmd,
	python_number_pad,
	python_delay_output,
#ifdef CHANGE_COLOR	/* the Mac uses a palette device */
	donull,/* TODO: Provide python interface. */
	donull,/* TODO: Provide python interface. */
#endif
	python_start_screen,
	python_end_screen,
	python_outrip,
/*
preference_update(preference)
    -- The player has just changed one of the wincap preference
        settings, and the NetHack core is notifying your window
        port of that change.  If your window-port is capable of
        dynamically adjusting to the change then it should do so.
        Your window-port will only be notified of a particular
        change if it indicated that it wants to be by setting the 
        corresponding bit in the wincap mask.
*/
/* TODO: Provide python interface. */
	genl_preference_update,
};

PyObject *
set_interface(PyObject *self, PyObject *args)
{
    if (interface_object)
    {
        Py_DECREF(interface_object);
    }
    if (!PyArg_ParseTuple(args, "O:interface_object", &interface_object))
    {
        return (NULL);
    }
    if (!interface_object)
    {
        return (NULL);
    }
    Py_INCREF(interface_object);
}

PyObject *
get_interface(PyObject *self, PyObject *args)
{
    return (interface_object);
}

PyObject *
check_player_setup(PyObject *self, PyObject *args)
{
    int	 initrole = 0;	/* starting role      (index into roles[])   */
    int	 initrace = 0;	/* starting race      (index into races[])   */
    int	 initgend = 0;	/* starting gender    (index into genders[]) */
    int	 initalign = 0;	/* starting alignment (index into aligns[])  */
    int	 randomall = 0;	/* randomly assign everything not specified */
    int	 pantheon = 0;	/* deity selection for priest character */
    
    if (!PyArg_ParseTuple(args, "iiiiii", &initrole, &initrace, &initgend, &initalign, &randomall, &pantheon))
    {
        PyErr_SetString(PyExc_TypeError, "check_player_setup accepts only a tuple of 6 integers");
	Py_RETURN_FALSE;
    }
    if (!ok_role(initrole, initrace, initgend, initalign))
    {
        Py_RETURN_FALSE;
    }
    if (!ok_race(initrole, initrace, initgend, initalign))
    {
        Py_RETURN_FALSE;
    }
    if (!ok_gend(initrole, initrace, initgend, initalign))
    {
        Py_RETURN_FALSE;
    }
    if (!ok_align(initrole, initrace, initgend, initalign))
    {
        Py_RETURN_FALSE;
    }
    
    Py_RETURN_TRUE;
}

void
win_python_init()
{
    PyObject *m = NULL;
    Py_Initialize();
    nethack_NethackProcType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&nethack_NethackProcType) < 0)
        return;
    m = Py_InitModule("nethack", PyNhMethods);
    if (!m)
        return;
    Py_INCREF(&nethack_NethackProcType);
    
    PyModule_AddObject(m, "NethackProcs", (PyObject *)&nethack_NethackProcType);

    PYTHON_INITIALIZED = 1;
}

void
win_python_finish()
{
    if (PYTHON_INITIALIZED)
    {
        if (interface_object)
            Py_DECREF(interface_object);
        Py_Finalize();
        PYTHON_INITIALIZED = 0;
    }
    if (python_script)
    {
	free(python_script);
    }
}

/*
player_selection()
    -- Do a window-port specific player type selection.  If
        player_selection() offers a Quit option, it is its
        responsibility to clean up and terminate the process.
        You need to fill in pl_character[0].
*/
void
python_player_selection()
{
    PyObject *rv = NULL;

    /* KMH, role patch -- Variables used during startup.
    *
    * If the user wishes to select a role, race, gender, and/or alignment
    * during startup, the choices should be recorded here.  This
    * might be specified through command-line options, environmental
    * variables, a popup dialog box, menus, etc.
    *
    * These values are each an index into an array.  They are not
    * characters or letters, because that limits us to 26 roles.
    * They are not booleans, because someday someone may need a neuter
    * gender.  Negative values are used to indicate that the user
    * hasn't yet specified that particular value.	If you determine
    * that the user wants a random choice, then you should set an
    * appropriate random value; if you just left the negative value,
    * the user would be asked again!
    *
    * These variables are stored here because the u structure is
    * cleared during character initialization, and because the
    * flags structure is restored for saved games.  Thus, we can
    * use the same parameters to build the role entry for both
    * new and restored games.
    *
    * These variables should not be referred to after the character
    * is initialized or restored (specifically, after role_init()
    * is called).
    */
    int	 initrole;	/* starting role      (index into roles[])   */
    int	 initrace;	/* starting race      (index into races[])   */
    int	 initgend;	/* starting gender    (index into genders[]) */
    int	 initalign;	/* starting alignment (index into aligns[])  */
    int	 randomall;	/* randomly assign everything not specified */
    int	 pantheon;	/* deity selection for priest character */
    rv = PyObject_CallMethod(interface_object, "player_selection", "iiiiii", initrole, initrace, initgend, initalign, randomall, pantheon);
    if (!rv)
    {
        PyErr_SetString(PyExc_AttributeError, "method player_selection failed\n");
	return;
    }
    if (!PyArg_ParseTuple(rv, "iiiiii", &initrole, &initrace, &initgend, &initalign, &randomall, &pantheon))
        PyErr_SetString(PyExc_TypeError, "incorrect type returned from player_selection method");
}

/*
askname()
    -- Ask the user for a player name.
*/
void
python_askname()
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "askname", NULL);
    if (!rv)
    {
        PyErr_SetString(PyExc_AttributeError, "method askname failed\n");
	return;
    }
    else
    {
        strncpy(plname, PyString_AsString(rv), PL_NSIZ - 1);
        if (plname == NULL)
        {
            PyErr_SetString(PyExc_ValueError, "Player name must not be null");
	    return;
        }
        plname[PL_NSIZ - 1] = '\0';
    }
}

/*
get_nh_event()
    -- Does window event processing (e.g. exposure events).
        A noop for the tty and X window-port.
*/
void
python_get_nh_event()
{
    PyObject *rv = NULL;
    
    rv = PyObject_CallMethod(interface_object, "get_nh_event", NULL);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method get_nh_event failed\n");
}

/*
raw_print(str)
    -- Print directly to a screen, or otherwise guarantee that
        the user sees str.  raw_print() appends a newline to str.
        It need not recognize ASCII control characters.  This is
        used during startup (before windowing system initialization
    -- maybe this means only error startup messages are raw),
        for error messages, and maybe other "msg" uses.  E.g.
        updating status for micros (i.e, "saving").
*/
void
python_raw_print(const char *str)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "raw_print", "s", str);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method raw_print failed\n");
}

/*
raw_print_bold(str)

    Like raw_print(), but prints in bold/standout (if possible).
*/
void
python_raw_print_bold(const char *str)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "raw_print_bold", "s", str);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method raw_print_bold failed\n");
}

/*
curs(window, x, y)
    -- Next output to window will start at (x,y), also moves
        displayable cursor to (x,y).  For backward compatibility,
        1 <= x < cols, 0 <= y < rows, where cols and rows are
        the size of window.
    -- For variable sized windows, like the status window, the
        behavior when curs() is called outside the window's limits
        is unspecified. The mac port wraps to 0, with the status
        window being 2 lines high and 80 columns wide.
    -- Still used by curs_on_u(), status updates, screen locating
        (identify, teleport).
    -- NHW_MESSAGE, NHW_MENU and NHW_TEXT windows do not
        currently support curs in the tty window-port.
*/
void
python_curs(winid window, int x, int y)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "curs", "iii", window, x, y);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method curs failed\n");
}

/*
putstr(window, attr, str)
    -- Print str on the window with the given attribute.  Only
        printable ASCII characters (040-0126) must be supported.
        Multiple putstr()s are output on separate lines.  Attributes
        can be one of
            ATR_NONE (or 0)
            ATR_ULINE
            ATR_BOLD
            ATR_BLINK
            ATR_INVERSE
        If a window-port does not support all of these, it may map
        unsupported attributes to a supported one (e.g. map them
        all to ATR_INVERSE).  putstr() may compress spaces out of
        str, break str, or truncate str, if necessary for the
        display.  Where putstr() breaks a line, it has to clear
        to end-of-line.
    -- putstr should be implemented such that if two putstr()s
        are done consecutively the user will see the first and
        then the second.  In the tty port, pline() achieves this
        by calling more() or displaying both on the same line.'
*/
void
python_putstr(winid window, int attr, const char *str)
{
    PyObject *rv;

    rv = PyObject_CallMethod(interface_object, "putstr", "iis", window, attr, str);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method putstr failed\n");
}

/*
start_menu(window)
    -- Start using window as a menu.  You must call start_menu()
        before add_menu().  After calling start_menu() you may not
        putstr() to the window.  Only windows of type NHW_MENU may
        be used for menus.
*/
void
python_start_menu(winid window)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "start_menu", "i", window);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method start_menu failed\n");
}

/*
add_menu(windid window, int glyph, const anything identifier,
            char accelerator, char groupacc,
            int attr, char *str, boolean preselected)
    -- Add a text line str to the given menu window.  If identifier
        is 0, then the line cannot be selected (e.g. a title).
        Otherwise, identifier is the value returned if the line is
        selected.  Accelerator is a keyboard key that can be used
        to select the line.  If the accelerator of a selectable
        item is 0, the window system is free to select its own
        accelerator.  It is up to the window-port to make the
        accelerator visible to the user (e.g. put "a - " in front
        of str).  The value attr is the same as in putstr().
        Glyph is an optional glyph to accompany the line.  If
        window port cannot or does not want to display it, this
        is OK.  If there is no glyph applicable, then this
        value will be NO_GLYPH.
    -- All accelerators should be in the range [A-Za-z],
        but there are a few exceptions such as the tty player
        selection code which uses '*'.
    -- It is expected that callers do not mix accelerator
        choices.  Either all selectable items have an accelerator
        or let the window system pick them.  Don't do both.
    -- Groupacc is a group accelerator.  It may be any character
        outside of the standard accelerator (see above) or a
        number.  If 0, the item is unaffected by any group
        accelerator.  If this accelerator conflicts with
        the menu command (or their user defined alises), it loses.
        The menu commands and aliases take care not to interfere
        with the default object class symbols.
    -- If you want this choice to be preselected when the
        menu is displayed, set preselected to TRUE.
*/

void
python_add_menu(winid window, int glyph, const ANY_P *identifier, CHAR_P accelerator, CHAR_P groupacc, int attr, const char *str, BOOLEAN_P preselected)
{

    PyObject *rv = NULL;
    PyCObject *id = (PyObject *)PyCObject_FromVoidPtr((void *)identifier, NULL);
    if (!id)
    {
	PyErr_SetString(PyExc_ValueError, "couldn't make a PyCObject from identifier in add_menu");
	return;
    }
    
#ifdef UNWIDENED_PROTOTYPES
#ifndef SKIP_BOOLEAN
    char format[] = "iiOccisc";
#else
    char format[] = "iiOccisi";
#endif /* UNWIDENED_PROTOTYPES */
#else
#ifdef WIDENED_PROTOTYPES
    char format[] = "iiOiiisi";
#endif /* WIDENED_PROTOTYPES */
#endif /* not UNWIDENED_PROTOTYPES */

    rv = PyObject_CallMethod(interface_object, "add_menu", format, window, glyph, id, accelerator, groupacc, attr, str, preselected);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method add_menu failed\n");
}

/*
end_menu(window, prompt)
    -- Stop adding entries to the menu and flushes the window
        to the screen (brings to front?).  Prompt is a prompt
        to give the user.  If prompt is NULL, no prompt will
        be printed.
    ** This probably shouldn't flush the window any more (if
    ** it ever did).  That should be select_menu's job.  -dean
*/
void
python_end_menu(winid window, const char *prompt)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "end_menu", "is", window, prompt);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method end_menu failed\n");
}

/*
int select_menu(windid window, int how, menu_item **selected)
    -- Return the number of items selected; 0 if none were chosen,
        -1 when explicitly cancelled.  If items were selected, then
        selected is filled in with an allocated array of menu_item
        structures, one for each selected line.  The caller must
        free this array when done with it.  The "count" field
        of selected is a user supplied count.  If the user did
        not supply a count, then the count field is filled with
        -1 (meaning all).  A count of zero is equivalent to not
        being selected and should not be in the list.  If no items
        were selected, then selected is NULL'ed out.  How is the
        mode of the menu.  Three valid values are PICK_NONE,
        PICK_ONE, and PICK_ANY, meaning: nothing is selectable,
        only one thing is selectable, and any number valid items
        may selected.  If how is PICK_NONE, this function should
        never return anything but 0 or -1.
    -- You may call select_menu() on a window multiple times --
        the menu is saved until start_menu() or destroy_nhwindow()
        is called on the window.
    -- Note that NHW_MENU windows need not have select_menu()
        called for them. There is no way of knowing whether
        select_menu() will be called for the window at
        create_nhwindow() time.
*/
int
python_select_menu(winid window, int how, MENU_ITEM_P **menu_list)
{
    PyObject *rv = NULL, *dict = NULL, *key = NULL, *value = NULL;
    int count = 0, i = 0;
    Py_ssize_t pos = 0;
    anything *a = NULL;
	    
    rv = PyObject_CallMethod(interface_object, "select_menu", "ii", window, how);

    if (!rv)
    {
	PyErr_SetString(PyExc_AttributeError, "method select_menu failed\n");
	return (0);
    }
    
    if (!PyArg_ParseTuple(rv, "O", dict))
    {
	PyErr_SetString(PyExc_TypeError, "method select_menu expects a python dict or None for a return value.");
	return (0);
    }
    if (dict == Py_None)
    {
	return (0);
    }
    else if (PyDict_Check(dict))
    {
	count = (int)PyDict_Size(dict);
	if (count == 0)
	{
	    return (0);
	}
	
	if ((how == PICK_NONE) && count > 0)
	{
	    PyErr_SetString(PyExc_ValueError, "method select_menu should not return a populated dict when 'how' is PICK_NONE");
	    return (0);
	}
	if ((how == PICK_ONE) && count > 1)
	{
	    PyErr_SetString(PyExc_ValueError, "method select_menu should return a dict with only one key:value pair when 'how' is PICK_ONE");
	    return (0);
	}
	menu_item *list = (menu_item *)malloc(sizeof (menu_item) * count);
	if (!list)
	{
	    PyErr_SetString(PyExc_MemoryError, "method select_menu unable to allocate memory for menu items.");
	    return (0);
	}
	i = 0;
	menu_list = &list;
	while (PyDict_Next(dict, &pos, &key, &value))
	{
	    /* Populate menu_list from dict */
	    list[i].item = (anything)PyCObject_AsVoidPtr(key);
	    if (value == Py_None)
		list[i].count = -1;
	    else
		list[i].count = PyInt_AsLong(value);
	    i++;
	}
	return (count);
    }
    else
    {
	PyErr_SetString(PyExc_TypeError, "method select_menu expects a python dict or python integer for a return value.");
	return (0);
    }
    
    return (0);
}

/*
int nh_poskey(int *x, int *y, int *mod)
    -- Returns a single character input from the user or a
        a positioning event (perhaps from a mouse).  If the
        return value is non-zero, a character was typed, else,
        a position in the MAP window is returned in x, y and mod.
        mod may be one of

            CLICK_1		 mouse click type 1 
            CLICK_2		 mouse click type 2 

        The different click types can map to whatever the
        hardware supports.  If no mouse is supported, this
        routine always returns a non-zero character.
*/
int
python_nh_poskey(int *x, int *y, int *mod)
{
    PyObject *rv = NULL;
    char ch = 0;
    
    rv = PyObject_CallMethod(interface_object, "nh_poskey", "iiii", ch, x, y, mod);
    
    if (!rv)
    {
        PyErr_SetString(PyExc_AttributeError, "method nh_poskey failed\n");
	return (0);
    }
    if (!PyArg_ParseTuple(rv, "ciii", &ch, &x, &y, &mod))
    {
	PyErr_SetString(PyExc_TypeError, "return type from nh_poskey is not (char ch, int x, int y, int mod)");
    }
    return ((int)ch);
}

/*
window = create_nhwindow(type)
    -- Create a window of type "type."
*/
winid
python_create_nhwindow(int type)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "create_nhwindow", "i", type);
    if (!rv)
    {
        PyErr_SetString(PyExc_AttributeError, "method create_nhwindow failed\n");
	return (-1);
    }
    if (PyInt_AsLong(rv) == -1)
    {
	if (PyErr_Occurred())
	{
	    PyErr_SetString(PyExc_TypeError, "get_ext_cmd must return an integer");
	}
	return (-1);
    }
    else
	return (PyInt_AsLong(rv));
}
/*
clear_nhwindow(window)
    -- Clear the given window, when appropriate.
*/
void
python_clear_nhwindow(winid window)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "clear_nhwindow", "i", window);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method clear_nhwindow failed\n");
}

/*
display_nhwindow(window, boolean blocking)
    -- Display the window on the screen.  If there is data
        pending for output in that window, it should be sent.
        If blocking is TRUE, display_nhwindow() will not
        return until the data has been displayed on the screen,
        and acknowledged by the user where appropriate.
    -- All calls are blocking in the tty window-port.
    -- Calling display_nhwindow(WIN_MESSAGE,???) will do a
        --more--, if necessary, in the tty window-port.
*/
void
python_display_nhwindow(winid window, int blocking)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "display_nhwindow", "ib", window, blocking == 1);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method display_nhwindow failed\n");
}

/*
destroy_nhwindow(window)
		-- Destroy will dismiss the window if the window has not
		   already been dismissed.
*/
void
python_destroy_nhwindow(winid window)
{
    PyObject *rv = NULL;
    
    rv = PyObject_CallMethod(interface_object, "destroy_nhwindow", "i", window);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method destroy_nhwindow failed\n");
}

/*
update_inventory()
    -- Indicate to the window port that the inventory has been
        changed.
    -- Merely calls display_inventory() for window-ports that
        leave the window up, otherwise empty.
*/

void
python_update_inventory()
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "update_inventory", NULL);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method update_inventory failed\n");
}

/*
doprev_message()
    -- Display previous messages.  Used by the ^P command.
    -- On the tty-port this scrolls WIN_MESSAGE back one line.
*/
int
python_doprev_message()
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "doprev_message", NULL);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method doprev_message failed\n");
}

/*
nhbell()
    -- Beep at user.  [This will exist at least until sounds are
        redone, since sounds aren't attributable to windows anyway.]
*/
void
python_nhbell()
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "nhbell", NULL);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method nhbell failed\n");
}

/*
mark_synch()
    -- Don't go beyond this point in I/O on any channel until
        all channels are caught up to here.  Can be an empty call
        for the moment
*/
void
python_mark_synch()
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "mark_synch", NULL);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method mark_synch failed\n");
}

/*
wait_synch()
    -- Wait until all pending output is complete (*flush*() for
        streams goes here).
    -- May also deal with exposure events etc. so that the
        display is OK when return from wait_synch().
*/
void
python_wait_synch()
{
    PyObject *rv = NULL;
    rv = PyObject_CallMethod(interface_object, "wait_synch", NULL);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method wait_synch failed\n");
}

/*
resume_nhwindows()
    -- Restore the windows after being suspended.
*/
void
python_resume_nhwindows()
{
    PyObject *rv = NULL;
    
    rv = PyObject_CallMethod(interface_object, "resume_nhwindows", NULL);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method resume_nhwindows failed\n");
}

/*
suspend_nhwindows(str)
        -- Prepare the window to be suspended.
*/
void
python_suspend_nhwindows(const char *str)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "suspend_nhwindows", "s", str);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method suspend_nhwindows failed\n");
}

/*
number_pad(state)
    -- Initialize the number pad to the given state.
*/
void
python_number_pad(int state)
{
    PyObject *rv = NULL;
    rv = PyObject_CallMethod(interface_object, "number_pad", "i", state);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method number_pad failed\n");
}

/*
init_nhwindows(int* argcp, char** argv)
    -- Initialize the windows used by NetHack.  This can also
        create the standard windows listed at the top, but does
        not display them.
    -- Any commandline arguments relevant to the windowport
        should be interpreted, and *argcp and *argv should
        be changed to remove those arguments.
    -- When the message window is created, the variable
        iflags.window_inited needs to be set to TRUE.  Otherwise
        all plines() will be done via raw_print().
    ** Why not have init_nhwindows() create all of the "standard"
    ** windows?  Or at least all but WIN_INFO?	-dean
*/
void
python_init_nhwindows(int *argcp, char **argv)
{
    PyObject *rv = NULL, *arglist = NULL;
    int i = 0;

    /* look for something from the options file telling us which file to load
       or look on the command line */
    FILE *f = fopen(python_script, "r");
    if (!f)
    {
	PyErr_SetString(PyExc_RuntimeError, "Unable to open python_script file");
	return;
    }

    int k = PyRun_SimpleFileEx(f, python_script, 1);
    if (k == -1)
    {
        PyErr_SetString(PyExc_RuntimeError, "An error occurred while executing startup script");
	return;
    }
    arglist = PyList_New(0);
    for (i = 0; i < *argcp; i++)
    {
        PyList_Append(arglist, PyString_FromString(argv[i]));
    }
    rv = NULL;
    rv = PyObject_CallMethod(interface_object, "init_nhwindows", "O", arglist);
    if (!rv)
    {
        PyErr_SetString(PyExc_AttributeError, "method init_nhwindows failed\n");    
	return;
    }
    if (!PyList_Check(rv))
    {
        PyErr_SetString(PyExc_TypeError, "method init_nhwindows must return a list of strings.");
	return;
    }
    if (!interface_object)
    {
	PyRun_SimpleString("import nethack");
	PyRun_SimpleString("nethack.set_interface(nethack.NethackProcs())\n");
    }
    /* This should be more robust - i.e. check that argv[0] remains
       the same and that argcp is equal or less than what it was.
       This implementation could cause buffer overflows.
       Also, changing the order of the list is probably unwise.
    */

    *argcp = PyList_Size(rv);
    for (i = 0; i < *argcp; i++)
    {
        argv[i] = PyString_AsString(PyList_GetItem(rv, i));
    }
}

/*
exit_nhwindows(str)
    -- Exits the window system.  This should dismiss all windows,
        except the "window" used for raw_print().  str is printed
        if possible.
*/
void
python_exit_nhwindows (const char *str)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "exit_nhwindows", "s", str);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method exit_nhwindows failed\n");
}

/*
delay_output()	
    -- Causes a visible delay of 50ms in the output.
        Conceptually, this is similar to wait_synch() followed
        by a nap(50ms), but allows asynchronous operation.
*/
void
python_delay_output()
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "delay_output", NULL);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method delay_output failed\n");
}

/*
getlin(const char *ques, char *input)
    -- Prints ques as a prompt and reads a single line of text,
        up to a newline.  The string entered is returned without the
        newline.  ESC is used to cancel, in which case the string
        "\033\000" is returned.
    -- getlin() must call flush_screen(1) before doing anything.
    -- This uses the top line in the tty window-port, other
        ports might use a popup.
    -- getlin() can assume the input buffer is at least BUFSZ
        bytes in size and must truncate inputs to fit, including
        the nul character.
*/
void
python_getlin(const char *question, char *input)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "getlin", "s", question);
    if (!rv)
    {
        PyErr_SetString(PyExc_AttributeError, "method getlin failed\n");
	return;
    }
    if (PyString_Check(rv))
    {
      strncpy(input, PyString_AsString(rv), BUFSZ - 1);
      input[BUFSZ - 1] = '\0';
    }
    else
    {
	PyErr_SetString(PyExc_TypeError, "getlin must return a string");
	return;
    }
}

/*
int get_ext_cmd(void)
    -- Get an extended command in a window-port specific way.
        An index into extcmdlist[] is returned on a successful
        selection, -1 otherwise.
*/
int
python_get_ext_cmd()
{
    PyObject *rv = NULL;
    rv = PyObject_CallMethod(interface_object, "get_ext_cmd", NULL);
    if (!rv)
    {
        PyErr_SetString(PyExc_AttributeError, "method get_ext_cmd failed\n");
	return (-1);
    }
    if (PyInt_AsLong(rv) == -1)
    {
      if (PyErr_Occurred())
      {
	  PyErr_SetString(PyExc_TypeError, "get_ext_cmd must return an integer");
	  return (-1);
      }
      else
	return (PyInt_AsLong(rv));
    }
    else
	return (PyInt_AsLong(rv));
}

/*
display_file(str, boolean complain)
    -- Display the file named str.  Complain about missing files
        iff complain is TRUE.
*/
void
python_display_file(const char *str, BOOLEAN_P complain)
{
    PyObject *rv = NULL;
#ifdef UNWIDENED_PROTOTYPES
#ifndef SKIP_BOOLEAN
    char format[] = "sc";
#else
    char format[] = "si";
#endif /* SKIP_BOOLEAN */
#else
#ifdef WIDENED_PROTOTYPES
    char format[] = "si";
#endif /* WIDENED_PROTOTYPES */
#endif /* not UNWIDENED_PROTOTYPES */

    rv = PyObject_CallMethod(interface_object, "display_file", format, str, complain);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method display_file failed\n");
}

/*
char yn_function(const char *ques, const char *choices, char default)
    -- Print a prompt made up of ques, choices and default.
        Read a single character response that is contained in
        choices or default.  If choices is NULL, all possible
        inputs are accepted and returned.  This overrides
        everything else.  The choices are expected to be in
        lower case.  Entering ESC always maps to 'q', or 'n',
        in that order, if present in choices, otherwise it maps
        to default.  Entering any other quit character (SPACE,
        RETURN, NEWLINE) maps to default.
    -- If the choices string contains ESC, then anything after
        it is an acceptable response, but the ESC and whatever
        follows is not included in the prompt.
    -- If the choices string contains a '#' then accept a count.
        Place this value in the global "yn_number" and return '#'.
    -- This uses the top line in the tty window-port, other
        ports might use a popup.
    -- If choices is NULL, all possible inputs are accepted and
        returned, preserving case (upper or lower.) This means that
        if the calling function needs an exact match, it must handle
        user input correctness itself.
*/
char
python_yn_function(const char *ques, const char *choices, CHAR_P def)
{
    PyObject *rv = NULL;
    char choice = '\0';
 
#ifdef UNWIDENED_PROTOTYPES
    char format[] = "ssc";
#else
#ifdef WIDENED_PROTOTYPES
    char format[] = "ssi";
#endif /* WIDENED_PROTOTYPES */
#endif /* not UNWIDENED_PROTOTYPES */

    if (!choices)
    {
      format[1] = 'O';
      Py_INCREF(Py_None);
      rv = PyObject_CallMethod(interface_object, "yn_function", format, ques, Py_None, def);
      Py_DECREF(Py_None);
    }
    else
    {
	rv = PyObject_CallMethod(interface_object, "yn_function", format, ques, choices, def);
    }
    if (!rv)
    {
        PyErr_SetString(PyExc_AttributeError, "method yn_function failed\n");
	return ('\0');
    }
    if (!PyString_Check(rv) || !PyString_AsString(rv) || PyString_Size(rv) != 1)
    {
        PyErr_SetString(PyExc_TypeError, "method yn_function expects a single character returned");
	return ('\0');
    }
    strncpy(&choice, PyString_AsString(rv), 1);
    return (choice);
}

#ifdef POSITIONBAR
/*
update_positionbar(char *features)
    -- Optional, POSITIONBAR must be defined. Provide some 
        additional information for use in a horizontal
        position bar (most useful on clipped displays).
        Features is a series of char pairs.  The first char
        in the pair is a symbol and the second char is the
        column where it is currently located.
        A '<' is used to mark an upstairs, a '>'
        for a downstairs, and an '@' for the current player
        location. A zero char marks the end of the list.
*/
void
python_update_positionbar(char *features)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "update_positionbar", "s", features);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method update_positionbar failed\n");
}
#endif

/*
print_glyph(window, x, y, glyph)
    -- Print the glyph at (x,y) on the given window.  Glyphs are
        integers at the interface, mapped to whatever the window-
        port wants (symbol, font, color, attributes, ...there's
        a 1-1 map between glyphs and distinct things on the map).
*/
void
python_print_glyph(winid window, int x, int y, int glyph)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "print_glyph", "iiii", window, x, y, glyph);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method print_glyph failed\n");
}

#ifdef CLIPPING
/*
cliparound(x, y)
    -- Make sure that the user is more-or-less centered on the
        screen if the playing area is larger than the screen.
    -- This function is only defined if CLIPPING is defined.
*/
void
python_cliparound(int x, int y)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "cliparound", "ii", x, y);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method cliparound failed\n");
}
#endif

/*
start_screen()
    -- Only used on Unix tty ports, but must be declared for
        completeness.  Sets up the tty to work in full-screen
        graphics mode.  Look at win/tty/termcap.c for an
        example.  If your window-port does not need this function
        just declare an empty function.
*/
void 
python_start_screen() 
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "start_screen", NULL);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method start_screen failed\n");
}

/*
end_screen()
    -- Only used on Unix tty ports, but must be declared for
        completeness.  The complement of start_screen().
*/
void 
python_end_screen()
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "end_screen", NULL);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method end_screen failed\n");
}

/*
outrip(winid, int)
    -- The tombstone code.  If you want the traditional code use
        genl_outrip for the value and check the #if in rip.c.
*/
void
python_outrip(winid window, int how)
{
    PyObject *rv = NULL;

    rv = PyObject_CallMethod(interface_object, "outrip", "ii", window, how);
    if (!rv)
        PyErr_SetString(PyExc_AttributeError, "method outrip failed\n");
}

/*
int nhgetch()	
    -- Returns a single character input from the user.
    -- In the tty window-port, nhgetch() assumes that tgetch()
        will be the routine the OS provides to read a character.
        Returned character _must_ be non-zero and it must be
        non meta-zero too (zero with the meta-bit set).
*/
int
python_nhgetch()
{
    PyObject *rv = NULL;
    char str = '\0';

    rv = PyObject_CallMethod(interface_object, "nhgetch", NULL);
    if (!rv)
    {
        PyErr_SetString(PyExc_AttributeError, "method nhgetch failed\n");
	return ('\0');
    }
    if (!PyString_AsString(rv) || PyString_Size(rv) != 1)
    {  
        PyErr_SetString(PyExc_TypeError, "nhget expects a single character returned");
	return ('\0');
    }

    strncpy(&str, PyString_AsString(rv), 1);
    return (str);
}

PyObject *
Py_init_nhwindows(PyObject *self, PyObject *args)
{
    PyObject *list = PyList_New(0);
    return (list);
}

PyObject *
Py_exit_nhwindows(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_suspend_nhwindows(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_resume_nhwindows(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_create_nhwindow(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_clear_nhwindow(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_display_nhwindow(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

 PyObject *
Py_destroy_nhwindow(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_curs(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_putstr(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_display_file(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_start_menu(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_add_menu(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_end_menu(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_select_menu(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_update_inventory(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_mark_synch(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_wait_synch(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_cliparound(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_update_positionbar(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_print_glyph(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_raw_print(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_raw_print_bold(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_nhgetch(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_nh_poskey(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_nhbell(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_doprev_message(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_yn_function(PyObject *self, PyObject *args)
{
  PyObject *str = PyString_FromString("");
  return (str);
}

PyObject *
Py_getlin(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_get_ext_cmd(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_get_nh_event(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_number_pad(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_delay_output(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_start_screen(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_end_screen(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_outrip(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_player_selection(PyObject *self, PyObject *args)
{
    Py_RETURN_NONE;
}

PyObject *
Py_askname(PyObject *self, PyObject *args)
{
    PyObject *s = PyString_FromString("");
    return (s);
}
