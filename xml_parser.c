#include "xml_parser.h"


struct Binary_node* xml_parser_initialize_head(struct Binary_node* head)
{
    head = malloc(sizeof(struct Binary_node));///remember to free

    head->data = NULL;
    head->first_child = NULL;
    head->next_sibling = NULL;
    head->next_sibling = NULL;
    head->parent = NULL;

    return(head);

}

void destroy(struct Binary_node* head)
{
    if(head->first_child!=NULL)
    {
        destroy(head->first_child);
    }
    if(head->next_sibling!=NULL)
    {
        destroy(head->next_sibling);
    }
    if(head->parent==NULL)
    {
        free(head->data);
        free(head);
    }
    else
    {
        free(head->data->tag_name);
        free(head->data);
        free(head);
    }
}

struct Binary_node* iterate(struct Binary_node* head,
                            struct Binary_node* iterator)
{
    ///takes head and iterator
    ///if head is passed in, as iterator, starts from the begining
    ///updates iterator
    ///returns in order of this, child, sibling
    if(iterator==NULL){return NULL;}
    if(iterator==head)
    {
        if(head->first_child==NULL){return NULL;}
        else{return head->first_child;}
    }
    else
    {
        if(iterator->first_child!=NULL)
            {return iterator->first_child;}
        else if(iterator->next_sibling!=NULL)
            {return iterator->next_sibling;}
        else
        {
            ///trace back to first sibling
            while(TRUE)
            {
                if(iterator->previous_sibling!=NULL)
                {
                    iterator = iterator->previous_sibling;
                }
                else
                {
                    while(iterator!=head)
                    {
                        iterator = iterator->parent;
                        if(iterator->next_sibling!=NULL)
                            {return iterator->next_sibling;}
                    }
                    return NULL;
                }
            }
        }
    }
}

void build_xml_tree(GtkTextBuffer* buffer, struct Binary_node* parent, GList* tags,
                    GtkTextIter startIter, GtkTextIter endIter)
{
    GtkTextIter start_of_start_tag, end_of_start_tag, place_holder;

    GtkTextIter start_of_end_tag, end_of_end_tag;

    place_holder = startIter;

    GList *next;

    gboolean found_matching_tag;

    struct Binary_node* last_sibling;
    last_sibling = NULL;

    ///Find the potential start of a tag.
    while(gtk_text_iter_forward_search (&place_holder, "<", 0, &place_holder,
                                     &end_of_start_tag, &endIter))
    {
        found_matching_tag = FALSE;
        next = tags;
        ///Iterate through tags in tags GList.
        while(next!=NULL)
        {
            start_of_start_tag = place_holder;
            gint tagLength = strlen(next->data);
            end_of_start_tag = start_of_start_tag;
            if(gtk_text_iter_forward_chars(&end_of_start_tag,tagLength))
            {
                ///Try to match tag in tags to tag in buffer.
                if(gtk_text_iter_forward_search (&start_of_start_tag,
                                                       next->data,
                                                       0,
                                                       &start_of_start_tag,
                                                       &end_of_start_tag,
                                                       &end_of_start_tag))
                {
                    ///Create an end tag.
                    gchar* start_tag = malloc((tagLength+1)*sizeof(gchar));///
                    gchar* end_tag = malloc((tagLength+2)*sizeof(gchar));///
                    gchar* tmp_string = malloc((tagLength+2)*sizeof(gchar));///

                    strcpy(start_tag, gtk_text_iter_get_text
                           (&start_of_start_tag, &end_of_start_tag));


                    strcpy(tmp_string,start_tag);
                    char end_tag_string[20];

                    end_tag_string[0] = '<';
                    end_tag_string[1] = '/';
                    int count = 1;

                    do
                    {
                        end_tag_string[count+1] = start_tag[count];
                        count++;
                    }while(start_tag[count]!='>');
                    end_tag_string[count+1] = '>';
                    end_tag_string[count+2] = '\0';

                    ///Try to match end tag.
                    if(gtk_text_iter_forward_search (&end_of_start_tag,
                                                     end_tag_string,
                                                     0,
                                                     &start_of_end_tag,
                                                     &end_of_end_tag,
                                                     &endIter));
                    {
                        found_matching_tag = TRUE;

                        end_tag = gtk_text_iter_get_text(&start_of_end_tag, &end_of_end_tag);

                        struct Node_data* new_node_data;///
                        new_node_data = malloc(sizeof(struct Node_data));///

                        new_node_data->tag_name = start_tag;
                        new_node_data->start_of_start_tag = start_of_start_tag;
                        new_node_data->end_of_start_tag = end_of_start_tag;
                        new_node_data->start_of_end_tag = start_of_end_tag;
                        new_node_data->end_of_end_tag = end_of_end_tag;

                        struct Binary_node* new_node;
                        new_node = malloc(sizeof(struct Binary_node));

                        new_node->data = new_node_data;
                        new_node->first_child=NULL;
                        new_node->next_sibling=NULL;
                        new_node->previous_sibling=last_sibling;
                        new_node->parent=parent;

                        if(parent->first_child==NULL)
                        {
                            parent->first_child = new_node;
                            last_sibling = new_node;
                        }

                        else
                        {
                            last_sibling->next_sibling = new_node;
                            last_sibling = new_node;
                        }

                        build_xml_tree(buffer, new_node, tags,end_of_start_tag,start_of_end_tag);

                    }

                free(end_tag);
                free(tmp_string);
                }
            }
            if(found_matching_tag){next=NULL;}
            else{next = g_list_next(next);}
        }

    ///increment place_holder to end of end tag if match was found
    ///or 1 place if not
    if(found_matching_tag==TRUE)
    {
        place_holder = end_of_end_tag;
    }
    else
    {
        if(gtk_text_iter_forward_chars(&place_holder,1)){;}
        else{place_holder=endIter;}
    }
}
}

