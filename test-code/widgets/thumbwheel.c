/* ********************************************************************** */

#include <stdio.h>

#include <gtk/gtk.h>

#include <Inventor/Gtk/widgets/gtkthumbwheel.h>

/* ********************************************************************** */

static void on_window_destroy(GtkWidget *widget, gpointer data)
{
  gtk_main_quit();
}

int
main(
  int argc,
  char ** argv )
{
  GtkWidget * window;
  GtkWidget * wheel;

  gtk_init( &argc, &argv );

  window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
  g_signal_connect(G_OBJECT(window), "destroy",
                   G_CALLBACK(on_window_destroy), NULL);

  gtk_container_set_border_width( GTK_CONTAINER(window), 10 );

  wheel = gtk_thumbwheel_new( TRUE );
  gtk_container_add( GTK_CONTAINER(window), wheel );
  gtk_widget_show( wheel );
  gtk_widget_show( window );

  gtk_main();
  return 0;
} /* main() */

/* ********************************************************************** */
