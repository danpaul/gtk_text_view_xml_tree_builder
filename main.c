/******************************************************************************
  Title          : hwk3_breczin.c
  Author         : Dan Breciznski
  Created on     : October, 2011
  Description    : Opens a text file, parses it for certain tags and styles
                   according to tags.

  Usage          : hwk3_breczin
  Build with     : gcc -o hwk3_breczin  hwk3_breczin.c open_file_get_buffer.c\
                        xml_parser.c `pkg-config gtk+-2.0 --cflags --libs`
  Modifications  : Oct. 20, 2011

******************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <libgen.h>
#include <gdk/gdkkeysyms.h>

#include "xml_parser.h"
#include "open_file_get_buffer.h"


/******************************************************************************
*                                                                             *
*                         constants and structures                            *
*                                                                             *
******************************************************************************/

int const WINWIDTH = 600;
int const WINHEIGHT = 600;

typedef struct _AppState
{
    GList* tags;
    GtkButton* close_button;
    GtkButton* open_button;
    GtkButton* quit_button;
    GtkButtonBox* chapter_button_box;
    GtkScrolledWindow* scrolled_window;
    GtkScrolledWindow* scrolled_window_chapters;
    GtkTextBuffer* buffer;
    GtkTextView* text_view;
    GtkWidget *hbox_top;
    GtkWidget *hbox_bottom;
    GtkWidget *vbox;
    GtkWidget *wrapper_box;
    GtkWidget *window;
} AppState;


/******************************************************************************
*                                                                             *
*                         function definitions                                *
*                                                                             *
******************************************************************************/

/**
Precondition:   Takes AppState struct and pointer to button clicked.
Postcondition:  Opens a file dialog, error checks user input, creates a new
                buffer and updates textview and chapter buttons.
*/
void on_open_button(GtkWidget *button, AppState *app_state);

/**
Precondition:   Takes AppState struct and pointer to button clicked.
Postcondition:  Clears the buuffer and chapter buttons from display.
*/
void on_close_button(GtkWidget *button, AppState *app_state);

/**
Postcondition:  Quits application.
*/
void on_quit_button(GtkWidget *button);

/**
Precondition:   Takes AppState struct.
Postcondition:  Initialises and populates tag table.
*/
void tag_table_init(GtkTextBuffer* buffer);

/**
Note:           Function and signature should be revised, buffer and
                tags are members of app_state.
Precondition:   Takes buffer, GList of tags and pointer to head node of
                xml tag tree.
Postcondition:  Iterates through the xml tree and styles text according
                to tags defined in tags list and tag_table_init function.
*/
void style_buffer(AppState* app_state, GtkTextBuffer* buffer, GList* tags,
                  struct Binary_node* head_node);

/**
Postcondition:  Helper function for tag_table_init.  Populages chapter
                buttons and menu.
*/
void add_chapter_button(AppState* app_state, struct Binary_node* chapter_node);


int main( int argc, char *argv[])

{
    gtk_init(&argc, &argv);

/******************************************************************************
*                                                                             *
*                         data / main containers                              *
*                                                                             *
******************************************************************************/
    AppState app_state;

    GtkButtonBox *button_box;

    app_state.tags = NULL;
    app_state.tags = g_list_append(app_state.tags, "<b>");
    app_state.tags = g_list_append(app_state.tags, "<i>");
    app_state.tags = g_list_append(app_state.tags, "<tt>");
    app_state.tags = g_list_append(app_state.tags, "<h>");

    app_state.hbox_top = gtk_hbox_new (FALSE, 2);
    app_state.hbox_bottom = gtk_hbox_new (FALSE, 2);
    app_state.vbox = gtk_vbox_new (FALSE, 2);

/******************************************************************************
*                                                                             *
*                           text_view/buffer init.                            *
*                                                                             *
******************************************************************************/

    app_state.text_view = gtk_text_view_new ();
    gtk_text_view_set_left_margin(app_state.text_view, 10);
    gtk_text_view_set_right_margin(app_state.text_view, 10);
    app_state.buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW
                                                 (app_state.text_view));

    tag_table_init(app_state.buffer);

/******************************************************************************
*                                                                             *
*                                  chapter buttons                            *
*                                                                             *
******************************************************************************/

    app_state.wrapper_box = gtk_vbutton_box_new ();

    app_state.chapter_button_box = gtk_vbutton_box_new ();

    gtk_button_box_set_layout(app_state.chapter_button_box, GTK_BUTTONBOX_START);
    gtk_box_set_spacing (app_state.chapter_button_box, 5);
    gtk_container_set_border_width (app_state.chapter_button_box, 10);

    app_state.scrolled_window_chapters = gtk_scrolled_window_new(NULL,NULL);

    gtk_widget_set_size_request (app_state.scrolled_window_chapters,
                                 WINWIDTH/4, -1);

    gtk_scrolled_window_add_with_viewport(app_state.scrolled_window_chapters,
                      app_state.wrapper_box);

    gtk_scrolled_window_set_policy(app_state.scrolled_window_chapters,
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);

/******************************************************************************
*                                                                             *
*                                  windows                                    *
*                                                                             *
******************************************************************************/

    app_state.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title              (GTK_WINDOW (app_state.window),
                                      "Text Viewer");
    gtk_window_set_default_size(GTK_WINDOW (app_state.window),
                                       WINWIDTH, WINHEIGHT);

    app_state.scrolled_window = gtk_scrolled_window_new(NULL,NULL);
    gtk_container_add(app_state.scrolled_window, app_state.text_view);
    gtk_scrolled_window_set_policy(app_state.scrolled_window,
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);

    gtk_container_add(GTK_CONTAINER(app_state.window),app_state.vbox);
    gtk_box_pack_start(GTK_BOX(app_state.wrapper_box),
                       app_state.chapter_button_box,
                        TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(app_state.hbox_top),
                       app_state.scrolled_window_chapters,
                       FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(app_state.hbox_top),
                       app_state.scrolled_window, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(app_state.vbox),
                       app_state.hbox_top, TRUE, TRUE, 0);///


/******************************************************************************
*                                                                             *
*                                  bottom menu buttons                        *
*                                                                             *
******************************************************************************/

    button_box = gtk_hbutton_box_new ();

    gtk_button_box_set_layout(GTK_BUTTON_BOX(button_box), GTK_BUTTONBOX_START);
    gtk_box_set_spacing (GTK_BOX (button_box), 5);
    gtk_container_set_border_width (GTK_CONTAINER (button_box), 10);

    app_state.open_button = gtk_button_new_from_stock (GTK_STOCK_OPEN);
    app_state.close_button = gtk_button_new_from_stock(GTK_STOCK_CLOSE);
    app_state.quit_button = gtk_button_new_from_stock(GTK_STOCK_QUIT);

    gtk_widget_set_sensitive(app_state.close_button, FALSE );

    gtk_box_pack_start (GTK_BOX(button_box), app_state.open_button,
                        FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX(button_box), app_state.close_button,
                        FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX(button_box), app_state.quit_button,
                        FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX(app_state.vbox),
                        GTK_BOX(button_box),
                        FALSE, FALSE, 0);

/******************************************************************************
*                                                                             *
*                                  signal connecting                          *
*                                                                             *
******************************************************************************/

    g_signal_connect (G_OBJECT (app_state.open_button), "clicked",
                      G_CALLBACK (on_open_button),
                      &app_state);

    g_signal_connect (G_OBJECT (app_state.close_button), "clicked",
                      G_CALLBACK (on_close_button),
                      &app_state);

    g_signal_connect (G_OBJECT (app_state.quit_button), "clicked",
                      G_CALLBACK (on_quit_button),
                      NULL);

    g_signal_connect(G_OBJECT(app_state.window), "destroy",
                     G_CALLBACK(gtk_main_quit),
                     NULL);


    gtk_widget_show_all (app_state.window);

    gtk_main ();
    return(0);
}

void on_open_button( GtkWidget *button, AppState *app_state)
{
    char* file_contents;
    GtkTextIter startIter;
    GtkTextIter endIter;

    struct Binary_node* head_node;
    head_node = xml_parser_initialize_head(head_node);

/******************************************************************************
                         load file
                         create buffer
*******************************************************************************/

    file_contents = open_and_get_file_contents(app_state->window,
                                               "Text Files",
                                               "text/*");
    if(file_contents!=NULL)
    {
        app_state->buffer = gtk_text_view_get_buffer(app_state->text_view);
        gtk_text_buffer_set_text(app_state->buffer, file_contents, -1);
    }
    else{return;}

/******************************************************************************
                         build xml tag tree from buffer
*******************************************************************************/
    gtk_text_buffer_get_bounds(app_state->buffer, &startIter, &endIter);

    build_xml_tree(app_state->buffer, head_node, app_state->tags, startIter,
                   endIter);

/******************************************************************************
                         iterate xml tag tree and call style function on
                         each node
*******************************************************************************/

    if(app_state->chapter_button_box!=NULL)
    {

        gtk_widget_destroy(app_state->chapter_button_box);
    }



    app_state->chapter_button_box = gtk_vbutton_box_new ();

    gtk_button_box_set_layout(app_state->chapter_button_box, GTK_BUTTONBOX_START);
    gtk_box_set_spacing (app_state->chapter_button_box, 5);
    gtk_container_set_border_width (app_state->chapter_button_box, 10);

    gtk_box_pack_start(GTK_BOX(app_state->wrapper_box), app_state->chapter_button_box,
                        FALSE, FALSE, 0);///



    style_buffer(app_state, app_state->buffer, app_state->tags, head_node);

    gtk_text_view_set_editable (app_state->text_view, FALSE);

/******************************************************************************
                                 destroy tree
*******************************************************************************/

    destroy(head_node);

    gtk_widget_set_sensitive(app_state->open_button, FALSE);
    gtk_widget_set_sensitive(app_state->close_button, TRUE);

    gtk_widget_show_all(app_state->window);

    return;
}

void style_buffer(AppState* app_state, GtkTextBuffer* buffer, GList* tags,
                  struct Binary_node* head_node)
{
    GList* place_holder;
    struct Binary_node* treeIterator = head_node;

    while(treeIterator!=NULL)
    {
        treeIterator = iterate(head_node, treeIterator);
        if(treeIterator!=NULL)
        {
            place_holder = tags;

            while(place_holder!=NULL)
            {
                if(strcmp(place_holder->data,treeIterator->data->tag_name)==0)
                {
                    if(strcmp(treeIterator->data->tag_name,"<h>")==0)
                    {
                        add_chapter_button(app_state, treeIterator);
                        printf(treeIterator->data->tag_name);///            do something here to add bookmark
                    }
                    else
                    {
                        gtk_text_buffer_apply_tag_by_name (buffer,
                                                           treeIterator->data->tag_name,
                                                           &treeIterator->data->start_of_start_tag,
                                                           &treeIterator->data->end_of_end_tag);
                    }
                    gtk_text_buffer_apply_tag_by_name (buffer,
                                                       "invisible",
                                                       &treeIterator->data->start_of_start_tag,
                                                       &treeIterator->data->end_of_start_tag);
                    gtk_text_buffer_apply_tag_by_name (buffer,
                                                       "invisible",
                                                       &treeIterator->data->start_of_end_tag,
                                                       &treeIterator->data->end_of_end_tag);
                    place_holder = NULL;
                }
                else
                {
                    place_holder = g_list_next(place_holder);
                }
            }
        }
    }
}

void add_chapter_button(AppState* app_state, struct Binary_node* chapter_node)
{
    ///Todo: functionality to scroll to chapter

    GtkButton* new_chapter_button;
    new_chapter_button = gtk_button_new_with_label
                        (gtk_text_iter_get_text(&chapter_node->data->end_of_start_tag,
                                                &chapter_node->data->start_of_end_tag));
    gtk_box_pack_start (app_state->chapter_button_box, new_chapter_button,
                        FALSE, FALSE, 0);
}

//Additional tags can be defined here
void tag_table_init(GtkTextBuffer* buffer)
{
    gtk_text_buffer_create_tag(buffer,"<i>","style",PANGO_STYLE_ITALIC, NULL);
    gtk_text_buffer_create_tag(buffer, "<tt>", "family", "monospace", NULL);
    gtk_text_buffer_create_tag(buffer, "<b>", "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_buffer_create_tag(buffer, "invisible", "invisible", TRUE, NULL);
}

void on_close_button(GtkWidget *button, AppState *app_state)
{

    gtk_widget_destroy(app_state->chapter_button_box);
    app_state->chapter_button_box=NULL;

    GtkTextBuffer  *buffer;
    buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(app_state->text_view));

    GtkTextIter startIter;
    GtkTextIter endIter;

    gtk_text_buffer_get_bounds(buffer, &startIter, &endIter);
    gtk_text_buffer_delete(buffer, &startIter, &endIter);

    gtk_widget_set_sensitive(app_state->close_button, FALSE );
    gtk_widget_set_sensitive(app_state->open_button, TRUE );
}

void on_quit_button(GtkWidget *button)
{
      gtk_main_quit ();
}
