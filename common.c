#include "common.h"

int setnonblocking (int sfd)
{
    int flags;
    if((flags = fcntl (sfd, F_GETFL, 0)) == -1)
    {
        WriteLogPError("fcntl()");
        return -1;
    }

    flags |= O_NONBLOCK;
    if((fcntl (sfd, F_SETFL, flags)) == -1)
    {
        WriteLogPError("fcntl()");
        return -1;
    }

    return 0;
}

void WriteLog(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsyslog(LOG_LOCAL0, format, args);
    va_end(args);

    return;
}
void WriteLogPError(const char * strerr)
{
    int err=errno;
    syslog(LOG_LOCAL0, "%s%s%s\n", strerr, ": ", strerror(err));

    return;
}
// for 32 bits int it is sufficient to allocate an array 12 bytes(12 chars).
int int_to_str(char *str, int number)
{
    return sprintf(str,"%d", number);
}

long long_to_str(char *str, long number)
{
    return sprintf(str,"%lu", number);
}
int checkRegularFile(int fd)
{
    struct stat sb;
    fstat(fd, &sb);
    switch (sb.st_mode & S_IFMT)
    {
    case S_IFREG:
        return 0;
    default:
        return -1;
    }
}

// Function to remove all spaces from a given string
void removeSpaces(char *str)
{
    // To keep track of non-space character count
    int count = 0;

    // Traverse the given string. If current character
    // is not space, then place it at index 'count++'
    for (int i = 0; str[i]; i++)
        if (str[i] != ' ')
            str[count++] = str[i]; // here count is
    // incremented
    str[count] = '\0';
}
void replace_trailing(char* str)
{

    char *pos;
    if ((pos=strchr(str, '\n')) != NULL)
        *pos = '\0';
    return;

}
int check_if_str_commented_or_blank(const char* str, char comment_sign, size_t strlen)
{
    /* returns 1 if string commented, 0 if no */
    char* newstr;
    char* s;
    size_t i=0;

    if((newstr=(char*)malloc(strlen*sizeof (char))) == NULL)
        return -1;
    strncpy(newstr, str, strlen);

    if((s=strchr(newstr,comment_sign)) != NULL)
        *s=0;
    else if((s=strchr(newstr,0)) == NULL)
    {
        free(newstr);
        return 0;
    }

    while((isblank(newstr[i])) != 0)
        i++;

    if((newstr+i) != s)
    {
        free(newstr);
        return 0;
    }
    free(newstr);
    return 1;
}
