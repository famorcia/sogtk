#! /bin/sh
# not to be executed - only sourced

GUI="Gtk"
lGUI="gtk"
uGUI="GTK"
WIDGET="GtkWidget *"
EVENT="GdkEvent *"
COMPONENTHEADER=""

HAVE_RENDERAREA=true
HAVE_EXAMINERVIEWER=true
HAVE_PLANEVIEWER=false
HAVE_WALKVIEWER=false
HAVE_FLYVIEWER=false

export GUI uGUI lGUI
export WIDGET EVENT COMPONENTHEADER
export HAVE_RENDERAREA HAVE_EXAMINERVIEWER
export HAVE_PLANEVIEWER HAVE_WALKVIEWER HAVE_FLYVIEWER

