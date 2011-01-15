#include "rumble.h"
extern masterHandle* master;

void rumble_clean_session(sessionHandle* session) {
    rumble_free_address(&session->sender);
    address* el;
    for ( el = (address*) cvector_first(session->recipients); el != NULL; el = (address*) cvector_next(session->recipients)) {
        rumble_free_address(el);
    }
    cvector_flush(session->recipients);
}

masterHandle* rumble_get_master() {
    return master;
}

void rumble_tag_file(FILE* fp, const char* host, const char* fid, const char* usr, const char* dmn ) {
    char* log = calloc(1,1024);
    char* now = rumble_mtime();
    if ( dmn && usr ) {
        sprintf(log, "Received: from %s by %s (rumble) for %s@%s with ESMTP id %s; %s\r\n", host, rumble_config_str("servername"), usr, dmn, fid, now);
    }
    else {
        sprintf(log, "Received: from %s by %s (rumble) with ESMTP id %s; %s\r\n", host, rumble_config_str("servername"), fid, now);
    }
    free(now);
    fwrite(log, strlen(log), 1, fp);
}

char* rumble_copy_mail(const char* fid, const char* usr, const char* dmn) {
    const char* path = rumble_config_str("storagefolder");
    char* nfid = calloc(1,25);
    sprintf(nfid, "%x%x%x", (uint32_t) pthread_self(), (uint32_t) time(0), (uint32_t) rand());
    char* filename = calloc(1, strlen(path) + 26);
    char* ofilename = calloc(1, strlen(path) + 26);
    sprintf(filename, "%s/%s", path, nfid);
    sprintf(ofilename, "%s/%s", path, fid);
    FILE* fp = fopen(filename, "w");
    FILE* ofp = fopen(ofilename, "r");
    #ifdef RUMBLE_DEBUG_STORAGE
        printf("Copying %s to file %s...\n", ofilename, filename);
    #endif
    free(filename);
    free(ofilename);
    if ( fp && ofp ) {
        char* now = rumble_mtime();
        fprintf(fp, "Received: from localhost by %s (rumble) for %s@%s with ESMTP id %s; %s\r\n", rumble_config_str("servername"), usr, dmn, nfid, now);
        free(now);
        void* buffer = calloc(1,4096);
        while (!feof(ofp)) {
            size_t rc = fread(buffer, 1, 4096, ofp);
            if ( rc < 0 ) break;
            if ( !fwrite(buffer, rc, 1, fp)) break;
        }
        fclose(fp);
        fclose(ofp);
        free(buffer);
    }
    else {
        if (fp) fclose(fp);
        if (ofp) fclose(ofp);
        free(nfid);
        return 0;
    }
    return nfid;
}