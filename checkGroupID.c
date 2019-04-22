#include <stdio.h>
#include <stdlib.h>
#include <grp.h>
#include <pwd.h>

int main(int argc, char *argv[])
{
    int j, ngroups;
    gid_t *groups;
    struct passwd *pw;
    struct group *gr;

    char *user = getenv("USER");

    ngroups = 50;

    groups = malloc(ngroups * sizeof (gid_t));
    if (groups == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /* Fetch passwd structure (contains first group ID for user) */

    pw = getpwnam(user);
    if (pw == NULL) {
        perror("getpwnam");
        exit(EXIT_SUCCESS);
    }

    /* Retrieve group list */

    if (getgrouplist(user, pw->pw_gid, groups, &ngroups) == -1) {
        fprintf(stderr, "getgrouplist() returned -1; ngroups = %d\n",
                ngroups);
        exit(EXIT_FAILURE);
    }

    /* Display list of retrieved groups, along with group names */

    fprintf(stderr, "User : %s belongs in %d groups\n", user, ngroups);
    for (j = 0; j < ngroups; j++) 
    {
        printf("%d", groups[j]);
        gr = getgrgid(groups[j]);
        if (gr != NULL)
            printf(" (%s)", gr->gr_name);
        printf("\n");
    }

    exit(EXIT_SUCCESS);
}

       