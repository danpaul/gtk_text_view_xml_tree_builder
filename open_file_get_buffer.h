#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <libgen.h>
#include <gdk/gdkkeysyms.h>

//Todo: The includes need to be sorted out.  Add ifndef check.

/******************************************************************************
Overview:   Essentially, this file should be renamed. I originally intended for
it to return a buffer but since I realised opening a file is such a general
and generic function, I decided to make it just return a pointer to the file data.

Questions:

Is a char pointer is the best to use as a generic pointer.

Also, I don't understand where the memory is allocated for the file contents.
None seems to be explicitly allocated but the file is read into memory none
the less.

Modifictions:  Modified from professor Weiss code for text file opening.

*******************************************************************************/

/**
Precondition:   Takes a pointer to main program window (for open file dialog).
                Takes a pointer to a C string for file filter and mime.
Postcondition:  Opens a file dialog, error checks user input, and returns a
                pinter to file data.
*/
char* open_file_and_get_file_contents(GtkWidget *window,
                                      char* filter_name,
                                      char* mime_type);
