#ifndef LOG_H
#define LOG_H

// Logging levels
#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_ERROR 2

extern int current_log_level;

// Function prototypes
const char* log_level_to_str(int level);
const char* get_time_str();
void log_message(int level, const char* format, ...);
void set_log_level_from_args(int argc, char* argv[]);

#endif
