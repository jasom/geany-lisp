#include "local.h"

gchar **glispGetUtilityEnv(void)
{
    GString *id = g_string_new("");
    gchar **env;
    g_string_printf(id,"glisp-%d",getpid());
    env = g_get_environ();
    env = g_environ_setenv(env,"GLISP_EMACSID",id->str,FALSE);

    glispStringDestroy(id);
    return env;
}


void glispStringDestroy(GString *s)
{
    if(s != NULL) g_string_free(s,TRUE);
}

void glispSlurpCb(GString *instring, GIOCondition condition, GPtrArray *accum)
{
    if (condition & (G_IO_IN | G_IO_PRI)) {
        // I think line buffered means call this once per line
        g_assert (instring->str[instring->len-1] == '\n');
        GString *line = g_string_new_len(instring->str,instring->len-1); //Strip newline
        g_ptr_array_add(accum,line);
    }
}

