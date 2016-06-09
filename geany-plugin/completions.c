#include "local.h"

static void complete_at_position(ScintillaObject *sci, gint pos);

void glisp_completions_charadded_cb(GeanyEditor *ed, SCNotification *nt, gint position)
{
    static gint first,last;

    if(strchr(" \"\t\n()#;",nt->ch)) {
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


static gchar *read_to_end_of_file(FILE *f)
{
    long start = ftell(f);
    long end;
    gchar *buf=NULL;

    fseek(f, 0L, SEEK_END);

    end = ftell(f);

    fseek(f, start, SEEK_SET);

    buf = g_malloc0(1+end-start);

    if(buf != NULL) {
        size_t bytes_read = fread(buf,1,end-start,f);
        if(bytes_read != (unsigned long)end-start) {
            g_free(buf);
            return NULL;
        }
    }

    return buf;
}

static void get_completions(ScintillaObject *sci, gint pos, long *backtrack, gchar **partial, gchar**completions) 
{
    int pipefd;
    int pid;
    size_t n = 0;

    FILE *outfile=NULL;
    gchar *line_buf = NULL;


    *completions=NULL;
    *backtrack=0;
    *partial=NULL;

    if(glisp_setup_pipe(GLISP_TOOLS_BASE "/lispcomplete", &pipefd, &outfile, &pid)) {
        fprintf(stderr,"Unable to run lispcomplete\n");
        return;
    }

    //Todo if pos is large, chunk this
    line_buf = sci_get_contents_range(sci,0,pos);

    if(line_buf == NULL) {
        goto error;
    }

    write(pipefd,line_buf,strlen(line_buf));

    g_free(line_buf);
    line_buf = NULL;

    close(pipefd);
    //TODO check for success
    waitpid(pid,NULL,0);

    fflush(outfile);
    fseek(outfile,0,SEEK_SET);

    getline(&line_buf, &n, outfile);
    if(line_buf == NULL) {
        goto error;
    }
    
    *backtrack = strtol(line_buf,NULL,10);
    g_free(line_buf);
    line_buf=NULL;

    getline(&line_buf, &n, outfile);
    if(line_buf == NULL) {
        goto error;
    }

    *partial=line_buf;
    line_buf=NULL;

    *completions = read_to_end_of_file(outfile);

error:
    if(line_buf != NULL) {
        g_free(line_buf);
    }
    close(pipefd);
    fclose(outfile);
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


void glisp_kb_run_lisp_complete(G_GNUC_UNUSED guint key_id)
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
