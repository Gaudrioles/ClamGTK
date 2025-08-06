#include <string.h>

#include "common.h"
#include "ui/gui.h"
#include "utils/fonction.h"
#include "stack/stack.h"
#include "../resource.h"

int main (int argc, char *argv[])
{
    int status = 0;
    Application *app = NULL;
    gboolean cmd = FALSE;
    
    if(!(app = g_malloc(sizeof(Application))))
    {
        return -1;
    }

    if(argc > 1)
    {
        
        if(argc > 2)
        {
            if(strcmp(argv[1], "-scan") == 0)
            {
                app->scanPath = g_strdup(argv[2]);
                cmd = TRUE;
            }
        }
        else if(strcmp(argv[1], "-version") == 0)
        {
            fprintf(stdout, "%s version %s\n", APP_NAME, APP_VERSION);
            if(app)
            {
                g_free(app);
            }
            return status;
        }

        argc = 1;
    }
    
    app->thread_Pulse_ID     = 0;
    app->thread_Progress_ID  = 0;
    app->thread_Scan_ID      = 0;
    app->cmd_satus           = 0;
    app->scanItem            = new_stack();
    app->application         = gtk_application_new(CLAMGTK_APP, G_APPLICATION_DEFAULT_FLAGS); /*G_APPLICATION_FLAGS_NONE*/ /*G_APPLICATION_DEFAULT_FLAGS*/
    
    g_application_register(G_APPLICATION(app->application), NULL, NULL);

    if(g_application_get_is_remote(G_APPLICATION(app->application)))
    {
        app->scanItem = clear_stack(app->scanItem);
        if(app)
        {
            g_free(app);
        }
        int ret = system(MSG_RUN);
        return ret;
    }
    
    if(cmd == TRUE)
    {
        g_signal_connect(app->application, "activate", G_CALLBACK(activateCMD), app);
    }
    else
    {
        g_signal_connect(app->application, "activate", G_CALLBACK(activate), app);
    }
    
    status = g_application_run(G_APPLICATION(app->application), argc, argv);
    
    g_object_unref(app->application);

    if(app)
    {
        g_free(app);
    }
    
    return status;
}