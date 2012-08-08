#include "open_file_get_buffer.h"


char* open_and_get_file_contents(GtkWidget *window,
                                 char* filter_name,
                                 char* mime_type)
{

    char* file_contents_local;

    GtkFileFilter  *filter;
    GtkWidget      *dialog;
    int            result;
    gchar          *filename;
    GError         *error = NULL;
    guint          nBytesInBuf;
    gchar          *contents;

    dialog = gtk_file_chooser_dialog_new ("Select File...",
                                    ///GTK_WINDOW (window),
                                    window,
                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                    GTK_STOCK_OK,
                                    GTK_RESPONSE_ACCEPT,
                                    GTK_STOCK_CANCEL,
                                    GTK_RESPONSE_CANCEL,
                                    NULL);

    filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, filter_name);
    gtk_file_filter_add_mime_type(filter, mime_type);
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog),
                                         g_get_home_dir());

    ///Run the dialog modally and get the user response
    result = gtk_dialog_run (GTK_DIALOG (dialog));
    switch (result) {
    case GTK_RESPONSE_ACCEPT:
         filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
         gtk_widget_destroy (dialog);
         break;
    case GTK_RESPONSE_DELETE_EVENT:
    case GTK_RESPONSE_CANCEL:
    case GTK_RESPONSE_NONE:
         gtk_widget_destroy (dialog);
         return;
    }

    ///This should not happen, but to be safe
    if (NULL == filename) {
        GtkWidget *msg;
        msg = gtk_message_dialog_new (GTK_WINDOW (window),
                                      GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
                                      "Failed to get file");
        gtk_dialog_run (GTK_DIALOG (msg));
        gtk_widget_destroy (msg);
        return NULL;
    }

    gtk_window_set_title(GTK_WINDOW (window), filename);

    if (!g_file_get_contents( filename, &file_contents_local, &nBytesInBuf, &error)) {
        g_printf(error->message);
        g_clear_error(&error);
        g_free(filename);
        return NULL;
    }

    g_free(filename);

    return(file_contents_local);
}
