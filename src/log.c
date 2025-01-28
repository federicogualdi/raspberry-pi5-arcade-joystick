#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include "include/log.h"

// Current log level (default to INFO)
int current_log_level = LOG_LEVEL_INFO;

// Convert log level to string
const char* log_level_to_str(int level) {
    switch (level) {
        case LOG_LEVEL_DEBUG: return "DEBUG";
        case LOG_LEVEL_INFO:  return "INFO";
        case LOG_LEVEL_ERROR: return "ERROR";
        default:              return "UNKNOWN";
    }
}

// Get current time string
const char* get_time_str() {
    static char buffer[20];
    time_t now = time(NULL);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
    return buffer;
}

// Log a message
void log_message(int level, const char* format, ...) {
    if (level >= current_log_level) {
        fprintf(stderr, "[%s] [%s] ", get_time_str(), log_level_to_str(level));

        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);

        fprintf(stderr, "\n");
    }
}

// Set log level from command line arguments
void set_log_level_from_args(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--log-level") == 0 && i + 1 < argc) {
            if (strcmp(argv[i + 1], "DEBUG") == 0) {
                current_log_level = LOG_LEVEL_DEBUG;
            } else if (strcmp(argv[i + 1], "INFO") == 0) {
                current_log_level = LOG_LEVEL_INFO;
            } else if (strcmp(argv[i + 1], "ERROR") == 0) {
                current_log_level = LOG_LEVEL_ERROR;
            } else {
                fprintf(stderr, "Unknown log level: %s\n", argv[i + 1]);
            }
        }
    }
}
