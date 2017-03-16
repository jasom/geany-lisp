#include "local.h"

GeanyPlugin     *geany_plugin;
GeanyData       *geany_data;
//GeanyFunctions  *geany_functions;

//PLUGIN_VERSION_CHECK(211)
//PLUGIN_KEY_GROUP(lisp, 2);
//static GtkWidget *main_menu_item = NULL;

static gboolean notify_cb(GObject *obj, GeanyEditor *ed, SCNotification *nt, gpointer user_data);
static void project_open_cb(G_GNUC_UNUSED GObject * obj, G_GNUC_UNUSED GKeyFile * config, G_GNUC_UNUSED gpointer user_data);
static void project_close_cb(G_GNUC_UNUSED GObject * obj, G_GNUC_UNUSED GKeyFile * config, G_GNUC_UNUSED gpointer user_data);
static void project_save_cb(G_GNUC_UNUSED GObject * obj, GKeyFile * config, G_GNUC_UNUSED gpointer user_data);
static gboolean myplugin_init(GeanyPlugin *plugin, gpointer pdata);
static void myplugin_cleanup(GeanyPlugin *plugin, gpointer pdata);

static PluginCallback plugin_callbacks[] =
{
    {"editor-notify", (GCallback)&notify_cb, TRUE,NULL},
    {"project-open", (GCallback)&project_open_cb, TRUE,NULL},
    {"project-close", (GCallback)&project_close_cb, TRUE,NULL},
    {"project-save", (GCallback)&project_save_cb, TRUE,NULL},
    {NULL,NULL,FALSE,NULL}
};


void geany_load_module(GeanyPlugin *plugin)
{
    plugin->info->name = "Lisp Development";
    plugin->info->description = "Lisp Development";
    plugin->info->version = "0.2";
    plugin->info->author = "Jason Miller <jasom@fastmail.com>";
    plugin->funcs->callbacks = plugin_callbacks;
    plugin->funcs->init = myplugin_init;
    plugin->funcs->cleanup = myplugin_cleanup;
    plugin->funcs->configure = NULL;
    plugin->funcs->help = NULL;
    GEANY_PLUGIN_REGISTER(plugin,225);
}

static void charadded_cb(GeanyEditor *ed, SCNotification *nt)
{
    gint position=sci_get_current_position(ed->sci);
    glispIndentCharaddedCb(ed,nt,position);
    glispCompletionsCharaddedCb(ed,nt,position);
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


static void project_open_cb(G_GNUC_UNUSED GObject * obj,
        GKeyFile * config,
        G_GNUC_UNUSED gpointer user_data)
{
    glispProjectOpen(config);
    glispServerStart();
}

static void project_save_cb(G_GNUC_UNUSED GObject * obj,
        GKeyFile * config,
        G_GNUC_UNUSED gpointer user_data)
{
    glispProjectSave(config);
}

static void project_close_cb(G_GNUC_UNUSED GObject * obj,
        G_GNUC_UNUSED GKeyFile * config,
        G_GNUC_UNUSED gpointer user_data)
{
    glispStopServer();
    glispProjectClose();
}

static gboolean myplugin_init(GeanyPlugin *plugin, G_GNUC_UNUSED gpointer pdata)
{
    GeanyKeyGroup *kg = plugin_set_key_group(plugin, "lisp", 2, NULL);
    
    geany_data = plugin->geany_data;
    geany_plugin = plugin;

    keybindings_set_item(kg, 0, glispKbRunComplete,
            0, 0, "run_lisp_complete", "Complete lisp symbol", NULL);
    keybindings_set_item(kg, 1, glispKbRunJump,
            0, 0, "run_lisp_jump", "Jump to Lisp definition", NULL);


    glispProjectInit();

    return TRUE;
}

static void myplugin_cleanup(G_GNUC_UNUSED GeanyPlugin *plugin, G_GNUC_UNUSED gpointer pdata)
{
    return;
}
