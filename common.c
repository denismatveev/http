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
