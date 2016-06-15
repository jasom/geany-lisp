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

/* Taken from projectorganizer plugin */
gchar *glispGetProjectBasePath(void)
{
        GeanyProject *project = geany_data->app->project;

        if (project && !EMPTY(project->base_path))
        {
                if (g_path_is_absolute(project->base_path))
                        return g_strdup(project->base_path);
                else
                {       /* build base_path out of project file name's dir and base_path */
                        gchar *path;
                        gchar *dir = g_path_get_dirname(project->file_name);

                        if (utils_str_equal(project->base_path, "./"))
                                return dir;

                        path = g_build_filename(dir, project->base_path, NULL);
                        g_free(dir);
                        return path;
                }
        }
        return NULL;
}
