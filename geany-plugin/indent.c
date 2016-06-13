#include "local.h"

static gint getIndentForLine(ScintillaObject *sci, gint start, gint end);
static gint getWhitespaceStart(gchar *line);

static gint glisp_get_paren_indent(ScintillaObject *sci, gint line)
{
    return getIndentForLine(sci, 0, line+1);
}

void glispIndentCharaddedCb(GeanyEditor *ed, SCNotification *nt, gint position)
{
    //dialogs_show_msgbox(GTK_MESSAGE_INFO, "Char: %d",nt->ch);
    
    switch(nt->ch) {
        case '\n':
            {
                long level;
                int line;

                line=sci_get_line_from_position(ed->sci,position);
                level = glisp_get_paren_indent(ed->sci,line-1);
                //fprintf(stderr,"Indent: %d\n",moreindent);
                fprintf(stderr,"Level: %ld\n",level);
                scintilla_send_message(ed->sci,SCI_SETLINEINDENTATION,line,level);
                scintilla_send_message(ed->sci,SCI_GOTOPOS,
                        scintilla_send_message(ed->sci,SCI_GETLINEINDENTPOSITION,line,0),0);

            }
            break;
    }
}
struct indentCbData {
    gint position;
    gint end;
    ScintillaObject *sci;
    gint indentLevel;
};

static gboolean stdinCb(GIOChannel *source, GIOCondition condition, struct indentCbData *data)
{
    gboolean returnvalue=TRUE;
    GError *E=NULL;
    char *line_buf=NULL;
    if(condition == G_IO_OUT) {
        gsize bytes_written;
        gsize len;


        if(data->position >= data->end) {
            line_buf=malloc(2);
            strcpy(line_buf,"X");
            returnvalue=FALSE;
        }
        else {
            line_buf = sci_get_line(data->sci, data->position);
        }

        len=strlen(line_buf);

        g_io_channel_write_chars(source,line_buf,len,
                &bytes_written,&E);
        g_assert(bytes_written == len);
        if(E != NULL) {
            fprintf(stderr,"Error writing to indenter: %s\n",E->message);
            returnvalue=FALSE;
            goto cleanup;
        }
        data->position++;
    } else if (condition == G_IO_ERR ||
            condition == G_IO_HUP ||
            condition == G_IO_NVAL) {
        fprintf(stderr,"Indentor closed stdin prematurely\n");
        returnvalue = FALSE;
        goto cleanup;
    }

cleanup:
    g_clear_error(&E);
    free(line_buf);
    return returnvalue;
}

static void stdoutCb(GString *instring, GIOCondition condition, struct indentCbData *data)
{
    if (condition & (G_IO_IN | G_IO_PRI)) {
        if(data->position < data->end-1) {
            //empty
        } else if(data->position == data->end-1) {
            fprintf(stderr,"LINE: %s\n",instring->str);
            data->indentLevel = getWhitespaceStart(instring->str);
        } else if(data->position == data->end) {
            fprintf(stderr,"LINE: %s\n",instring->str);
            data->indentLevel = getWhitespaceStart(instring->str) - data->indentLevel;
        }
        data->position++;
    }
}

static gint getIndentForLine(ScintillaObject *sci, gint start, gint end)
{
    GError *E=NULL;
    gint result=0;
    gchar *argv[2] = {GLISP_TOOLS_BASE "/lispindent" , NULL};
    struct indentCbData *data = g_malloc(sizeof(struct indentCbData) *2);

    data[0].position=start;
    data[0].end=end;
    data[0].sci=sci;
    data[1].position=start;
    data[1].end=end;
    data[1].sci=sci;

    if (! spawn_with_callbacks(NULL,NULL,argv,NULL,SPAWN_SYNC,
            (GIOFunc)stdinCb,&data[0],
            (SpawnReadFunc)stdoutCb,&data[1],0,
            NULL,NULL,0,
            NULL,NULL,
            NULL,
            &E)) {

        g_assert(E);
        fprintf(stderr, "Unable to run definitionjump: %s\n",E->message);
        goto error;
    }

    result =  scintilla_send_message(sci, SCI_GETLINEINDENTATION,end-1,0) + data[1].indentLevel;

error:
    g_clear_error(&E);
    g_free(data);
    return result;
}

static gint getWhitespaceStart(gchar *line)
{
    gint position = 0;
    int i;

    for (i=0;line[i];++i) {
        switch (line[i]) {
            case ' ':
                position++;
                break;
            case '\t':
                //TODO figure out actual tab-stop
                position +=8;
                break;
            default:
                return position;
        }
    }
    return position;
}
