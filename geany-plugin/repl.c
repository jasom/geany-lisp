#include "local.h"

static struct ReplInfo {
    GtkWidget *container;
    GtkWidget *output;
    GtkWidget *prompt;
    GtkWidget *input;
    GtkTextBuffer *outputBuffer;
    GtkTextBuffer *inputBuffer;
    guint lastPosition;

} ReplInfo;

static gboolean replPoll (gpointer user_data);
static gchar* getOutputLines(guint startPosition);
static gchar* getPrompt();
gboolean glisp_repl_key_pressed (G_GNUC_UNUSED GtkWidget *widget,
        GdkEvent  *event, G_GNUC_UNUSED gpointer user_data);

void glispCreateReplUi()
{
    fprintf(stderr,"\nNotebook: %p\n",ui_lookup_widget(geany_data->main_widgets->window, "notebook_info"));
    fprintf(stderr,"\nNotebook 2: %p\n",geany_data->main_widgets->message_window_notebook);

    GtkWidget* ReplTabLabel = gtk_label_new("REPL");

    GtkBuilder *b = gtk_builder_new_from_file(GLISP_TOOLS_BASE "/repl.glade");
    gtk_builder_add_callback_symbols (b, "glisp_repl_key_pressed",G_CALLBACK(glisp_repl_key_pressed),NULL);
    gtk_builder_connect_signals(b,NULL);

    ReplInfo.container = GTK_WIDGET(gtk_builder_get_object(b, "glisp_repl_parent"));
    ReplInfo.output = GTK_WIDGET(gtk_builder_get_object(b, "glisp_repl_output"));
    ReplInfo.outputBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ReplInfo.output));
    ReplInfo.prompt = GTK_WIDGET(gtk_builder_get_object(b, "glisp_repl_prompt"));
    ReplInfo.input = GTK_WIDGET(gtk_builder_get_object(b, "glisp_repl_input"));
    ReplInfo.inputBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(ReplInfo.input));
    ReplInfo.lastPosition = 1;

    g_timeout_add(250, replPoll, NULL);

    gtk_notebook_append_page(GTK_NOTEBOOK(geany_data->main_widgets->message_window_notebook),
            ReplInfo.container, ReplTabLabel);
    gtk_widget_show_all(ReplInfo.container);

}


static gboolean replPoll (G_GNUC_UNUSED gpointer user_data)
{

    gchar *text = getOutputLines(ReplInfo.lastPosition);
    ReplInfo.lastPosition += strlen(text);

    GtkTextIter end;

    /* TODO use a mark instead of cursor? */

    gtk_text_buffer_get_end_iter(ReplInfo.outputBuffer,&end);
    gtk_text_buffer_place_cursor(ReplInfo.outputBuffer,&end);

    gtk_text_buffer_insert_at_cursor(ReplInfo.outputBuffer, text, strlen(text));

    if(strlen(text) > 0) {
        GtkTextMark *cursor = gtk_text_buffer_get_insert(ReplInfo.outputBuffer);
        gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(ReplInfo.output),cursor, 0,FALSE,0,0);
    }

    g_free(text);

    text = getPrompt();

    gtk_label_set_text(GTK_LABEL(ReplInfo.prompt), text);

    g_free(text);

    return TRUE;
}

static gchar* getOutputLines(guint startPosition)
{
    gchar *argv[4] = {GLISP_UTILITY,"repl-output",NULL, NULL};

    gchar *arg2 = g_strdup_printf("%u",startPosition);
    gchar *result = NULL;
    GError *E=NULL;

    argv[2] = arg2;
    
    GString *output = g_string_sized_new(1024);
    if (! spawn_sync(NULL,NULL,argv,NULL,NULL,output, NULL, NULL, &E))
    {

        g_assert(E);
        fprintf(stderr, "Unable to get completions: %s\n",E->message);
        goto cleanup;
    }

    result = g_string_free(output,FALSE);

cleanup:
    g_clear_error(&E);
    return result;
}

static gchar* getPrompt()
{
    gchar *argv[3] = {GLISP_UTILITY,"repl-prompt", NULL};

    GError *E=NULL;

    GString *output = g_string_sized_new(1024);
    if (! spawn_sync(NULL,NULL,argv,NULL,NULL,output, NULL, NULL, &E))
    {
        g_assert(E);
        fprintf(stderr, "Unable to get prompt: %s\n",E->message);
        goto cleanup;
    }


cleanup:
    g_clear_error(&E);
    return g_string_free(output,FALSE);
}

static gchar* evaluateInput(gchar *input, guint offset)
{
    gchar* offsetString = g_strdup_printf("%u", offset);

    gchar* argv[] = {GLISP_UTILITY,"repl-send-input", input, offsetString, NULL };

    GError *E=NULL;

    GString *output = g_string_sized_new(1024);
    if (! spawn_sync(NULL,NULL,argv,NULL,NULL,output, NULL, NULL, &E))
    {
        g_assert(E);
        fprintf(stderr, "Unable to send input: %s\n",E->message);
        goto cleanup;
    }

cleanup:
    g_clear_error(&E);
    g_free(offsetString);
    return g_string_free(output,FALSE);
}


gboolean glisp_repl_key_pressed (GtkWidget *widget, GdkEvent  *event, G_GNUC_UNUSED gpointer user_data)
{
    guint keyval;

    g_assert(widget == ReplInfo.input);

    g_assert(gdk_event_get_keyval(event, &keyval));

    if(keyval == GDK_KEY_Return) {
        fprintf(stderr, "Return Pressed\n");
        GtkTextIter start, cursor, end;
        gtk_text_buffer_get_start_iter(ReplInfo.inputBuffer,&start);
        gtk_text_buffer_get_end_iter(ReplInfo.inputBuffer,&end);
        gtk_text_buffer_get_iter_at_mark(ReplInfo.inputBuffer, &cursor,
                gtk_text_buffer_get_insert(ReplInfo.inputBuffer));

        //guint offset = cursorDifference(start,cursor);
        guint offset = gtk_text_iter_get_offset(&cursor);

        gchar *text = gtk_text_buffer_get_text(ReplInfo.inputBuffer,
                &start,
                &end,
                FALSE);

        gchar *newInput = evaluateInput(text, offset);
        gtk_text_buffer_set_text(ReplInfo.inputBuffer,newInput,-1);
        g_free(text);
        g_free(newInput);
        return TRUE;
    }
    return FALSE;
}
