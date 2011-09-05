/*

tildexpand.c

Author: Tatu Ylonen <ylo@cs.hut.fi>

Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>, Espoo, Finland
                   All rights reserved

Created: Wed Jul 12 01:07:36 1995 ylo

*/

#include "includes.h"
#include "xmalloc.h"
#include "ssh.h"

/* Expands tildes in the file name.  Returns data allocated by xmalloc.
   Warning: this calls getpw*. */

char *tilde_expand_filename(const char *filename, uid_t my_uid)
{
    const char *cp;
    unsigned int userlen;
    char *expanded;
    struct passwd *pw;
    char user[100];
    const char *homedir;

    /* Return immediately if no tilde. */
    if (filename[0] != '~')
        return xstrdup(filename);


    /* Skip the tilde. */
    filename++;

    /* Find where the username ends. */
    cp = strchr(filename, '/');
    if (cp)
        userlen = cp - filename;        /* Have something after username. */
    else
        userlen = strlen(filename);     /* Nothign after username. */
    if (userlen == 0) {
        strcpy(user, "(self)");
        pw = getpwuid(my_uid);  /* Own home directory. */
    } else {
        /* Tilde refers to someone elses home directory. */
        if (userlen > sizeof(user) - 1)
            fatal("User name after tilde too long.");
        memcpy(user, filename, userlen);
        user[userlen] = 0;
        pw = getpwnam(user);
    }

    /* Check that we found the user. */
    if (!pw)
        fatal("Unknown user %.100s.", user);

    homedir = pw->pw_dir;

    /* If referring to someones home directory, return it now. */
    if (!cp) {                  /* Only home directory specified */
        return xstrdup(homedir);
    }

    /* Build a path combining the specified directory and path. */
    expanded = xmalloc(strlen(homedir) + strlen(cp + 1) + 2);
    snprintf(expanded, strlen(homedir) + strlen(cp + 1) + 2, "%s/%s", homedir, cp + 1);
    return expanded;
}
