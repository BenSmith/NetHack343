#!/usr/bin/env python
# Copyright (c) Ben Smith, 2010 
# NetHack may be freely redistributed.  See license for details.

# This is a bare minimum implementation of the NetHack Python interface

import nethack

class MyHack(nethack.NethackProcs):

    def add_menu(self, winid, glyph, identifier, accelerator, groupaccel, attr, string, preselected):
        print ('*add_menu(%s, %s, %s, %s, %s, %s, %s, %s)' % (winid, glyph, identifier, accelerator, groupaccel, attr, string, preselected))

    def askname(self):
        print ('*askname')
        return ""

    def clear_nhwindow(self, winid):
        print ('*clear_nhwindow(%s)' % winid)

    def cliparound(self, x, y):
        print ('*cliparound(%s, %s)' % (x, y))

    def create_nhwindow(self, winid):
        print ('*create_nhwindow(%s)' % winid)

    def curs(self, winid, x, y):
        print ('*curs(%s, %s, %s)' % (winid, x, y))

    def delay_output(self):
        print ('*delay_output')

    def destroy_nhwindow(self, winid):
        print ('*destroy_nhwindow(%s)' % winid)

    def display_file(self, filename, complain):
        print ('*display_file(%s, %s)' % (filename, complain))

    def display_nhwindow(self, winid, blocking):
        print ('*display_nhwindow(%s, %s)' % (winid, blocking))

    def doprev_message(self):
        print ('*doprev_message')

    def end_menu(self, winid, string):
        print ('*end_menu(%s, %s)' % (winid, string))

    def end_screen(self):
        print ('*end_screen')

    def exit_nhwindows(self, string):
        print ('*exit_nhwindows(%s)' % string)

    def get_ext_cmd(self):
        print ('*get_ext_cmd')
        return 0

    def getlin(self, question):
        print ('*getlin(%s)' %s)
        return ""

    def init_nhwindows(self, argv):
        print ('*init_nhwindows(%s)' % argv)
        return argv

    def mark_synch(self):
        print ('*mark_synch')

    def nh_poskey(self, ch, x, y, mod):
        print ('*nh_poskey(%s, %s, %s, %s)' % (ch, x, y, mod))
        return (0, 0, 0, 0)

    def nhbell(self):
        print ('*nhbell')

    def nhgetch(self):
        print ('*nhgetch')
        return ""
    def number_pad(self, state):
        print ('*number_pad(%s)' % state)

    def outrip(self):
        print ('*outrip')

    def player_selection(self, role, race, gender, align, random, pantheon):
        print ('*player_selection(%s, %s, %s, %s, %s, %s)' % (role, race, gender, align, random, pantheon))
        return (role, race, gender, align, random, pantheon)

    def print_glyph(self, winid, x, y, glyph):
        print ('*print_glyph(%s, %s, %s, %s)' % (winid, x, y, glyph))

    def putstr(self, winid, attr, string):
        print ('*putstr(%s, %s, %s)' % (winid, attr, string))

    def raw_print(self, string):
        print ('*raw_print(%s)' % string)

    def raw_print_bold(self, string):
        print ('*raw_print_bold(%s)' % string)

    def resume_nhwindows(self):
        print ('*resume_nhwindows')

    def select_menu(self, winid, how):
        print ('*select_menu(%s, %s)' % (winid, how))
        return dict()

    def start_menu(self, winid):
        print ('*start_menu(%s)' % winid)

    def start_screen(self):
        print ('*start_screen')

    def suspend_nhwindows(self, string):
        print ('*suspend_nhwindows(%s)' % string)

    def update_inventory(self):
        print ('*update_inventory')

    def update_positionbar(self, features):
        print ('*update_positionbar(%s)' % features)

    def wait_synch(self):
        print ('*wait_synch')

    def yn_function(self, question, choices, default):
        print ('*yn_function(%s, %s, %s)' % (question, choices, default))
        return ""

    def get_nh_event(self):
        print ('*get_nh_event')

h = MyHack()
print (dir(h))
nethack.set_interface(h)

