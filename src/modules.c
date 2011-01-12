#include "rumble.h"
#include <dlfcn.h>

#ifndef RTLD_NODELETE
#define RTLD_NODELETE 0x80
#endif

void rumble_modules_load(masterHandle* master) {
    configElement* el;
    for ( el = cvector_first(master->readOnly.conf); el != NULL; el = cvector_next(master->readOnly.conf)) {
        if ( !strcmp(el->key, "loadmodule")) {
            printf("<modules> Loading %s...\n", el->value);
            void* handle = dlopen(el->value, RTLD_LAZY | RTLD_NODELETE);
            if (!handle) {
                fprintf (stderr, "\n<modules> Error loading %s: %s\n", el->value, dlerror());
                exit(1);
            }
            char* d = dlerror();    /* Clear any existing error */
            if ( d ) { printf("<modules> Warning: %s\n", d); }
            rumble_module_info* modinfo = calloc(1,sizeof(rumble_module_info));
            int (*init)(void* master, rumble_module_info* modinfo);
            uint32_t (*mcheck) ();
            char* error;
            init = dlsym(handle, "rumble_module_init");
            mcheck = dlsym(handle, "rumble_module_check");
            if ((error = dlerror()) != NULL)  {
                fprintf (stderr, "<modules> Warning: %s does not contain required module functions.\n", el->value);
            }
            if ( init && mcheck ) { 
                master->readOnly.currentSO = el->value;
                cvector_add(master->readOnly.modules, modinfo);
                uint32_t ver = (*mcheck)();
                if ( ver != RUMBLE_VERSION ) fprintf(stderr, "<modules> Warning: %s was compiled using an older version of librumble!\n<modules> Please recompile the module using the latest sources to avoid crashes or bugs.\n", el->value);
                int x = (*init)(master, modinfo);
                if ( x == EXIT_SUCCESS ) { printf("<modules> %s loaded OK.\n", el->value); }
                else { fprintf(stderr, "<modules> Error: %s failed to load!\n", el->value); dlclose(handle); exit(EXIT_FAILURE); }
            }
            //dlclose(handle);
        }
    }
}