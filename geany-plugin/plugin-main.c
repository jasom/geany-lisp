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

void plugin_init(G_GNUC_UNUSED GeanyData *data)
{
    keybindings_set_item(plugin_key_group, 0, glispKbRunComplete,
            0, 0, "run_lisp_complete", "Complete lisp symbol", NULL);
    keybindings_set_item(plugin_key_group, 1, glispKbRunJump,
            0, 0, "run_lisp_jump", "Jump to Lisp definition", NULL);
}
void plugin_cleanup(void)
{
    //gtk_widget_destroy(main_menu_item);
}

