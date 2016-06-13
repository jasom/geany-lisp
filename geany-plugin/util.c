#include "local.h"

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

