#include "local.h"

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

static void get_completions(ScintillaObject *sci, long *backtrack, gchar **partial, gchar**completions) 
{
    int pipefd;
    int pid;
    size_t n = 0;

    FILE *outfile=NULL;
    gchar *line_buf = NULL;

    gint pos = sci_get_current_position(sci);

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

void glisp_kb_run_lisp_complete(G_GNUC_UNUSED guint key_id)
{
    GeanyDocument* doc = document_get_current();
    GeanyEditor* editor;
    ScintillaObject *sci;
    long rootlen;
    gchar *partial;
    gchar *completions;

    if(!doc || !doc->editor || ! doc->editor->sci) {
        return;
    }
    editor = doc->editor;
    sci = editor->sci;

    get_completions(sci, &rootlen, &partial, &completions);

    if (completions == NULL) goto error;

    // TODO do this only once per editor
    scintilla_send_message(sci,SCI_AUTOCSETORDER,SC_ORDER_CUSTOM,0);
    scintilla_send_message(sci,SCI_AUTOCSETSEPARATOR,'\n',0);

    scintilla_send_message(sci,SCI_AUTOCSHOW,rootlen,(intptr_t)completions);

error:
    if(completions != NULL) g_free(completions);
    if(partial != NULL) g_free(partial);
}

