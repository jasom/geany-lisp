#include "local.h"

static struct ProjectInfo {
    gchar *lispInit;
    gchar *asdfFile;
    gchar *systemName;
    gboolean newProject;
} *ProjectInfo = NULL;

static void enableGlispCb(GtkMenuItem *item, gpointer user_data);
static void glispAddProjectMenuItems(void);
static GtkWidget *enableGlispMenuItem;
static void openLispFiles(const gchar *path);

const gchar * glispProjectGetLispInit(void)
{
    if(ProjectInfo == NULL) return NULL;
    return ProjectInfo->lispInit;
}

static void installDefaultMake()
{
    gchar *buildCmd = g_strdup_printf(GLISP_UTILITY " lisp-build-project %%p/%s %s", ProjectInfo->asdfFile, ProjectInfo->systemName);
    g_assert(buildCmd != NULL);
    build_set_menu_item(GEANY_BCS_PROJ, GEANY_GBG_NON_FT, 0, GEANY_BC_LABEL, "_Make");
    build_set_menu_item(GEANY_BCS_PROJ, GEANY_GBG_NON_FT, 0, GEANY_BC_COMMAND, buildCmd);
    build_set_menu_item(GEANY_BCS_PROJ, GEANY_GBG_NON_FT, 0, GEANY_BC_WORKING_DIR, "%p");

    g_free(buildCmd);
}


static void newProjectInfo(gchar *lispInit)
{
    ProjectInfo = g_malloc0(sizeof(struct ProjectInfo));
    ProjectInfo->lispInit = g_strdup(lispInit);
    ProjectInfo->newProject = TRUE;
    gchar *asdfname = g_strconcat(geany_data->app->project->name,".asd",NULL);

    g_assert(asdfname != NULL);

    ProjectInfo->asdfFile = g_build_filename("lisp",asdfname,NULL);
    ProjectInfo->systemName = g_strdup(geany_data->app->project->name);

    g_free(asdfname);
}

static void destroyProjectInfo()
{
    if(ProjectInfo != NULL) {
        g_free(ProjectInfo->lispInit);
        g_free(ProjectInfo->asdfFile);
        g_free(ProjectInfo->systemName);
        g_free(ProjectInfo);
        ProjectInfo = NULL;
    }
}


void glispProjectOpen(GKeyFile *keyFile)
{
    gchar *tmp;
    GString *id = g_string_new("");

    fprintf(stderr,"glispProjectOpen\n");
    g_assert(ProjectInfo == NULL);

    tmp = g_key_file_get_string(keyFile, "glisp", "lisp_init", NULL);

    if(tmp == NULL) {
        gtk_widget_set_sensitive(enableGlispMenuItem, TRUE);
        return;
    }

    gtk_widget_set_sensitive(enableGlispMenuItem, FALSE);

    ProjectInfo = g_malloc0(sizeof(struct ProjectInfo));

    ProjectInfo->lispInit = tmp;

    tmp = g_key_file_get_string(keyFile, "glisp", "asdf_file", NULL);
    ProjectInfo->asdfFile = tmp;

    tmp = g_key_file_get_string(keyFile, "glisp", "system_name", NULL);
    ProjectInfo->systemName = tmp;

    tmp = g_key_file_get_string(keyFile, "build-menu", "NF_00_LB",NULL);
    if(tmp == NULL) {
        installDefaultMake();
    }
    g_free(tmp);

    ProjectInfo->newProject = FALSE;


    g_setenv("GLISP_INIT",ProjectInfo->lispInit, TRUE);
    g_string_printf(id,"glisp-%d",getpid());
    g_setenv("GLISP_EMACSID",id->str,FALSE);
    g_setenv("GLISP_UTILITY_PATH",GLISP_TOOLS_BASE,FALSE);

    glispStringDestroy(id);
}

void glispProjectClose(void)
{
    destroyProjectInfo();
}

void glispProjectSave(GKeyFile *config)
{
    fprintf(stderr,"glispProjectSave\n");
    // New project does a save but not open
    if(ProjectInfo == NULL) {
        glispProjectOpen(config);
        return;
    }

    g_key_file_set_string(config, "glisp", "lisp_init", ProjectInfo->lispInit);
    g_key_file_set_string(config, "glisp", "asdf_file", ProjectInfo->asdfFile);
    g_key_file_set_string(config, "glisp", "system_name", ProjectInfo->systemName);

    /* If it's a new project, force an open */
    if(ProjectInfo->newProject) {
        destroyProjectInfo();
        glispProjectOpen(config);
    }
}

void glispProjectInit(void)
{
    glispAddProjectMenuItems();
    gtk_widget_set_sensitive(enableGlispMenuItem, FALSE);
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
    GError *E = NULL;
    gchar *argv[5] = {GLISP_UTILITY, "quickproject", NULL, NULL, NULL};
    
    g_setenv("GLISP_UTILITY_PATH",GLISP_TOOLS_BASE,FALSE);

    if(geany_data->app->project == NULL) {
        goto cleanup;
    }

    /*
    GString *id = g_string_new("");
    g_string_printf(id,"glisp-%d",getpid());
    g_setenv("GLISP_EMACSID",id->str,FALSE);
    glispStringDestroy(id);
    */

    argv[2] = projectPath;
    argv[3] = geany_data->app->project->name;

    if (! spawn_sync(NULL,NULL, argv,NULL,NULL,NULL,NULL,NULL,&E))
    {

        g_assert(E);
        fprintf(stderr, "Unable to run quickproject: %s\n",E->message);
        goto cleanup;
    }

    //gtk_widget_set_sensitive(enableGlispMenuItem, FALSE);
    newProjectInfo(GLISP_DEFAULT_LISP_INIT);
    project_write_config();
    glispServerStart();

    openLispFiles(projectPath);

cleanup:
    g_free(projectPath);
    g_clear_error(&E);
}

static void openLispFilesInternal(const gchar *path)
{
    static GPatternSpec* lispPattern=NULL;
    static GPatternSpec* asdfPattern=NULL;
    GDir *directory=NULL;
    GError *E = NULL;

    if(lispPattern == NULL) {
        lispPattern = g_pattern_spec_new("*.lisp");
        asdfPattern = g_pattern_spec_new("*.asd");
    }

    directory=g_dir_open(path, 0, &E);
    if(directory == NULL) {
        g_assert(E);
        fprintf(stderr, "Unable to open directory: %s\n", E->message);
        goto cleanup;
    }

    for(const gchar *name = g_dir_read_name(directory);
            name != NULL;
            name = g_dir_read_name(directory)) {
        gchar * filename = g_build_filename(path,name,NULL);
        if(g_file_test(filename, G_FILE_TEST_IS_REGULAR)) {
            if(g_pattern_match_string(lispPattern, name) ||
                    g_pattern_match_string(asdfPattern, name)) {
                document_open_file(filename, FALSE, NULL, NULL);
            }
        }else if(g_file_test(filename, G_FILE_TEST_IS_SYMLINK)) {
            /* g_file_test follows symlinks, so if we reach here
             * it's a symlink to something other than a regular file
             * We don't want to follow directory symlinks without
             * circularity tests, so just skip for now */
        } else if(g_file_test(filename, G_FILE_TEST_IS_DIR)) {
            openLispFiles(name);
        }
        g_free(filename);
    }

cleanup:
    if(directory != NULL) g_dir_close(directory);
}

static void openLispFiles(const gchar *projectDirectory)
{
    gchar *cwd = g_get_current_dir();
    g_chdir(projectDirectory);
    openLispFilesInternal("lisp");
    g_free(cwd);
}
