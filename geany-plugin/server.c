#include "local.h"

void glispServerStart()
{
    GError *E = NULL;
    gchar *projectPath = glispGetProjectBasePath();
    const gchar *argv[4] = {GLISP_TOOLS_BASE "/launch-lisp",NULL,NULL,NULL};
    const gchar *lispInit = glispProjectGetLispInit();

    if(!lispInit) goto cleanup;

    argv[1] = projectPath;
    argv[2] = lispInit;

    if (! spawn_sync(NULL,NULL, (gchar **)argv,NULL,NULL,NULL,NULL,NULL,&E))
    {

        g_assert(E);
        fprintf(stderr, "Unable to start lisp server: %s\n",E->message);
        goto cleanup;
    }

cleanup:
    g_free(projectPath);
    g_clear_error(&E);
}

void glispStopServer(void)
{
    GError *E = NULL;

    gchar *argv[2] = {GLISP_TOOLS_BASE "/stop-lisp" , NULL};



    if (! spawn_sync(NULL,NULL, argv,NULL,NULL,NULL,NULL,NULL,&E))
    {

        g_assert(E);
        fprintf(stderr, "Unable to start lisp server: %s\n",E->message);
        goto cleanup;
    }

cleanup:
    g_clear_error(&E);
}
