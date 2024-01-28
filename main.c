#include <gtk/gtk.h>
#include <string.h>
#include <pthread.h>

#include "main.h"
#include "gui.h"
#include "fonction.h"

int main (int argc, char* argv[])
{
    int status;
    st_widgets st;
    gboolean cmd = FALSE;

    if(argc > 1)
    {
        if(strcmp(argv[1], "-scan") == 0)
        {
            st.scanPath = g_strdup(argv[2]);
            argc = 1;
            cmd = TRUE;
        }
    }
    
    st.application = gtk_application_new("ClamGTK.app", G_APPLICATION_FLAGS_NONE); /*G_APPLICATION_FLAGS_NONE*/ /*G_APPLICATION_DEFAULT_FLAGS*/
    
    if(cmd == TRUE)
    {
        g_signal_connect(st.application, "activate", G_CALLBACK(activateCMD), &st);
    }
    else
    {
        g_signal_connect(st.application, "activate", G_CALLBACK(activate), &st);
    }
    
    status = g_application_run(G_APPLICATION(st.application), argc, argv);
    
    g_object_unref(st.application);
    
    return status;
}