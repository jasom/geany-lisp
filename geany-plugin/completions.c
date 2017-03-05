#include "local.h"

static void complete_at_position(ScintillaObject *sci, gint pos);

void glispCompletionsCharaddedCb(GeanyEditor *ed, SCNotification *nt, gint position)
{
    static gint first,last;

    //BUG: Need to check sytnax at point so I don't try to complete in strings
    if(strchr(" \"\t\n(),;",nt->ch)) {
        first=-2;
        last=-2;
        return;
    }

    if(position != last +1) {
        first=position;
        last=position;
        return;
    }
    else {
        last++;
    }
    if(last-first > 3) {
        complete_at_position(ed->sci,position);
    }

}

struct stdinData {
    char *p;
    gsize len;
    gsize offset;
};

static gboolean stdinCb(GIOChannel *source, GIOCondition condition, struct stdinData *data)
{
    gboolean returnvalue=TRUE;
    GError *E=NULL;
    if(condition == G_IO_OUT) {
        gsize bytes_written=0;
        if (data->offset >= data->len) {
            returnvalue=FALSE;
        } else {

            g_io_channel_write_chars(source,
                    data->p + data->offset,
                    data->len - data->offset,
                    &bytes_written,&E);
            if(E != NULL) {
                fprintf(stderr,"Error writing to indenter: %s\n",E->message);
                returnvalue=FALSE;
                goto cleanup;
            }
            data->offset+=bytes_written;
        }
    } else if (condition == G_IO_ERR ||
            condition == G_IO_HUP ||
            condition == G_IO_NVAL) {
        fprintf(stderr,"Completions closed stdin prematurely\n");
        returnvalue = FALSE;
        goto cleanup;
    }

cleanup:
    g_clear_error(&E);
    return returnvalue;
}

static void get_completions(ScintillaObject *sci, gint pos, long *backtrack, gchar **partial, gchar**completions) 
{
    struct stdinData stdinData = {NULL,0,0};
    GPtrArray *inputBuffer = g_ptr_array_new_with_free_func((GDestroyNotify)glispStringDestroy);
    GError *E=NULL;
    GString *tmp=NULL;
    gchar *argv[3] = {GLISP_UTILITY,"lisp-complete", NULL};
    gsize i;
    GString *output=NULL;

    *completions=NULL;
    *backtrack=0;
    *partial=NULL;

    stdinData.p = sci_get_contents_range(sci,0,pos);

    if(stdinData.p == NULL) {
        goto cleanup;
    }

    stdinData.len=strlen(stdinData.p);


    if (! spawn_with_callbacks(NULL,NULL,argv,NULL,SPAWN_SYNC,
            (GIOFunc)stdinCb,&stdinData,
            (SpawnReadFunc)glispSlurpCb,inputBuffer,0,
            NULL,NULL,0,
            NULL,NULL,
            NULL,
            &E)) {

        g_assert(E);
        fprintf(stderr, "Unable to get completions: %s\n",E->message);
        goto cleanup;
    }

    if(inputBuffer->len <  3) {
        goto cleanup;
    }

    // No errors after this point
    
    tmp=g_ptr_array_index(inputBuffer,0);
    *backtrack = g_ascii_strtoull(tmp->str,NULL,10);

    tmp=g_ptr_array_index(inputBuffer,1);
    *partial=g_string_free(tmp,FALSE);
    //Take ownership of string from array
    g_ptr_array_index(inputBuffer,1)=NULL;

    output = g_string_sized_new(1024);
    for(i=2;i<inputBuffer->len;++i) {
        tmp=g_ptr_array_index(inputBuffer,i);
        g_string_append(output,tmp->str);
        g_string_append_c(output,'\n');
    }
    *completions = g_string_free(output,FALSE);

cleanup:
    g_clear_error(&E);
    g_ptr_array_free(inputBuffer,TRUE);
    return;
}


static void complete_at_position(ScintillaObject *sci, gint pos)
{
    long rootlen;
    gchar *partial;
    gchar *completions;
    gint lexer,style;

    if(pos<2) {
        return;
    }

    lexer = sci_get_lexer(sci);
    style = sci_get_style_at(sci,pos-2);

    if(!highlighting_is_code_style(lexer,style)) {
        return;
    }

    get_completions(sci, pos, &rootlen, &partial, &completions);

    if (completions == NULL) goto error;

    // TODO do this only once per editor
    scintilla_send_message(sci,SCI_AUTOCSETORDER,SC_ORDER_CUSTOM,0);
    scintilla_send_message(sci,SCI_AUTOCSETSEPARATOR,'\n',0);

    scintilla_send_message(sci,SCI_AUTOCSHOW,rootlen,(intptr_t)completions);

error:
    if(completions != NULL) g_free(completions);
    if(partial != NULL) g_free(partial);
}


void glispKbRunComplete(G_GNUC_UNUSED guint key_id)
{
    GeanyDocument* doc = document_get_current();
    GeanyEditor* editor;
    ScintillaObject *sci;

    if(!doc || !doc->editor || ! doc->editor->sci) {
        return;
    }
    editor = doc->editor;
    sci = editor->sci;

    gint position=sci_get_current_position(sci);
    complete_at_position(sci,position);
}
