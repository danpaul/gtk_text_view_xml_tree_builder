#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <libgen.h>
#include <gdk/gdkkeysyms.h>


/******************************************************************************
Overview:   Parses strictly formatted XML into a binary implementation of a
            N-ary tree which preseves the XML hierarchy.  This is intended
            to take an arbitrary list of XML tags an make a tree with it.
*******************************************************************************/

struct Node_data
{
    gchar* tag_name;
    GtkTextIter start_of_start_tag;
    GtkTextIter end_of_start_tag;
    GtkTextIter start_of_end_tag;
    GtkTextIter end_of_end_tag;
};

struct Binary_node
{
    gboolean iterator_marker;
    struct Node_data* data;
    struct Binary_node* first_child;
    struct Binary_node* next_sibling;
    struct Binary_node* parent;
    struct Binary_node* previous_sibling;
};

/******************************************************************************
PreCondition:   Takes a buffer, pointer to a Binary_node struct(defined below)
                a GList of start tags and the begin and end of the section of
                the buffer to parse.

PostCondition:  Parses xml into a binary node tree that preserves the structure
                order and hierarchy of the xml.

Note:           Does no error checking of xml formatting.
*******************************************************************************/
void build_xml_tree(GtkTextBuffer* buffer, struct Binary_node* parent, GList* tags,
                    GtkTextIter startIter, GtkTextIter endIter);

/******************************************************************************
PreCondition:   Takes a pointer to the tree's head and the current position
                of the iterator.
PostCondition:  If iterator points to head, next iterator is returned.
                After entire tree has been iterated, NULL is returned.
Note:           Iterates in xml order.
                I.e.: It recursively returns "current node" followed by
                all children nodes, followed by all sibling nodes.
*******************************************************************************/
struct Binary_node* iterate(struct Binary_node* head,
                            struct Binary_node* iterator);

///Destroys entire tree including head and frees its memroy.
void destroy(struct Binary_node* head);

///Initializes head values to NULL.
struct Binary_node* xml_parser_initialize_head(struct Binary_node* head);
