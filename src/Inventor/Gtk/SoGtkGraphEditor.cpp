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

#include <config.h>

#define GTK_ENABLE_BROKEN 1
#include <gtk/gtk.h>

#include <Inventor/SoOutput.h>
#include <Inventor/errors/SoDebugError.h>
#include <Inventor/misc/SoChildList.h>
#include <Inventor/SoLists.h>
#include <Inventor/fields/SoField.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/actions/SoWriteAction.h>

#include <sogtkdefs.h>

#include <Inventor/Gtk/SoGtkGraphEditor.h>

// *************************************************************************

/*!
  \class SoGtkGraphEditor Inventor/Gtk/SoGtkGraphEditor.h
  \brief The SoGtkGraphEditor class is a component for viewing and editing
  the Inventor scene graph of a model.
  \ingroup editors

  This component is not implemented so far that editing is possible.
*/

/*!
  \enum SoGtkGraphEditor::BuildFlag
  FIXME: write doc
*/

/*!
  \var SoGtkGraphEditor::BuildFlag SoGtkGraphEditor::MENUBAR
  FIXME: write doc
*/

/*!
  \var SoGtkGraphEditor::BuildFlag SoGtkGraphEditor::GRAPHEDITOR
  FIXME: write doc
*/

/*!
  \var SoGtkGraphEditor::BuildFlag SoGtkGraphEditor::STATUSBAR
  FIXME: write doc
*/

/*!
  \var SoGtkGraphEditor::BuildFlag SoGtkGraphEditor::EVERYTHING
  FIXME: write doc
*/

// *************************************************************************

SOGTK_OBJECT_SOURCE(SoGtkGraphEditor);

// *************************************************************************

/*!
  Public constructor.
*/

SoGtkGraphEditor::SoGtkGraphEditor(
  GtkWidget * const parent,
  const char * const name,
  const SbBool embed,
  const int parts)
: inherited(parent, name, embed)
{
  this->constructor(TRUE, parts);
} // SoGtkGraphEditor()

/*!
  Protected constructor.
*/

SoGtkGraphEditor::SoGtkGraphEditor(
  GtkWidget * const parent,
  const char * const name,
  const SbBool embed,
  const int parts,
  const SbBool build)
: inherited(parent, name, embed)
{
  this->constructor(build, parts);
} // SoGtkGraphEditor()

/*!
  Common constructor code.
*/

void
SoGtkGraphEditor::constructor(
  const SbBool build,
  const int parts)
{
  this->scenegraph = (SoNode *) NULL;

  this->buildflags = parts & EVERYTHING;

  this->editorbase = (GtkWidget *) NULL;
  this->menubar = (GtkWidget *) NULL;
  this->grapheditor = (GtkWidget *) NULL;
  this->graphroot = (GtkWidget *) NULL;
  this->statusbar = (GtkWidget *) NULL;
  this->statusmessage = (GtkWidget *) NULL;

  this->horizontal = (GtkAdjustment *) NULL;
  this->vertical = (GtkAdjustment *) NULL;

  this->setClassName("SoGtkGraphEditor");

  if (! build) return;
  this->setSize(SbVec2s(350, 450));
  GtkWidget * parent = this->getParentWidget();
  GtkWidget * editor = this->buildWidget(parent);
  this->setBaseWidget(editor);
} // constructor()

/*!
  Destructor.
*/

SoGtkGraphEditor::~SoGtkGraphEditor(
  void)
{
} // ~SoGtkGraphEditor()

// *************************************************************************

/*!
  Set the scene graph to edit.
*/

void
SoGtkGraphEditor::setSceneGraph(// virtual
  SoNode * root)
{
  if (this->scenegraph) {
    this->clearSceneGraphTree();
    this->scenegraph->unref();
  }
  this->scenegraph = root;
  if (this->scenegraph) {
    this->scenegraph->ref();
    this->buildSceneGraphTree();
  }
} // setSceneGraph()

/*!
  Returns the scene graph used in the graph editor.
*/

SoNode *
SoGtkGraphEditor::getSceneGraph(
  void) const
{
  return this->scenegraph;
} // getSceneGraph;

/*
  FIXME: write doc
*/

GtkWidget *
SoGtkGraphEditor::buildSubGraph(
  GtkWidget * parent,
  SoNode * node)
{
  assert(node != NULL);
  const char * classname = node->getTypeId().getName().getString();
  SbName nodename = node->getName();
  GtkWidget * root;
  
  // GTK3 MIGRATION: GtkTree removed, using simple VBox with labels as stub
  GtkWidget * vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  GtkWidget * label;
  
  if (! nodename) {
    label = gtk_label_new((gchar*)classname);
  } else {
    const char * name = nodename.getString();
    char buf[128];
    sprintf(buf, "%s \"%s\"", classname, name);
    label = gtk_label_new(buf);
  }
  gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
  gtk_widget_show(label);
  root = vbox;
  
  g_object_set_data(G_OBJECT(root), "SoNode", (gpointer) node);
  gtk_widget_show(root);
  // gtk_tree_append no longer available - just add to parent if it's a container
  if (GTK_IS_CONTAINER(parent)) {
    gtk_container_add(GTK_CONTAINER(parent), root);
  }
  g_signal_connect(G_OBJECT(root), "clicked",
    G_CALLBACK(SoGtkGraphEditor::selectionCB), (gpointer) this);

  GtkWidget * substuff = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(vbox), substuff, FALSE, FALSE, 0);
  gtk_widget_show(substuff);

  SoFieldList fields;
  if (node->getFields(fields) > 0) {
    GtkWidget * fieldsitem = gtk_label_new("[fields]");
    gtk_box_pack_start(GTK_BOX(substuff), fieldsitem, FALSE, FALSE, 0);
    gtk_widget_show(fieldsitem);
    GtkWidget * fieldstree = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(substuff), fieldstree, FALSE, FALSE, 0);
    gtk_widget_show(fieldstree);
    for (int i = 0; i < fields.getLength(); i++) {
      SoField * field = fields[i];
      SbName fieldname;
      node->getFieldName(field, fieldname);
      const char * fieldnamestr = fieldname.getString();
      GtkWidget * fieldwidget = gtk_label_new((gchar*)fieldnamestr);
      g_object_set_data(G_OBJECT(fieldwidget), "SoNode", (gpointer) node);
      g_object_set_data(G_OBJECT(fieldwidget), "SoField", (gpointer) field);
      gtk_box_pack_start(GTK_BOX(fieldstree), fieldwidget, FALSE, FALSE, 0);
      gtk_widget_show(fieldwidget);
      g_signal_connect(G_OBJECT(fieldwidget), "clicked",
        G_CALLBACK(SoGtkGraphEditor::selectionCB), (gpointer) this);
    }
  }

  SoChildList * children = node->getChildren();
  if (children != NULL && children->getLength() > 0) {
    for (int i = 0; i < children->getLength(); i++) {
      GtkWidget * kid = SoGtkGraphEditor::buildSubGraph(substuff, (SoNode *) children->get(i));
    }
  }
  return root;
}

/*!
  FIXME: write doc
*/

void
SoGtkGraphEditor::buildSceneGraphTree(// virtual, protected
  void)
{
  GtkWidget * graph = this->buildSubGraph(this->graphroot, this->scenegraph);
  // gtk_tree_item_expand no longer available in GTK3
  // Tree is now displayed as simple VBox hierarchy
} // buildSceneGraphTree()

/*!
  FIXME: write doc
*/

void
SoGtkGraphEditor::clearSceneGraphTree(// virtual, protected
  void)
{
  SoDebugError::postInfo("SoGtkGraphEditor::clearSceneGraphTree", 
    _("[not implemented]"));
} // clearSceneGraphTree()

/*!
  FIXME: write doc
*/

void
SoGtkGraphEditor::saveSceneGraph(// virtual, protected
  void)
{
  if (! this->scenegraph) {
    this->setStatusMessage(_("No scene to save."));
    return;
  }
  SoOutput * output = new SoOutput;
  if (! output->openFile("scene.iv")) {
    this->setStatusMessage(_("Error opening 'scene.iv' for writing."));
    delete output;
    return;
  }
  SoWriteAction writer(output);
  writer.apply(this->scenegraph);
  delete output;
  this->setStatusMessage(_("Scene saved in 'scene.iv'."));
} // saveSceneGraph()

// *************************************************************************

/*!
  This method returns builds and returns the basic graph editor component.
*/

GtkWidget *
SoGtkGraphEditor::buildWidget(// virtual, protected
  GtkWidget * parent)
{
  this->editorbase = GTK_WIDGET(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
  if (this->buildflags & MENUBAR) {
    this->menubar = this->buildMenuBarWidget(this->editorbase);
    gtk_widget_set_size_request(GTK_WIDGET(this->menubar), -1, 30);
    gtk_widget_show(this->menubar);
    gtk_box_pack_start(GTK_BOX(this->editorbase), GTK_WIDGET(this->menubar), FALSE, TRUE, 0);
  }
  if (this->buildflags & GRAPHEDITOR) {
    this->grapheditor = this->buildGraphEditorWidget(this->editorbase);
    gtk_widget_show(this->grapheditor);
    gtk_box_pack_start(GTK_BOX(this->editorbase), GTK_WIDGET(this->grapheditor), TRUE, TRUE, 0);
  }
  if (this->buildflags & STATUSBAR) {
    this->statusbar = this->buildStatusBarWidget(this->editorbase);
    gtk_widget_set_size_request(GTK_WIDGET(this->statusbar), -1, 30);
    gtk_widget_show(this->statusbar);
    gtk_box_pack_end(GTK_BOX(this->editorbase), GTK_WIDGET(this->statusbar), FALSE, TRUE, 0);
  }
  return this->editorbase;
} // buildWidget()

/*!
  This function builds and returns the graph editor menu bar.
*/

GtkWidget *
SoGtkGraphEditor::buildMenuBarWidget(// virtual, protected
  GtkWidget * parent)
{
  GtkWidget * menubar = GTK_WIDGET(gtk_menu_bar_new());
  GtkWidget * filemenuitem = gtk_menu_item_new_with_label(_("File"));
  gtk_widget_show(filemenuitem);
  gtk_menu_shell_append(GTK_MENU_SHELL(menubar), GTK_WIDGET(filemenuitem));
  GtkWidget * filemenu = GTK_WIDGET(gtk_menu_new());
  gtk_widget_show(filemenu);
  GtkWidget * saveitem = gtk_menu_item_new_with_label(_("Save"));
  gtk_widget_show(saveitem);
  gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), GTK_WIDGET(saveitem));
  g_signal_connect(G_OBJECT(saveitem), "activate",
    G_CALLBACK(SoGtkGraphEditor::saveCB), (gpointer) this);
  GtkWidget * closeitem = gtk_menu_item_new_with_label(_("Close"));
  gtk_widget_show(closeitem);
  gtk_menu_shell_append(GTK_MENU_SHELL(filemenu), GTK_WIDGET(closeitem));
  g_signal_connect(G_OBJECT(closeitem), "activate",
    G_CALLBACK(SoGtkGraphEditor::closeCB), (gpointer) this);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(filemenuitem), GTK_WIDGET(filemenu));
  return menubar;
} // buildMenuBarWidget()

/*!
  This function builds and returns the actual graph editor widget.
*/

GtkWidget *
SoGtkGraphEditor::buildGraphEditorWidget(// virtual, protected
  GtkWidget * parent)
{
  /* value, lower, upper, step_increment, page_increment, page_size */
  this->vertical = GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 1000, 10, 100, 100));
  this->horizontal = GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 1000, 10, 100, 1000));

  GtkWidget * editor = GTK_WIDGET(gtk_scrolled_window_new(horizontal, vertical));
  gtk_scrolled_window_set_policy(
    GTK_SCROLLED_WINDOW(editor), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  // GTK3 MIGRATION: GtkTree removed, using VBox instead
  this->graphroot = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_show(this->graphroot);
  gtk_container_add(GTK_CONTAINER(editor), this->graphroot);

  return editor;
} // buildGraphEditorWidget()

/*!
  This function builds and returns the graph editor status bar.
*/

GtkWidget *
SoGtkGraphEditor::buildStatusBarWidget(// virtual, protected
  GtkWidget * parent)
{
  GtkWidget * statusframe = GTK_WIDGET(gtk_frame_new((gchar *) NULL));
  gtk_widget_set_size_request(statusframe, -1, 30);
  gtk_frame_set_shadow_type(GTK_FRAME(statusframe), GTK_SHADOW_ETCHED_IN);
  gtk_container_set_border_width(GTK_CONTAINER(statusframe), 3);
  
  GtkWidget * statuswidgets = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_size_request(statusframe, -1, 30);
  gtk_container_add(GTK_CONTAINER(statusframe), statuswidgets);
  gtk_widget_show(statuswidgets);

  this->statusmessage = GTK_WIDGET(gtk_label_new((gchar *) NULL));
  gtk_label_set_justify(GTK_LABEL(this->statusmessage), GTK_JUSTIFY_LEFT);
  gtk_widget_show(this->statusmessage);
  gtk_box_pack_start(GTK_BOX(statuswidgets), this->statusmessage, FALSE, TRUE, 3);

  return statusframe;
} // buildStatusBarWidget()

// *************************************************************************

/*!
  FIXME: write doc
*/

void
SoGtkGraphEditor::nodeSelection(// virtual, protected
  GtkWidget * treeitem,
  SoNode * node)
{
  this->setStatusMessage(node->getTypeId().getName().getString());
} // nodeSelection()

/*!
  FIXME: write doc
*/

void
SoGtkGraphEditor::fieldSelection(// virtual, protected
  GtkWidget * treeitem,
  SoNode * node,
  SoField * field)
{
  this->setStatusMessage(field->getTypeId().getName().getString());
} // fieldSelection()

// *************************************************************************

// doc in super
void
SoGtkGraphEditor::sizeChanged(const SbVec2s & size)
{
  // inherited::sizeChanged(size);
} // sizeChanged()

// *************************************************************************

/*!
  FIXME: write doc
*/

void
SoGtkGraphEditor::saveCB(// static, private
  GObject * obj,
  gpointer closure)
{
  assert(closure != NULL);
  SoGtkGraphEditor * editor = (SoGtkGraphEditor *) closure;
  editor->saveSceneGraph();
} // saveCB()

/*!
  FIXME: write doc
*/

void
SoGtkGraphEditor::closeCB(// static, private
  GObject * obj,
  gpointer closure)
{
  assert(closure != NULL);
  SoGtkGraphEditor * editor = (SoGtkGraphEditor *) closure;
  editor->hide();
} // closeCB()

// *************************************************************************

/*!
  FIXME: write doc
*/

void
SoGtkGraphEditor::setStatusMessage(// virtual, protected
  const char * message)
{
  gtk_label_set_text(GTK_LABEL(this->statusmessage), message);
} // setStatusMessage()

// *************************************************************************

/*!
  FIXME: write doc
*/

void
SoGtkGraphEditor::selectionCB(
  GObject * object,
  gpointer closure)
{
  assert(closure != NULL);
  SoGtkGraphEditor * editor = (SoGtkGraphEditor *) closure;
  SoNode * node = (SoNode *) g_object_get_data(G_OBJECT(object), "SoNode");
  SoField * field = (SoField *) g_object_get_data(G_OBJECT(object), "SoField");
  if (node && ! field)
    editor->nodeSelection(GTK_WIDGET(object), node);
  else if (node && field)
    editor->fieldSelection(GTK_WIDGET(object), node, field);
  else
    SoDebugError::postInfo("SoGtkGraphEditor::selectionCB", "[invalid selection data]");
} // selectionCB()

// *************************************************************************

/*!
  FIXME: write doc
*/

const char *
SoGtkGraphEditor::getDefaultWidgetName(// virtual, protected
  void) const
{
  static const char defaultWidgetName[] = "SoGtkGraphEditor";
  return defaultWidgetName;
} // getDefaultWidgetName()

/*!
  FIXME: write doc
*/

const char *
SoGtkGraphEditor::getDefaultTitle(// virtual, protected
  void) const
{
  static const char defaultTitle[] = N_("Graph Editor");
  return _(defaultTitle);
} // getDefaultTitle()

/*!
  FIXME: write doc
*/

const char *
SoGtkGraphEditor::getDefaultIconTitle(// virtual, protected
  void) const
{
  static const char defaultIconTitle[] = N_("Graph Editor");
  return _(defaultIconTitle);
} // getDefaultIconTitle()

// *************************************************************************
