#include "local.h"

static struct ProjectInfo {
    gchar *lispInit;
    //gchar *asdfFile;
    //gchar *systemName;
} *ProjectInfo = NULL;

static void enableGlispCb(GtkMenuItem *item, gpointer user_data);
static void glispAddProjectMenuItems(void);

const gchar * glispProjectGetLispInit(void)
{
    if(ProjectInfo == NULL) return NULL;
    return ProjectInfo->lispInit;
}

static void newProjectInfo(gchar *lispInit)
{
    ProjectInfo = g_malloc0(sizeof(struct ProjectInfo));
    ProjectInfo->lispInit = g_strdup(lispInit);
}

static void destroyProjectInfo()
{
    if(ProjectInfo != NULL) {
        g_free(ProjectInfo->lispInit);
        g_free(ProjectInfo);
        ProjectInfo = NULL;
    }
}

static GtkWidget *enableGlispMenuItem;

void glispProjectOpen(GKeyFile *keyFile)
{
    gchar *tmp;

    g_assert(ProjectInfo == NULL);

    tmp = g_key_file_get_string(keyFile, "glisp", "lisp_init", NULL);

    if(tmp == NULL) {
        gtk_widget_set_sensitive(enableGlispMenuItem, TRUE);
        return;
    }

    gtk_widget_set_sensitive(enableGlispMenuItem, FALSE);

    ProjectInfo = g_malloc0(sizeof(struct ProjectInfo));

    ProjectInfo->lispInit = tmp;
    //ProjectInfo->asdfFile = g_key_file_get_value(keyFile, "glisp", "asdf_file", NULL);
    //ProjectInfo->systemName = g_key_file_get_value(keyFile, "glisp", "system_name", NULL);
}

void glispProjectClose(void)
{

    destroyProjectInfo();
}

void glispProjectSave(GKeyFile *config)
{
    if(ProjectInfo == NULL) {
        return;
    }

    g_key_file_set_string(config, "glisp", "lisp_init", ProjectInfo->lispInit);
}

void glispProjectInit(void)
{
    glispAddProjectMenuItems();
}

static void glispAddProjectMenuItems(void)
{
    GtkWidget * menuItem = gtk_menu_item_new_with_label("Enable GLisp on project");
    gtk_container_add(GTK_CONTAINER(geany->main_widgets->project_menu), menuItem);
    g_signal_connect((gpointer) menuItem, "activate", G_CALLBACK(enableGlispCb), NULL);
    gtk_widget_show(menuItem);

    enableGlispMenuItem = menuItem;
}


static void enableGlispCb(G_GNUC_UNUSED GtkMenuItem *item, G_GNUC_UNUSED gpointer user_data)
{
    gchar *projectPath = glispGetProjectBasePath();
    gchar **env = NULL;
    GError *E = NULL;
    gchar *argv[4] = {GLISP_TOOLS_BASE "/quickproject",NULL,NULL,NULL};
    

    if(geany_data->app->project == NULL) {
        goto cleanup;
    }

    argv[1] = projectPath;
    argv[2] = geany_data->app->project->name;

    env = glispGetUtilityEnv();

    if (! spawn_sync(NULL,NULL, argv,env,NULL,NULL,NULL,NULL,&E))
    {

        g_assert(E);
        fprintf(stderr, "Unable to start lisp server: %s\n",E->message);
        goto cleanup;
    }

    newProjectInfo(GLISP_DEFAULT_LISP_INIT);
    project_write_config();
cleanup:
    g_free(projectPath);
    g_strfreev(env);
    g_clear_error(&E);
}