#ifndef CONFIG_H
#define CONFIG_H

// Customizable preferences for fine tuning
#define BTN_DEBOUNCE_USEC (30 * 1000)   // 30 ms in u_sec
#define INTERRUPT_WAIT_USEC (100 * 1000) // 100 ms in u_sec

// Injected during the build process
#ifndef BUILD_TIMESTAMP
#define BUILD_TIMESTAMP "unknown"
#endif

#endif
