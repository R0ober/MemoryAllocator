#pragma once

// terminal visuals
#define RESET     "\033[0m"
#define BOLD      "\033[1m"
#define DIM       "\033[2m"

// Base palette
#define COLOR_WHITE   "\033[1;97m"        // bright white 
#define COLOR_GOLD    "\033[38;5;220m"    // warm gold 
#define COLOR_GRAY    "\033[38;5;244m"    // muted gray 
#define COLOR_CORAL   "\033[38;5;203m"    // soft coral
#define COLOR_GREEN   "\033[38;5;114m"    // soft green 
#define COLOR_LIGHT_GREY "\033[38;5;248m"  // light grey 

#define COLOR_ALLOC   COLOR_CORAL
#define COLOR_FREE    COLOR_GREEN
#define COLOR_TITLE   COLOR_GOLD BOLD
#define COLOR_BORDER  COLOR_GOLD
#define COLOR_ADDR    COLOR_LIGHT_GREY

#define BLOCK_FULL    "█"
#define BLOCK_EMPTY   "░"
#define BLOCK_ALLOC   "█"

#define SEP    "│"
#define BOX_TL "╭"
#define BOX_TR "╮"
#define BOX_BL "╰"
#define BOX_BR "╯"
#define BOX_H  "─"
#define BOX_V  "│"
#define BOX_LT "├"
#define BOX_RT "┤"




