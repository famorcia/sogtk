# SoGtk GTK2→GTK3 Porting Report

**Data**: 24 Gennaio 2026  
**Status**: Porting Iniziato - Compilazione Parziale  
**Target**: GTK3 3.24.41

## Setup Completato ✅

- ✅ GTK3 3.24.41 installato
- ✅ gtkglext-1.0 (OpenGL per GTK3) installato
- ✅ CMakeLists.txt aggiornato per GTK3
- ✅ CMake configuration completata

## Problematiche Tecniche

### Fatto (API Migrate Locali)
- ✅ `gtk_timeout_add/remove` → `g_timeout_add/g_source_remove`
- ✅ `gtk_idle_add/remove` → `g_idle_add/g_source_remove`
- ✅ `GtkObject` → `GObject` (header declarations)
- ✅ `gtkgl/gtkglarea.h` → `gtkglext-1.0` includes
- ✅ CMake GTK2 → GTK3 variables

### Ancora da Fare

**GTK2 Functions ancora usate in codice sorgente:**
1. `gtk_signal_connect()` - rimosso in GTK3
2. `GTK_SIGNAL_FUNC()` - rimosso in GTK3
3. `GTK_OBJECT()` macro - rimosso in GTK3
4. `gtk_signal_disconnect_by_func()` - rimosso in GTK3
5. `gtk_signal_connect_object()` - rimosso in GTK3
6. `gtk_tree_*` functions - rimossi in GTK3 (necessita GtkTreeView)
7. `GTK_WIDGET_TOPLEVEL` macro - rimosso in GTK3
8. `GTK_WIDGET_REALIZED` macro - rimosso in GTK3
9. `GTK_WIDGET_DRAWABLE` macro - rimosso in GTK3
10. `widget->parent` field - rimosso in GTK3 (usare gtk_widget_get_parent)
11. `widget->allocation` field - rimosso in GTK3
12. `GtkDialog->action_area` field - rimosso in GTK3
13. `GtkDialog->vbox` field - rimosso in GTK3
14. `gtk_widget_size_request()` - deprecato in GTK3
15. `GDK_NO_EXPOSE` event type - rimosso in GTK3
16. `gtk_timeout_add()` - ancora presente in una linea (line 230)

## Soluzione Consigliate

### Opzione 1: Continuare con GTK3 ⭐
**Pro:**
- GTK3 è una transizione intermedia di soli 10 anni rispetto a GTK2
- Meno radicale di GTK4
- Supportato fino al 2024/2025

**Contro:**
- Comunque molti API changes
- GTK3 stesso sarà deprecato tra poco (GTK4 diventa principale)
- Sforzo medio comunque (~15-20 ore)

**Tempo stimato**: 10-15 ore di lavoro

### Opzione 2: Saltare direttamente a GTK4 ⭐⭐
**Pro:**
- È il futuro di GTK
- Supportato per molti anni ancora
- API più moderna

**Contro:**
- Molto più radicale (come visto nel primo tentativo)
- Tempo più lungo

**Tempo stimato**: 25-35 ore

## File Prioritari da Aggiornare

### Priorità 1 (Alto)
1. `src/Inventor/Gtk/SoGtkComponent.cpp` - 15+ occorrenze GTK2 API
2. `src/Inventor/Gtk/SoGtk.cpp` - 10+ occorrenze
3. `src/Inventor/Gtk/SoGtkGraphEditor.cpp` - Tree widgets (obsoleti)

### Priorità 2 (Medio)
1. `src/Inventor/Gtk/SoGtkRoster.cpp` - GtkCList (obsoleto)
2. `src/Inventor/Gtk/SoGtkMaterialList.cpp` - GtkCList (obsoleto)
3. `src/Inventor/Gtk/viewers/*.cpp` - Varie API GTK2

### Priorità 3 (Basso)
1. `src/Inventor/Gtk/devices/*.cpp` - Minimal GTK2 usage
2. `src/Inventor/Gtk/widgets/*.cpp` - Dipende da altri

## Prossimi Passi

Se continui con GTK3:
1. Fixare `gtk_signal_connect` → `g_signal_connect` (15+ linee)
2. Fixare `GTK_OBJECT` macros (20+ linee)
3. Fixare macros deprecati (GTK_WIDGET_TOPLEVEL, etc.) (10+ linee)
4. Fixare field access (`widget->parent`, `widget->allocation`) (5+ linee)
5. Rimpiazzare GtkTree con GtkTreeView (20-30 linee complesse)
6. Rimpiazzare GtkCList (30-40 linee)
7. Fixare GtkDialog->vbox/action_area access (5 linee)
8. Compilare e testare

## Stima Realistica

**Per compilazione funzionale**: 8-12 ore  
**Per runtime testing**: +4-6 ore  
**Totale**: 12-18 ore

## Raccomandazione Finale

Given the scope of this project, I'd recommend:

**A breve termine**: Completare GTK3 porting (è il passo intermedio logico)  
**A lungo termine**: Piano per GTK4 quando GTK3 sarà EOL

Vuoi che continui con GTK3? Posso automatizzare gran parte dei fix rimasti.
