/**************************************************************************\
 * Copyright (c) Kongsberg Oil & Gas Technologies AS
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 
 * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\**************************************************************************/

// *************************************************************************

// Class documentation in common/viewers/SoGuiPlaneViewer.cpp.in.

// *************************************************************************

#include <config.h>

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>

#include <Inventor/Gtk/widgets/gtkthumbwheel.h>

#include <sogtkdefs.h>
#include <Inventor/Gtk/viewers/SoGtkPlaneViewer.h>
#include <Inventor/Gtk/viewers/SoGtkPlaneViewerP.h>
#include <Inventor/Gtk/widgets/SoGtkViewerButtonP.h>

#include <Inventor/Gtk/SoGtkCursor.h>
#include <Inventor/Gtk/common/pixmaps/x.xpm>
#include <Inventor/Gtk/common/pixmaps/y.xpm>
#include <Inventor/Gtk/common/pixmaps/z.xpm>
#include <Inventor/Gtk/common/pixmaps/perspective.xpm>
#include <Inventor/Gtk/common/pixmaps/ortho.xpm>

// *************************************************************************

// SoGtkPlaneViewerP "private implementation" class.

#define PUBLIC(ptr) (ptr->pub)
#define PRIVATE(ptr) (ptr->pimpl)

enum {
  X_BUTTON = 0,
  Y_BUTTON = 1,
  Z_BUTTON = 2,
  CAMERA_BUTTON = 3,

  FIRST_BUTTON = 0,
  LAST_BUTTON = CAMERA_BUTTON
};

struct SoGtkViewerButton
SoGtkPlaneViewerP::SoGtkPlaneViewerButtons[] = {
  { // plane X button
    N_("x"), "X",
    (GCallback) SoGtkPlaneViewerP::xbuttonCB,
    x_xpm
  },
  { // plane Y button
    N_("y"), "Y",
    (GCallback) SoGtkPlaneViewerP::ybuttonCB,
    y_xpm
  },
  { // plane Z button
    N_("z"), "Z",
    (GCallback) SoGtkPlaneViewerP::zbuttonCB,
    z_xpm
  },
  { // camera type button
    N_("camera"), "C",
    (GCallback) SoGtkPlaneViewerP::camerabuttonCB,
    perspective_xpm
  }
};

SoGtkPlaneViewerP::SoGtkPlaneViewerP(SoGtkPlaneViewer * publ)
  : SoGuiPlaneViewerP(publ)
{
  /* GdkColormap removed in GTK3 */
  /* gpointer colormap = gtk_widget_get_colormap(PUBLIC(this)->getParentWidget()); */

  /* gdk_pixmap_colormap_create_from_xpm_d removed in GTK3 */
  /* this->orthopixmap =
    gdk_pixmap_colormap_create_from_xpm_d((GdkWindow *) 0, colormap,
                                          &this->orthomask, (GdkColor *) 0,
                                          (gchar **)ortho_xpm);

  this->perspectivepixmap =
    gdk_pixmap_colormap_create_from_xpm_d((GdkWindow *) 0, colormap,
                                          &this->perspectivemask,
                                          (GdkColor *) 0,
                                          (gchar **)perspective_xpm); */
  
  this->orthopixmap = NULL;
  this->orthomask = NULL;
  this->perspectivepixmap = NULL;
  this->perspectivemask = NULL;
}

SoGtkPlaneViewerP::~SoGtkPlaneViewerP()
{
  /* GdkPixmap cleanup removed in GTK3 */
  /* Button pixmaps.
  gdk_pixmap_unref(this->orthopixmap);
  gdk_bitmap_unref(this->orthomask);  
  gdk_pixmap_unref(this->perspectivepixmap);
  gdk_bitmap_unref(this->perspectivemask); */
  
  if (this->orthopixmap) g_object_unref(this->orthopixmap);
  if (this->orthomask) g_object_unref(this->orthomask);
  if (this->perspectivepixmap) g_object_unref(this->perspectivepixmap);
  if (this->perspectivemask) g_object_unref(this->perspectivemask);
}

void
SoGtkPlaneViewerP::constructor(const SbBool build)
{
  this->commonConstructor(); // set up generic stuff

  PUBLIC(this)->setClassName("SoGtkPlaneViewer");

  PUBLIC(this)->setPopupMenuString(_("Plane Viewer"));

  PUBLIC(this)->setLeftWheelString(_("Translate Y"));
  PUBLIC(this)->setBottomWheelString(_("Translate X"));
  PUBLIC(this)->setRightWheelString(_("Zoom"));

  if (! build) return;
  GtkWidget * viewer = PUBLIC(this)->buildWidget(PUBLIC(this)->getParentWidget());
  PUBLIC(this)->setBaseWidget(viewer);
//  PUBLIC(this)->setSize(SbVec2s(500, 460));
}


void
SoGtkPlaneViewerP::xbuttonCB(GtkWidget * button, gpointer closure)
{
  SoGtkPlaneViewer * viewer = (SoGtkPlaneViewer *) closure;
  PRIVATE(viewer)->viewPlaneX();
}

void
SoGtkPlaneViewerP::ybuttonCB(GtkWidget * button, gpointer closure)
{
  SoGtkPlaneViewer * viewer = (SoGtkPlaneViewer *) closure;
  PRIVATE(viewer)->viewPlaneY();
}

void
SoGtkPlaneViewerP::zbuttonCB(GtkWidget * button, gpointer closure)
{
  SoGtkPlaneViewer * viewer = (SoGtkPlaneViewer *) closure;
  PRIVATE(viewer)->viewPlaneZ();
}

void
SoGtkPlaneViewerP::camerabuttonCB(GtkWidget * button, gpointer closure)
{
  SoGtkPlaneViewer * viewer = (SoGtkPlaneViewer *) closure;
  if (viewer->getCamera()) { viewer->toggleCameraType(); }
}


// *************************************************************************

// Documented in superclass.
void
SoGtkPlaneViewer::setCamera(SoCamera * newCamera)
{
  if (newCamera) {
    const SoType camtype = newCamera->getTypeId() ;
    const SbBool orthotype =
      camtype.isDerivedFrom(SoOrthographicCamera::getClassTypeId());

    this->setRightWheelString(orthotype ? _("Zoom") : _("Dolly"));
    if (PRIVATE(this)->cameratogglebutton) {
      /* GTK3: gtk_image_set_from_pixbuf simplified, GTK_BIN->child removed */
      /* GtkWidget * label = GTK_BIN(PRIVATE(this)->cameratogglebutton)->child;
      gtk_image_set_from_pixbuf(GTK_IMAGE(label),
        (orthotype ? PRIVATE(this)->orthopixmap : PRIVATE(this)->perspectivepixmap),
        (orthotype ? PRIVATE(this)->orthomask : PRIVATE(this)->perspectivemask)); */
    }
  }
  inherited::setCamera(newCamera);
}

// *************************************************************************

// Documented in superclass.
GtkWidget *
SoGtkPlaneViewer::buildWidget(GtkWidget * parent)
{
  return inherited::buildWidget(parent);
}

// *************************************************************************

// Documented in superclass.
void
SoGtkPlaneViewer::createViewerButtons(GtkWidget * parent,
                                      SbPList * buttonlist)
{
  inherited::createViewerButtons(parent, buttonlist);

  gpointer * tooltips = NULL;
  gpointer * colormap = NULL;

  const size_t buttons = sizeof(SoGtkPlaneViewerP::SoGtkPlaneViewerButtons)
                         / sizeof(SoGtkViewerButton);
  for (size_t button = 0; button < buttons; button++) {
    GtkWidget * widget = (GtkWidget*) 0 ;

    switch(button)
    {
    case CAMERA_BUTTON :
      PRIVATE(this)->cameratogglebutton = widget = gtk_button_new();
      break ;
    default: 
      widget = gtk_button_new();
      break ;
    }

    gtk_widget_set_can_focus(widget, FALSE);
    /* gtk_tooltip_set_text removed in GTK3 */
    /* gtk_tooltip_set_text(tooltips, widget, 
      _(SoGtkPlaneViewerP::SoGtkPlaneViewerButtons[button].keyword), NULL); */

    /* GdkPixbuf and GdkPixbuf removed in GTK3 */
    GdkPixbuf * gdk_pixmap = NULL;
    GdkPixbuf * gdk_mask   = NULL;

    switch(button)
    {
    case CAMERA_BUTTON :
      {
        SoType t = this->getCameraType();
        if (t.isDerivedFrom(SoOrthographicCamera::getClassTypeId()))
        {
          gdk_pixmap = PRIVATE(this)->orthopixmap;
          gdk_mask   = PRIVATE(this)->orthomask;
        }
        else if (t.isDerivedFrom(SoPerspectiveCamera::getClassTypeId()))
        {
          gdk_pixmap = PRIVATE(this)->perspectivepixmap;
          gdk_mask   = PRIVATE(this)->perspectivemask;
        }
        else assert(0 && "unsupported cameratype");
      }
      break;
    default:
      /* gdk_pixmap_colormap_create_from_xpm_d removed in GTK3 */
      /* gdk_pixmap =
        gdk_pixmap_colormap_create_from_xpm_d((GdkWindow *) 0, colormap,
                                              &gdk_mask, (GdkColor *) 0,
                                              (gchar **)SoGtkPlaneViewerP::SoGtkPlaneViewerButtons[button].xpm_data); */
      break;
    }


    GtkWidget * label = gtk_image_new(); /* gtk_pixmap_new removed in GTK3 */
    /* GtkWidget * label = gtk_pixmap_new(gdk_pixmap, gdk_mask); */
    gtk_widget_show(label);

    switch(button)
    {
    case CAMERA_BUTTON:
      break ;
    default :
      /* gdk_pixmap_unref(gdk_pixmap);
      gdk_bitmap_unref(gdk_mask); */
      if (gdk_pixmap) g_object_unref(gdk_pixmap);
      if (gdk_mask) g_object_unref(gdk_mask);
      break ;
    };

    gtk_container_add(GTK_CONTAINER(widget), GTK_WIDGET(label));
    if (SoGtkPlaneViewerP::SoGtkPlaneViewerButtons[button].pressed != NULL) {
      g_signal_connect(G_OBJECT(widget), "pressed",
        G_CALLBACK(SoGtkPlaneViewerP::SoGtkPlaneViewerButtons[button].pressed),
        (gpointer) this);
    }
    buttonlist->append(widget);
  }
}

// ************************************************************************
