#include "rumble.h"
#include <openssl/sha.h>
#ifndef PRIx32
#define PRIx32 "x"
#endif

/* char* rumble_sha256(const unsigned char* d)
 * Converts the string (d) into a SHA-256 digest (64 byte hex string).
 * Note: For extra speed, digests are printed out "backwards" as:
 * DDCCBBAA DDCCBBAA DDCCBBAA DDCCBBAA DDCCBBAA DDCCBBAA DDCCBBAA DDCCBBAA
 * This should have no effect on security and is roughly 3 times faster.
*/
char* rumble_sha256(const unsigned char* d) {
	unsigned int* x;
    unsigned char* md = (unsigned char*) malloc(33);
    char* ret = (char*) malloc(65);
	if (!ret) merror();
    SHA256(d, strlen((const char*) d), md);
    x = (unsigned int*) md;
    sprintf((char*) ret, "%08"PRIx32"%08"PRIx32"%08"PRIx32"%08"PRIx32"%08"PRIx32"%08"PRIx32"%08"PRIx32"%08"PRIx32, x[0],x[1],x[2],x[3],x[4],x[5],x[6],x[7]);
    memset(md, 0, 33); // Erase md, just in case.
    free(md);
    return ret;
}

/* char* rumble_sha160(const unsigned char* d)
 * Converts the string (d) into a hex SHA1 160 bit digest (40 byte hex string).
 * This is used for simpler tasks, such as grey-listing, where collisions are
 * of less importance.
 * Note: For extra speed, digests are printed out "backwards" as:
 * DDCCBBAA DDCCBBAA DDCCBBAA DDCCBBAA DDCCBBAA
 * This should have no effect on security and is roughly 3 times faster.
*/
char* rumble_sha160(const unsigned char* d) {
	unsigned int* x;
    unsigned char* md = (unsigned char*) malloc(21);
    char* ret = (char*) malloc(41);
	if (!ret) merror();
    SHA1(d, strlen((const char*) d), md);
    x = (unsigned int*) md;
    sprintf((char*) ret, "%08"PRIx32"%08"PRIx32"%08"PRIx32"%08"PRIx32"%08"PRIx32, x[0],x[1],x[2],x[3],x[4]);
    memset(md, 0, 21); // Erase md, just in case.
    free(md);
    return ret;
}

