#ifndef TRELL_SYSLOG_H
#define TRELL_SYSLOG_H

#include "../tui/tui.h"

typedef enum {
    SYSLOG_INFO = 'I',
    SYSLOG_WARNING = 'W',
    SYSLOG_ERROR = 'E'
} syslog_severity_t;

tui_window_t* syslog_create();
void syslog_log(uint32_t pid, syslog_severity_t sev, char* data, ...);

#endif
