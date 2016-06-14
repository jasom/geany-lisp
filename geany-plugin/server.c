#include "local.h"

void glispStartServer(void)
{
    GError *E = NULL;
    gchar **env = NULL;

    gchar *argv[2] = {GLISP_TOOLS_BASE "/launch-lisp" , NULL};


    env = glispGetUtilityEnv();

    if (! spawn_sync(NULL,NULL, argv,env,NULL,NULL,NULL,NULL,&E))
    {

        g_assert(E);
        fprintf(stderr, "Unable to start lisp server: %s\n",E->message);
        goto cleanup;
    }

cleanup:
    g_clear_error(&E);
    g_strfreev(env);
}

void glispStopServer(void)
{
    GError *E = NULL;
    gchar **env = NULL;

    gchar *argv[2] = {GLISP_TOOLS_BASE "/stop-lisp" , NULL};


    env = glispGetUtilityEnv();

    if (! spawn_sync(NULL,NULL, argv,env,NULL,NULL,NULL,NULL,&E))
    {

        g_assert(E);
        fprintf(stderr, "Unable to start lisp server: %s\n",E->message);
        goto cleanup;
    }

cleanup:
    g_clear_error(&E);
    g_strfreev(env);
}
