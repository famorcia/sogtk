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

#include <gtk/gtk.h>

#include <config.h> // HAVE_CONFIG_H

#include <Inventor/errors/SoDebugError.h>
#include <Inventor/events/SoKeyboardEvent.h>

#include <sogtkdefs.h>
#include <Inventor/Gtk/devices/SoGtkKeyboard.h>
#include <Inventor/Gtk/devices/SoGuiKeyboardP.h>
#include <Inventor/Gtk/devices/SoGtkInputFocus.h>

#define PRIVATE(obj) ((obj)->pimpl)
#define PUBLIC(obj) ((obj)->pub)

// *************************************************************************

class SoGtkKeyboardP : public SoGuiKeyboardP {
};

// *************************************************************************

SoGtkKeyboard::SoGtkKeyboard(int eventbits)
{
  PRIVATE(this) = new SoGtkKeyboardP;
  PRIVATE(this)->eventmask = eventbits & SoGtkKeyboard::ALL_EVENTS;
}

SoGtkKeyboard::~SoGtkKeyboard()
{
  delete PRIVATE(this);
}

// *************************************************************************

// A Gtk-event handler internally used to automatically grab the
// keyboard focus, when a mouse pointer enters a widget connected to a
// SoGtkKeyboard.
static gboolean
EnterHandler(GtkWidget *widget,
             GdkEventCrossing *event,
             gpointer user_data)
{
  // Compell CAN_FOCUS on the (gtkglarea) widget. G.Barrand.
  if (!gtk_widget_get_can_focus(widget)) 
    gtk_widget_set_can_focus(widget, TRUE);
  if (!gtk_widget_has_focus(widget))
    gtk_widget_grab_focus(widget);
  return FALSE;
}

void
SoGtkKeyboard::enable(GtkWidget * widget, SoGtkEventHandler * func,
                      gpointer closure)
{
  if (func) {
    if (PRIVATE(this)->eventmask & SoGtkKeyboard::KEY_PRESS) {
      g_signal_connect(G_OBJECT(widget), "key_press_event",
                         G_CALLBACK(func), closure);
      gtk_widget_add_events(GTK_WIDGET(widget),GDK_KEY_PRESS_MASK);
    }
    if (PRIVATE(this)->eventmask & SoGtkKeyboard::KEY_RELEASE) {
      g_signal_connect(G_OBJECT(widget), "key_release_event",
                         G_CALLBACK(func), closure);
      gtk_widget_add_events(GTK_WIDGET(widget),GDK_KEY_RELEASE_MASK);
    }
  }

  // When entering the window, we want to have the keyboard
  // focus. G.Barrand.
  //
  // FIXME: this seems bogus, as we're checking our eventmask against
  // a flag in SoGtkInputFocus? Does it work? If so, is it by sheer
  // luck? 20020625 mortene.
  if (PRIVATE(this)->eventmask & SoGtkInputFocus::ENTER_WINDOW) {
    g_signal_connect(G_OBJECT(widget), "enter_notify_event",
                       G_CALLBACK(EnterHandler), closure);
    gtk_widget_add_events(GTK_WIDGET(widget), GDK_ENTER_NOTIFY_MASK);
  }
}

void
SoGtkKeyboard::disable(GtkWidget * widget,
                       SoGtkEventHandler * func,
                       gpointer closure)
{
  if (func) {
    /* g_signal_handlers_disconnect_by_func requires the exact same callback address */
    gulong handler_id = g_signal_handler_find(G_OBJECT(widget), 
                                             G_SIGNAL_MATCH_FUNC,
                                             0,
                                             NULL,
                                             NULL,
                                             (gpointer)func,
                                             NULL);
    if (handler_id > 0) {
      g_signal_handler_disconnect(G_OBJECT(widget), handler_id);
    }
  }
}

// *************************************************************************

static
SoKeyboardEvent::Key
getKeyForKeyCode(guint keycode, char & printable)
{
  switch (keycode) {
  case GDK_KEY_Shift_L:        return SoKeyboardEvent::LEFT_SHIFT;
  case GDK_KEY_Shift_R:        return SoKeyboardEvent::RIGHT_SHIFT;
  case GDK_KEY_Control_L:      return SoKeyboardEvent::LEFT_CONTROL;
  case GDK_KEY_Control_R:      return SoKeyboardEvent::RIGHT_CONTROL;
  case GDK_KEY_Alt_L:          return SoKeyboardEvent::LEFT_ALT;
  case GDK_KEY_Alt_R:          return SoKeyboardEvent::RIGHT_ALT;
  case GDK_KEY_0: 
    printable = '0';
    return SoKeyboardEvent::NUMBER_0;
  case GDK_KEY_parenright:
    printable = ')';
    return SoKeyboardEvent::NUMBER_0;
    
  case GDK_KEY_exclam:
    printable = '!';
  case GDK_KEY_1: 
    return SoKeyboardEvent::NUMBER_1;  
  case GDK_KEY_at: 
    printable = '@';
  case GDK_KEY_2: 
    return SoKeyboardEvent::NUMBER_2;
  case GDK_KEY_numbersign:
    printable = '#';
  case GDK_KEY_3: 
    return SoKeyboardEvent::NUMBER_3;
  case GDK_KEY_dollar: 
    printable = '$';
  case GDK_KEY_4:
    return SoKeyboardEvent::NUMBER_4;
  case GDK_KEY_percent:
    printable = '%';
  case GDK_KEY_5: 
    return SoKeyboardEvent::NUMBER_5;
  case GDK_KEY_asciicircum: // ?
    printable = '^';
  case GDK_KEY_6: 
    return SoKeyboardEvent::NUMBER_6;
  case GDK_KEY_ampersand:
    printable = '&';
  case GDK_KEY_7: 
    return SoKeyboardEvent::NUMBER_7;
  case GDK_KEY_asterisk:
    printable = '*';
  case GDK_KEY_8: 
    return SoKeyboardEvent::NUMBER_8;
  case GDK_KEY_parenleft:
    printable = '(';
  case GDK_KEY_9: 
    return SoKeyboardEvent::NUMBER_9;

  case GDK_KEY_a: case GDK_KEY_A:  return SoKeyboardEvent::A;
  case GDK_KEY_b: case GDK_KEY_B:  return SoKeyboardEvent::B;
  case GDK_KEY_c: case GDK_KEY_C:  return SoKeyboardEvent::C;
  case GDK_KEY_d: case GDK_KEY_D:  return SoKeyboardEvent::D;
  case GDK_KEY_e: case GDK_KEY_E:  return SoKeyboardEvent::E;
  case GDK_KEY_f: case GDK_KEY_F:  return SoKeyboardEvent::F;
  case GDK_KEY_g: case GDK_KEY_G:  return SoKeyboardEvent::G;
  case GDK_KEY_h: case GDK_KEY_H:  return SoKeyboardEvent::H;
  case GDK_KEY_i: case GDK_KEY_I:  return SoKeyboardEvent::I;
  case GDK_KEY_j: case GDK_KEY_J:  return SoKeyboardEvent::J;
  case GDK_KEY_k: case GDK_KEY_K:  return SoKeyboardEvent::K;
  case GDK_KEY_l: case GDK_KEY_L:  return SoKeyboardEvent::L;
  case GDK_KEY_m: case GDK_KEY_M:  return SoKeyboardEvent::M;
  case GDK_KEY_n: case GDK_KEY_N:  return SoKeyboardEvent::N;
  case GDK_KEY_o: case GDK_KEY_O:  return SoKeyboardEvent::O;
  case GDK_KEY_p: case GDK_KEY_P:  return SoKeyboardEvent::P;
  case GDK_KEY_q: case GDK_KEY_Q:  return SoKeyboardEvent::Q;
  case GDK_KEY_r: case GDK_KEY_R:  return SoKeyboardEvent::R;
  case GDK_KEY_s: case GDK_KEY_S:  return SoKeyboardEvent::S;
  case GDK_KEY_t: case GDK_KEY_T:  return SoKeyboardEvent::T;
  case GDK_KEY_u: case GDK_KEY_U:  return SoKeyboardEvent::U;
  case GDK_KEY_v: case GDK_KEY_V:  return SoKeyboardEvent::V;
  case GDK_KEY_w: case GDK_KEY_W:  return SoKeyboardEvent::W;
  case GDK_KEY_x: case GDK_KEY_X:  return SoKeyboardEvent::X;
  case GDK_KEY_y: case GDK_KEY_Y:  return SoKeyboardEvent::Y;
  case GDK_KEY_z: case GDK_KEY_Z:  return SoKeyboardEvent::Z;

  case GDK_KEY_Home:           return SoKeyboardEvent::HOME;
  case GDK_KEY_Left:           return SoKeyboardEvent::LEFT_ARROW;
  case GDK_KEY_Up:             return SoKeyboardEvent::UP_ARROW;
  case GDK_KEY_Right:          return SoKeyboardEvent::RIGHT_ARROW;
  case GDK_KEY_Down:           return SoKeyboardEvent::DOWN_ARROW;
  case GDK_KEY_Page_Up:        return SoKeyboardEvent::PAGE_UP;
  case GDK_KEY_Page_Down:      return SoKeyboardEvent::PAGE_DOWN;
  case GDK_KEY_End:            return SoKeyboardEvent::END;
#if 0
  case GDK_KEY_Prior:          return SoKeyboardEvent::PRIOR;
  case GDK_KEY_Next:           return SoKeyboardEvent::NEXT;
#endif

  case GDK_KEY_KP_Enter:       return SoKeyboardEvent::PAD_ENTER;
  case GDK_KEY_KP_F1:          return SoKeyboardEvent::PAD_F1;
  case GDK_KEY_KP_F2:          return SoKeyboardEvent::PAD_F2;
  case GDK_KEY_KP_F3:          return SoKeyboardEvent::PAD_F3;
  case GDK_KEY_KP_F4:          return SoKeyboardEvent::PAD_F4;
  case GDK_KEY_KP_0:           return SoKeyboardEvent::PAD_0;
  case GDK_KEY_KP_1:           return SoKeyboardEvent::PAD_1;
  case GDK_KEY_KP_2:           return SoKeyboardEvent::PAD_2;
  case GDK_KEY_KP_3:           return SoKeyboardEvent::PAD_3;
  case GDK_KEY_KP_4:           return SoKeyboardEvent::PAD_4;
  case GDK_KEY_KP_5:           return SoKeyboardEvent::PAD_5;
  case GDK_KEY_KP_6:           return SoKeyboardEvent::PAD_6;
  case GDK_KEY_KP_7:           return SoKeyboardEvent::PAD_7;
  case GDK_KEY_KP_8:           return SoKeyboardEvent::PAD_8;
  case GDK_KEY_KP_9:           return SoKeyboardEvent::PAD_9;
  case GDK_KEY_KP_Add:         return SoKeyboardEvent::PAD_ADD;
  case GDK_KEY_KP_Subtract:    return SoKeyboardEvent::PAD_SUBTRACT;
  case GDK_KEY_KP_Multiply:    return SoKeyboardEvent::PAD_MULTIPLY;
  case GDK_KEY_KP_Divide:      return SoKeyboardEvent::PAD_DIVIDE;
  case GDK_KEY_KP_Space:       return SoKeyboardEvent::PAD_SPACE;
  case GDK_KEY_KP_Tab:         return SoKeyboardEvent::PAD_TAB;
  case GDK_KEY_KP_Insert:      return SoKeyboardEvent::PAD_INSERT;
  case GDK_KEY_KP_Delete:      return SoKeyboardEvent::PAD_DELETE;
  case GDK_KEY_KP_Decimal:     return SoKeyboardEvent::PAD_PERIOD;

  case GDK_KEY_F1:             return SoKeyboardEvent::F1;
  case GDK_KEY_F2:             return SoKeyboardEvent::F2;
  case GDK_KEY_F3:             return SoKeyboardEvent::F3;
  case GDK_KEY_F4:             return SoKeyboardEvent::F4;
  case GDK_KEY_F5:             return SoKeyboardEvent::F5;
  case GDK_KEY_F6:             return SoKeyboardEvent::F6;
  case GDK_KEY_F7:             return SoKeyboardEvent::F7;
  case GDK_KEY_F8:             return SoKeyboardEvent::F8;
  case GDK_KEY_F9:             return SoKeyboardEvent::F9;
  case GDK_KEY_F10:            return SoKeyboardEvent::F10;
  case GDK_KEY_F11:            return SoKeyboardEvent::F11;
  case GDK_KEY_F12:            return SoKeyboardEvent::F12;

  case GDK_KEY_BackSpace:      return SoKeyboardEvent::BACKSPACE;
  case GDK_KEY_Tab:            return SoKeyboardEvent::TAB;
  case GDK_KEY_Return:         return SoKeyboardEvent::RETURN;
#if 0
  case GDK_KEY_Return:         return SoKeyboardEvent::ENTER;
#endif
  case GDK_KEY_Pause:          return SoKeyboardEvent::PAUSE;
  case GDK_KEY_Scroll_Lock:    return SoKeyboardEvent::SCROLL_LOCK;
  case GDK_KEY_Escape:         return SoKeyboardEvent::ESCAPE;
#ifdef HAVE_SOKEYBOARDEVENT_DELETE
  case GDK_KEY_Delete:         return SoKeyboardEvent::DELETE;
#else
  case GDK_KEY_Delete:         return SoKeyboardEvent::KEY_DELETE;
#endif
  case GDK_KEY_Print:          return SoKeyboardEvent::PRINT;
  case GDK_KEY_Insert:         return SoKeyboardEvent::INSERT;
  case GDK_KEY_Num_Lock:       return SoKeyboardEvent::NUM_LOCK;
  case GDK_KEY_Caps_Lock:      return SoKeyboardEvent::CAPS_LOCK;
  case GDK_KEY_Shift_Lock:     return SoKeyboardEvent::SHIFT_LOCK;

  case GDK_KEY_space: return SoKeyboardEvent::SPACE;

  case GDK_KEY_apostrophe:
    printable = '\'';
    return SoKeyboardEvent::APOSTROPHE;
  case GDK_KEY_quotedbl: 
    printable = '\"';
    return SoKeyboardEvent::APOSTROPHE;

  case GDK_KEY_comma:
    printable = ',';
    return SoKeyboardEvent::COMMA;
  case GDK_KEY_less: 
    printable = '<';
    return SoKeyboardEvent::COMMA;

  case GDK_KEY_minus:
    printable = '-';
    return SoKeyboardEvent::MINUS;
  case GDK_KEY_underscore:
    printable = '_';
    return SoKeyboardEvent::MINUS;

  case GDK_KEY_period:
    printable = '.';
    return SoKeyboardEvent::PERIOD;
  case GDK_KEY_greater:
    printable = '>';
    return SoKeyboardEvent::PERIOD;

  case GDK_KEY_slash:
    printable = '/';
    return SoKeyboardEvent::SLASH;
  case GDK_KEY_question:
    printable = '?';
    return SoKeyboardEvent::SLASH;

  case GDK_KEY_semicolon:
    printable = ';';
    return SoKeyboardEvent::SEMICOLON;
  case GDK_KEY_colon:
    printable = ':';
    return SoKeyboardEvent::SEMICOLON;

  case GDK_KEY_equal:
    printable = '=';
    return SoKeyboardEvent::EQUAL;
  case GDK_KEY_plus:
    printable = '+';
    return SoKeyboardEvent::EQUAL;

  case GDK_KEY_bracketleft:
    printable = '[';
    return SoKeyboardEvent::BRACKETLEFT;
  case GDK_KEY_braceleft:
    printable = '{';
    return SoKeyboardEvent::BRACKETLEFT;

  case GDK_KEY_backslash:
    printable = '\\';
    return SoKeyboardEvent::BACKSLASH;
  case GDK_KEY_bar:
    printable = '|';
    return SoKeyboardEvent::BACKSLASH;

  case GDK_KEY_bracketright:
    printable = ']';
    return SoKeyboardEvent::BRACKETRIGHT;
  case GDK_KEY_braceright:
    printable = '}';
    return SoKeyboardEvent::BRACKETRIGHT;
    
  case GDK_KEY_grave:
    printable = '`';
    return SoKeyboardEvent::GRAVE;
  case GDK_KEY_asciitilde:
    printable = '~';
    return SoKeyboardEvent::GRAVE;

  default:
    break;
  }

  return SoKeyboardEvent::ANY;
}

// *************************************************************************

const SoEvent *
SoGtkKeyboard::translateEvent(GdkEvent * ev)
{
  switch (ev->type) {
  case GDK_KEY_PRESS:
    if (PRIVATE(this)->eventmask & KEY_PRESS) {
      const GdkEventKey * const event = (GdkEventKey *) ev;
      SbTime stamp;
      stamp.setMsecValue(event->time);
      PRIVATE(this)->kbdevent->setTime(stamp);
      const SbVec2s pos = SoGtkDevice::getLastEventPosition();
      SoGtkDevice::setEventPosition(PRIVATE(this)->kbdevent, pos[0], pos[1]);
      char printable = 0;
      PRIVATE(this)->kbdevent->setKey(getKeyForKeyCode(event->keyval, printable));
#if 0 // disabled. Breaks build when compiling against OIV
      if (printable) PRIVATE(this)->kbdevent->setPrintableCharacter(printable);
#endif // disabled
      PRIVATE(this)->kbdevent->setState(SoButtonEvent::DOWN);
      PRIVATE(this)->kbdevent->setShiftDown((event->state & GDK_SHIFT_MASK) ? TRUE : FALSE);
      PRIVATE(this)->kbdevent->setCtrlDown((event->state & GDK_CONTROL_MASK) ? TRUE : FALSE);
      PRIVATE(this)->kbdevent->setAltDown((event->state & GDK_MOD1_MASK) ? TRUE : FALSE);
      return PRIVATE(this)->kbdevent;
    }
    break;

  case GDK_KEY_RELEASE:
    if (PRIVATE(this)->eventmask & KEY_RELEASE) {
      const GdkEventKey * const event = (GdkEventKey *) ev;
      SbTime stamp;
      stamp.setMsecValue(event->time);
      PRIVATE(this)->kbdevent->setTime(stamp);
      const SbVec2s pos = SoGtkDevice::getLastEventPosition();
      SoGtkDevice::setEventPosition(PRIVATE(this)->kbdevent, pos[0], pos[1]);
      char printable = 0;
      PRIVATE(this)->kbdevent->setKey(getKeyForKeyCode(event->keyval, printable));
#if 0 // disabled. Breaks build when compiling against OIV
      if (printable) PRIVATE(this)->kbdevent->setPrintableCharacter(printable);
#endif // disabled
      PRIVATE(this)->kbdevent->setState(SoButtonEvent::UP);
      PRIVATE(this)->kbdevent->setShiftDown((event->state & GDK_SHIFT_MASK) ? TRUE : FALSE);
      PRIVATE(this)->kbdevent->setCtrlDown((event->state & GDK_CONTROL_MASK) ? TRUE : FALSE);
      PRIVATE(this)->kbdevent->setAltDown((event->state & GDK_MOD1_MASK) ? TRUE : FALSE);
      return PRIVATE(this)->kbdevent;
    }
    break;

  default:
    break;
  }

  return (const SoEvent *) NULL;
}

// *************************************************************************
