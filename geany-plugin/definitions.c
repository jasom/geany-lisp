#include "local.h"

static void goToFilePosition(gchar *filename, gsize position);

typedef struct symbolLocation {
    GString *filename;
    GString *definitionName;
    gsize position;
} SymbolLocation;


static SymbolLocation *newSymbolLocation()
{
    SymbolLocation *s = g_new(SymbolLocation,1);
    s->filename = g_string_new("");
    s->definitionName = g_string_new("");
    s->position=0;
    return s;
}

static void deleteSymbolLocation(SymbolLocation *s)
{
    g_string_free(s->filename,TRUE);
    g_string_free(s->definitionName,TRUE);
    g_free(s);
}

static gboolean readSymbolLocation(GPtrArray *input, guint *idx, GPtrArray *output)
{
    SymbolLocation *s = newSymbolLocation();
    GString *tmp=NULL;
    gboolean retval = FALSE;
    guint i = *idx;
    
    if(i >= input->len) {
        goto error;
    }

    if(i+3 > input->len) {
        fprintf(stderr,"EOF in middle of symbol location info\n");
        //Free spurious lines
        g_ptr_array_remove_range(input,i,input->len-i);
        goto error;
    }

    s->definitionName=g_ptr_array_index(input,i);
    g_ptr_array_index(input,i)=NULL;
    i++;

    s->filename=g_ptr_array_index(input,i);
    g_ptr_array_index(input,i)=NULL;
    i++;

    tmp = g_ptr_array_index(input,i);
    s->position = g_ascii_strtoull(tmp->str,NULL,10);
    g_string_free(tmp,TRUE);
    g_ptr_array_index(input,i)=NULL;
    i++;

    g_ptr_array_add(output,s);

    retval=TRUE;
    goto cleanup;

error:
    deleteSymbolLocation(s);

cleanup:
    *idx=i;
    return retval;
}

static GPtrArray * readSymbolLocations(GPtrArray *input)
{
    GPtrArray *locations = g_ptr_array_new_with_free_func((GDestroyNotify)deleteSymbolLocation);
    guint index=0;

    while(readSymbolLocation(input, &index, locations));

    return locations;
}

/* Next several functions are taken from symbols.c */

/* positions a popup at the caret from the ScintillaObject in @p data */
static void goto_popup_position_func(GtkMenu *menu, gint *x, gint *y, gboolean *push_in, gpointer data)
{
        gint line_height;
        GdkScreen *screen = gtk_widget_get_screen(GTK_WIDGET(menu));
        gint monitor_num;
        GdkRectangle monitor;
        GtkRequisition req;
        GdkEventButton *event_button = g_object_get_data(G_OBJECT(menu), "geany-button-event");

        if (event_button)
        {
                /* if we got a mouse click, popup at that position */
                *x = (gint) event_button->x_root;
                *y = (gint) event_button->y_root;
                line_height = 0; /* we don't want to offset below the line or anything */
        }
        else /* keyboard positioning */
        {
                ScintillaObject *sci = data;
                GdkWindow *window = gtk_widget_get_window(GTK_WIDGET(sci));
                gint pos = sci_get_current_position(sci);
                gint line = sci_get_line_from_position(sci, pos);
                gint pos_x = scintilla_send_message(sci, SCI_POINTXFROMPOSITION, 0, pos);
                gint pos_y = scintilla_send_message(sci, SCI_POINTYFROMPOSITION, 0, pos);

                line_height = scintilla_send_message(sci, SCI_TEXTHEIGHT, line, 0);

                gdk_window_get_origin(window, x, y);
                *x += pos_x;
                *y += pos_y;
        }

        monitor_num = gdk_screen_get_monitor_at_point(screen, *x, *y);

#if GTK_CHECK_VERSION(3, 0, 0)
        gtk_widget_get_preferred_size(GTK_WIDGET(menu), NULL, &req);
#else
        gtk_widget_size_request(GTK_WIDGET(menu), &req);
#endif

#if GTK_CHECK_VERSION(3, 4, 0)
        gdk_screen_get_monitor_workarea(screen, monitor_num, &monitor);
#else
        gdk_screen_get_monitor_geometry(screen, monitor_num, &monitor);
#endif

        /* put on one size of the X position, but within the monitor */
        if (gtk_widget_get_direction(GTK_WIDGET(menu)) == GTK_TEXT_DIR_RTL)
        {
                if (*x - req.width - 1 >= monitor.x)
                        *x -= req.width + 1;
                else if (*x + req.width > monitor.x + monitor.width)
                        *x = monitor.x;
                else
                        *x += 1;
        }
        else
        {
                if (*x + req.width + 1 <= monitor.x + monitor.width)
                        *x = MAX(monitor.x, *x + 1);
                else if (*x - req.width - 1 >= monitor.x)
                        *x -= req.width + 1;
                else
                        *x = monitor.x + MAX(0, monitor.width - req.width);
        }

        /* try to put, in order:
         * 1. below the Y position, under the line
         * 2. above the Y position
         * 3. within the monitor */
        if (*y + line_height + req.height <= monitor.y + monitor.height)
                *y = MAX(monitor.y, *y + line_height);
        else if (*y - req.height >= monitor.y)
                *y = *y - req.height;
        else
                *y = monitor.y + MAX(0, monitor.height - req.height);

        *push_in = FALSE;
}


static void on_goto_popup_item_activate(G_GNUC_UNUSED GtkMenuItem *item, SymbolLocation *sl)
{
    goToFilePosition(sl->filename->str,sl->position);
}


static void show_goto_popup(GeanyDocument *doc, GPtrArray *tags, gboolean have_best)
{
        GtkWidget *first = NULL;
        GtkWidget *menu;
        GdkEvent *event;
        GdkEventButton *button_event = NULL;
        SymbolLocation *sl;

 
        guint i;

        menu = gtk_menu_new();

        foreach_ptr_array(sl, i, tags)
        {
                GtkWidget *item;
                gchar *fname = g_path_get_basename(sl->filename->str);
                gchar *text;

                if (! first && have_best)
                        /* For translators: it's the filename and line number of a symbol in the goto-symbol popup menu */
                        text = g_markup_printf_escaped(_("<b>%s: %lu</b>"), fname, sl->position);
                else
                        /* For translators: it's the filename and line number of a symbol in the goto-symbol popup menu */
                        text = g_markup_printf_escaped(_("%s: %lu"), fname, sl->position);

                //image = gtk_image_new_from_pixbuf(symbols_icons[get_tag_class(tmtag)].pixbuf);
                //label = g_object_new(GTK_TYPE_LABEL, "label", text, "use-markup", TRUE, "xalign", 0.0, NULL);
                item = g_object_new(GTK_TYPE_MENU_ITEM, "label", text, NULL);
                g_signal_connect_data(item, "activate", G_CALLBACK(on_goto_popup_item_activate),
                                      sl, (GClosureNotify) deleteSymbolLocation, 0);
                gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

                if (! first)
                        first = item;

                g_free(text);
                g_free(fname);
        }

        gtk_widget_show_all(menu);

        if (first) /* always select the first item for better keyboard navigation */
                g_signal_connect(menu, "realize", G_CALLBACK(gtk_menu_shell_select_item), first);

        event = gtk_get_current_event();
        if (event && event->type == GDK_BUTTON_PRESS) button_event = (GdkEventButton *) event;
        else if (event) gdk_event_free(event);

        g_object_set_data_full(G_OBJECT(menu), "geany-button-event", button_event,
                               button_event ? (GDestroyNotify) gdk_event_free : NULL);
        gtk_menu_popup(GTK_MENU(menu), NULL, NULL, goto_popup_position_func, doc->editor->sci,
                                   button_event ? button_event->button : 0, gtk_get_current_event_time ());
}


/* Taken verbatim from editor.c */
/* TODO switch to using GString vs buffer+len */
#define IS_ASCII(c) (((unsigned char)(c)) < 0x80)

static void read_current_word(GeanyEditor *editor, gint pos, gchar *word, gsize wordlen,
                const gchar *wc, gboolean stem)
{
    gint line, line_start, startword, endword;
    gchar *chunk;
    ScintillaObject *sci;

    g_return_if_fail(editor != NULL);
    sci = editor->sci;

    if (pos == -1)
        pos = sci_get_current_position(sci);

    line = sci_get_line_from_position(sci, pos);
    line_start = sci_get_position_from_line(sci, line);
    startword = pos - line_start;
    endword = pos - line_start;

    word[0] = '\0';
    chunk = sci_get_line(sci, line);

    if (wc == NULL)
        wc = GEANY_WORDCHARS;

    /* the checks for "c < 0" are to allow any Unicode character which should make the code
     * a little bit more Unicode safe, anyway, this allows also any Unicode punctuation,
     * TODO: improve this code */
    while (startword > 0 && (strchr(wc, chunk[startword - 1]) || ! IS_ASCII(chunk[startword - 1])))
        startword--;
    if (!stem)
    {
        while (chunk[endword] != 0 && (strchr(wc, chunk[endword]) || ! IS_ASCII(chunk[endword])))
            endword++;
    }

    if (startword != endword)
    {
        chunk[endword] = '\0';

        g_strlcpy(word, chunk + startword, wordlen); /* ensure null terminated */
    }
    else
        g_strlcpy(word, "", wordlen);

    g_free(chunk);
}

const char *lispWordChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijlkmnopqrstuvwxyz1234567890*&^%$@!-_+=:/?<>";

static void goToFilePosition(gchar *filename, gsize position)
{
    GeanyDocument *old_doc = document_get_current();
    GeanyDocument *new_doc;


    new_doc = document_find_by_real_path(filename);

    if (new_doc == NULL) {
        /* not found in opened document, should open */
        new_doc = document_open_file(filename, FALSE, NULL, NULL);
    }

    if (new_doc != NULL) {
        gint lineno=sci_get_line_from_position(new_doc->editor->sci, position) + 1;

        navqueue_goto_line(old_doc, new_doc, lineno);
    }
}

static void exit_cb(G_GNUC_UNUSED GPid pid, gint status, GPtrArray *stdout_lines)
{
    GError *E=NULL;
    GPtrArray *results = NULL;
    SymbolLocation *sl=NULL;
    GeanyDocument* doc = document_get_current();

    if(!g_spawn_check_exit_status(status,&E)) {
        /* TODO log to messages? */
        fprintf(stderr, "definitionjump exited with error: %s\n",E->message);
        goto cleanup;
    }


    results = readSymbolLocations(stdout_lines);

    /* DEBUG information
    foreach_ptr_array(sl, i, results) {
        fprintf(stderr,"Found Location; FILENAME='%s'; LINENUMBER=%lu; LABEL='%s'\n",
                sl->filename->str, sl->position, sl->definitionName->str);
    }
    */

    switch(results->len) {
        case 0:
            break;
        case 1:
            sl=g_ptr_array_index(results,0);
            goToFilePosition(sl->filename->str,sl->position);
            break;
        default:
            // The popup takes ownership of the SymbolLocations
            g_ptr_array_set_free_func(results,NULL);
            show_goto_popup(doc, results, FALSE);
    }
    
cleanup:
    g_clear_error(&E);
    if(results) g_ptr_array_free(results,TRUE);
    g_ptr_array_free(stdout_lines,TRUE);
}

    
void glispKbRunJump(G_GNUC_UNUSED guint key_id)
{
    GeanyDocument* doc = document_get_current();
    GeanyEditor* editor;
    GError *E=NULL;
    G_GNUC_UNUSED ScintillaObject *sci;
    const gint MAX_WORD_SIZE=1024;
    char *argv[4] = {0};
    GPtrArray *inputBuffer = g_ptr_array_new_with_free_func((GDestroyNotify)glispStringDestroy);


    gchar *word =g_malloc(MAX_WORD_SIZE);
    gchar *package = NULL;

    if(!doc || !doc->editor || ! doc->editor->sci) {
        return;
    }
    editor = doc->editor;
    sci = editor->sci;

    package = glispSearchBufferPackage(sci);

    read_current_word(editor, -1, word, MAX_WORD_SIZE, lispWordChars, FALSE);

    argv[0] = GLISP_TOOLS_BASE "/definitionjump";
    argv[1] = word;
    argv[2] = package;
    argv[3] = NULL;

    if (! spawn_with_callbacks(NULL,NULL,argv,NULL,0,
            NULL,NULL,
            (SpawnReadFunc)glispSlurpCb,inputBuffer,0,
            NULL,NULL,0,
            (GChildWatchFunc)exit_cb,inputBuffer,
            NULL,
            &E)) {

        g_assert(E);
        fprintf(stderr, "Unable to run definitionjump: %s\n",E->message);
        goto error;
    }
    return;

error:
    g_clear_error(&E);
    g_free(word);
    g_free(package);
    g_ptr_array_free(inputBuffer,TRUE);
}
