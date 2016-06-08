#include "local.h"

static gint get_indent_for_line(ScintillaObject *sci, gint start, gint end);
static gint get_whitespace_start(gchar *line);

gint glisp_get_paren_indent(ScintillaObject *sci, gint line)
{
    return get_indent_for_line(sci, 0, line+1);
}

static gint get_indent_for_line(ScintillaObject *sci, gint start, gint end)
{
    int pipefd;
    int pid;
    gint i;
    size_t n = 0;

    FILE *outfile=NULL;
    gchar *line_buf = NULL;

    if(glisp_setup_pipe(GLISP_TOOLS_BASE "/lispindent", &pipefd, &outfile, &pid)) {
        fprintf(stderr,"Unable to run lispindent\n");
        return 0;
    }


    for(i=start;i<end;++i)
    {
        line_buf = sci_get_line(sci, i);
        if(line_buf == NULL) {
            goto error;
        }
        write(pipefd, line_buf, strlen(line_buf));
        g_free(line_buf);
        line_buf = NULL;
    }
    write(pipefd,"X\n", 2);

    close(pipefd);
    //TODO check for success
    waitpid(pid,NULL,0);

    fflush(outfile);
    fseek(outfile,0,SEEK_SET);

    for(i=start;i<end-1;++i) {
        getline(&line_buf, &n, outfile);
        fprintf(stderr,"LINE: %s\n",line_buf);
        free(line_buf);
        line_buf = NULL;
        n=0;
    }

    {
        gint prev_indent;
        gint new_indent;

        getline(&line_buf, &n, outfile);
        fprintf(stderr,"LINE: %s\n",line_buf);
        prev_indent = get_whitespace_start(line_buf);

        free(line_buf);
        line_buf = NULL;
        n=0;

        getline(&line_buf, &n, outfile);
        new_indent = get_whitespace_start(line_buf);

        free(line_buf);
        line_buf = NULL;
        n=0;

        return scintilla_send_message(sci, SCI_GETLINEINDENTATION,end-1,0) + (new_indent - prev_indent);
    }

error:
    if(line_buf != NULL) {
        g_free(line_buf);
    }
    close(pipefd);
    fclose(outfile);
    return 0;
}

static gint get_whitespace_start(gchar *line)
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
