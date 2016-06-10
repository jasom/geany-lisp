#include "local.h"

GeanyPlugin     *geany_plugin;
GeanyData       *geany_data;
GeanyFunctions  *geany_functions;

PLUGIN_VERSION_CHECK(211)
PLUGIN_SET_INFO("Lisp Development", "Lisp Development",
                "1.0", "John Doe <john.doe@example.org>");
PLUGIN_KEY_GROUP(lisp, 2);
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

    /* change to g_file_open_tmp */
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
        execl(program,program,NULL);
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
    gint position=sci_get_current_position(ed->sci);
    glisp_indent_charadded_cb(ed,nt,position);
    glisp_completions_charadded_cb(ed,nt,position);
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
    keybindings_set_item(plugin_key_group, 1, glisp_kb_run_lisp_jump,
            0, 0, "run_lisp_jump", "Jump to Lisp definition", NULL);
}
void plugin_cleanup(void)
{
    //gtk_widget_destroy(main_menu_item);
}
