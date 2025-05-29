#include <gtk/gtk.h>
#include <string.h>
#include <pthread.h>

#include "main.h"
#include "gui.h"
#include "fonction.h"
#include "stack.h"

int main (int argc, char *argv[])
{
    int status;
    st_widgets *st =  NULL;
    gboolean cmd = FALSE;

    if(!(st = g_malloc(sizeof(st_widgets))))
    {
        return -1;
    }

    if(argc > 1)
    {
        if(argc > 2)
        {
            if(strcmp(argv[1], "-scan") == 0)
            {
                st->scanPath = g_strdup(argv[2]);
                cmd = TRUE;
            }
        }
        argc = 1;
    }
    
    st->thread_Pulse_ID     = 0;
    st->thread_Progress_ID  = 0;
    st->thread_Scan_ID      = 0;
    st->cmd_satus           = 0;
    st->scanItem            = new_stack();
    st->application         = gtk_application_new(CLAMGTK_APP, G_APPLICATION_DEFAULT_FLAGS); /*G_APPLICATION_FLAGS_NONE*/ /*G_APPLICATION_DEFAULT_FLAGS*/
    
    g_application_register(G_APPLICATION(st->application), NULL, NULL);

    if(g_application_get_is_remote(G_APPLICATION(st->application)))
    {
        st->scanItem = clear_stack(st->scanItem);
        if(st)
        {
            g_free(st);
        }
        int ret = system(MSG_RUN);
        return ret;
    }
    
    if(cmd == TRUE)
    {
        g_signal_connect(st->application, "activate", G_CALLBACK(activateCMD), st);
    }
    else
    {
        g_signal_connect(st->application, "activate", G_CALLBACK(activate), st);
    }
    
    status = g_application_run(G_APPLICATION(st->application), argc, argv);
    
    g_object_unref(st->application);

    if(st)
    {
        g_free(st);
    }
    
    return status;
}