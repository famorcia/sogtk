/**************************************************************************\
 * Copyright (c) Kongsberg Oil & Gas Technologies AS
 * All rights reserved.
 * 
 * STUB for GTK3 - Original widget requires GTK2 type system rewrite
 * The original gtkthumbwheel.cpp implemented a custom GTK2 widget using
 * deprecated type system (gtk_type_new, gtk_signal_new, GtkObjectClass).
 * For GTK3, this needs to be completely rewritten using GObject.
 * This stub allows compilation to proceed.
\**************************************************************************/

#include <gtk/gtk.h>
#include "gtkthumbwheel.h"

/* Minimal stub implementation for GtkThumbWheel widget */

GType gtk_thumbwheel_get_type(void)
{
  static GType type = 0;
  if (!type) {
    static const GTypeInfo info = {
      sizeof(GtkThumbWheelClass),
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      sizeof(GtkThumbWheel),
      0,
      NULL,
      NULL
    };
    type = g_type_register_static(GTK_TYPE_MISC, "GtkThumbWheel", &info, (GTypeFlags)0);
  }
  return type;
}

GtkWidget *gtk_thumbwheel_new(gint vertical)
{
  GtkWidget *widget = GTK_WIDGET(g_object_new(GTK_TYPE_THUMBWHEEL, NULL));
  if (widget) {
    GtkThumbWheel *thumbwheel = GTK_THUMBWHEEL(widget);
    thumbwheel->vertical = vertical ? TRUE : FALSE;
  }
  return widget;
}

void gtk_thumbwheel_set_update_policy(GtkThumbWheel *thumbwheel, guint policy)
{
  if (thumbwheel) thumbwheel->policy = policy & 3;
}

void gtk_thumbwheel_set_value(GtkThumbWheel *thumbwheel, gfloat value)
{
  /* TODO: Implement in GTK3 */
  if (thumbwheel) thumbwheel->value = value;
}

gfloat gtk_thumbwheel_get_value(GtkThumbWheel *thumbwheel)
{
  return thumbwheel ? thumbwheel->value : 0.0f;
}

void gtk_thumbwheel_set_range_boundary_handling(
  GtkThumbWheel *thumbwheel,
  GtkThumbWheelBoundaryType handling)
{
  /* TODO: Implement in GTK3 */
}

GtkThumbWheelBoundaryType gtk_thumbwheel_get_range_boundary_handling(
  GtkThumbWheel *thumbwheel)
{
  return GTK_THUMBWHEEL_BOUNDARY_CLAMP;
}

void gtk_thumbwheel_set_motion_method(
  GtkThumbWheel *thumbwheel,
  GtkThumbWheelMotionType handling)
{
  /* TODO: Implement in GTK3 */
}

GtkThumbWheelMotionType gtk_thumbwheel_get_motion_method(
  GtkThumbWheel *thumbwheel)
{
  return GTK_THUMBWHEEL_MOTION_AUTHENTIC;
}
