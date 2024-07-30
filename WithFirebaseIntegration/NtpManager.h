#ifndef NTPMANAGER_H
#define NTPMANAGER_H

#include <time.h>

void initNtp();
void updateTime();
void printCurrentTime();

extern bool ntp_initialized;


#endif