#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <limits.h>

#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <stdarg.h>
#include <sys/stat.h>

void WriteLog(const char *format, ...);
void WriteLogPError(const char*);
int setnonblocking (int);
int int_to_str(char *str, int number);
long long_to_str(char *str, long number);

#endif //COMMON_H
