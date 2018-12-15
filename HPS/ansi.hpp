/* Header File for Pre-defined colored terminal output */

#ifndef ANSI_HPP
#define ANSI_HPP

/* ===== Define Colors for Pretty Colored Outputs ===== */

// https://misc.flogisoft.com/bash/tip_colors_and_formatting
#define ANSI_DFLT	"\e[39m"
#define ANSI_RESET	"\e[0m"

#define ANSI_BOLD	"\e[1m"
#define ANSI_UNDERL	"\e[4m"
#define ANSI_BLINK	"\e[5m"
#define ANSI_REVRS	"\e[7m"
#define ANSI_HIDDEN	"\e[8m"

#define ANSI_BLACK	"\e[30m"
#define ANSI_RED	"\e[31m"
#define ANSI_GREEN	"\e[32m"
#define ANSI_YELLOW	"\e[33m"
#define ANSI_BLUE	"\e[34m"
#define ANSI_PURPL	"\e[35m"
#define ANSI_CYAN	"\e[36m"

#define ANSI_LGRAY	"\e[37m"
#define ANSI_DGRAY	"\e[90m"
#define ANSI_LRED	"\e[91m"
#define ANSI_LGREEN	"\e[92m"
#define ANSI_LBLUE	"\e[94m"
#define ANSI_LPURPL	"\e[95m"
#define ANSI_LCYAN	"\e[96m"
#define ANSI_WHITE	"\e[97m"

#endif
