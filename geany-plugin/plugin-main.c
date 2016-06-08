#include "local.h"

GeanyPlugin     *geany_plugin;
GeanyData       *geany_data;
GeanyFunctions  *geany_functions;

PLUGIN_VERSION_CHECK(211)
PLUGIN_SET_INFO("Lisp Indenting", "Lisp Indentation",
                "1.0", "John Doe <john.doe@example.org>");
PLUGIN_KEY_GROUP(lisp, 1);
//static GtkWidget *main_menu_item = NULL;

static gboolean notify_cb(GObject *obj, GeanyEditor *ed, SCNotification *nt, gpointer user_data);

PluginCallback plugin_callbacks[] =
{
    {"editor_notify", (GCallback)&notify_cb, TRUE,NULL},
    {NULL,NULL,FALSE,NULL}
};

/* Utility function for setting up input and output of external program *
 * Should probably replace with somethng that uses the callbacks */
gint glisp_setup_pipe(const char *program, int *pipefd, FILE **outf, int *outpid) 
{
    int pipefds[2] = {0};
    int pid;

    FILE *outfile = tmpfile();

    if(outfile == NULL) {
        goto error0;
    }

    if(pipe(pipefds)) {
        goto error1;
    }

    pid = fork();

    if ( pid < 0) {
        goto error2;
    }

    if( pid == 0 ) {
        dup2(pipefds[0], 0);
        close(pipefds[1]);
        dup2(fileno(outfile),1);
        execl(program,"",NULL);
        fprintf(stderr,"%s\n",program);
        perror("Error launching external program");
        exit(1);
    }

    close(pipefds[0]);
    *outf = outfile;
    *pipefd=pipefds[1];
    *outpid=pid;

    return 0;

error2:
    close(pipefds[0]);
    close(pipefds[1]);

error1:
    fclose(outfile);
error0:
    return -1;
}

static void charadded_cb(GeanyEditor *ed, SCNotification *nt)
{
    //dialogs_show_msgbox(GTK_MESSAGE_INFO, "Char: %d",nt->ch);
    
    long charAddedAt;
    switch(nt->ch) {
        case '\n':
            charAddedAt=scintilla_send_message(ed->sci,SCI_GETCURRENTPOS,0,0);
            {
                long level;
                int line;
                int position;

                position=charAddedAt;
                line=scintilla_send_message(ed->sci,SCI_LINEFROMPOSITION,
                        position,0);
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


static gboolean notify_cb(G_GNUC_UNUSED GObject *obj, GeanyEditor *ed, SCNotification *nt, G_GNUC_UNUSED gpointer user_data)
{
    if(!ed->document ||
            ed->document->file_type->id != GEANY_FILETYPES_LISP) {
        return FALSE;
    }
    //fprintf(stderr,"Code: %d\n",nt->nmhdr.code);
    switch(nt->nmhdr.code) {
        case SCN_CHARADDED:
            charadded_cb(ed,nt);
            break;
    }
    return FALSE;
}

void plugin_init(G_GNUC_UNUSED GeanyData *data)
{
    keybindings_set_item(plugin_key_group, 0, glisp_kb_run_lisp_complete,
            0, 0, "run_lisp_complete", "Complete lisp symbol", NULL);
}
void plugin_cleanup(void)
{
    //gtk_widget_destroy(main_menu_item);
}
