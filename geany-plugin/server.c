#include "local.h"

void glispServerStart()
{
    GError *E = NULL;
    gchar *projectPath = glispGetProjectBasePath();
    const gchar *argv[5] = {GLISP_UTILITY, "launch-lisp",NULL,NULL,NULL};
    const gchar *lispInit = glispProjectGetLispInit();

    if(!lispInit) goto cleanup;

    argv[2] = projectPath;
    argv[3] = lispInit;

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

    gchar *argv[3] = {GLISP_UTILITY, "stop-lisp" , NULL};



    if (! spawn_sync(NULL,NULL, argv,NULL,NULL,NULL,NULL,NULL,&E))
    {

        g_assert(E);
        fprintf(stderr, "Unable to start lisp server: %s\n",E->message);
        goto cleanup;
    }

cleanup:
    g_clear_error(&E);
}
