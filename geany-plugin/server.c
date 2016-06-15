#include "local.h"

void glispServerStart()
{
    GError *E = NULL;
    GString *slimePath = g_string_new("");
    gchar **env = NULL;
    gchar *projectPath = glispGetProjectBasePath();
    const gchar *argv[4] = {GLISP_TOOLS_BASE "/launch-lisp",NULL,NULL,NULL};
    const gchar *lispInit = glispProjectGetLispInit();

    if(!lispInit) goto cleanup;

    g_string_printf(slimePath,"%s/quicklisp/slime-helper.el",projectPath);

    argv[1] = slimePath->str;
    argv[2] = lispInit;

    env = glispGetUtilityEnv();

    if (! spawn_sync(NULL,NULL, (gchar **)argv,env,NULL,NULL,NULL,NULL,&E))
    {

        g_assert(E);
        fprintf(stderr, "Unable to start lisp server: %s\n",E->message);
        goto cleanup;
    }

cleanup:
    g_free(projectPath);
    glispStringDestroy(slimePath);
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
