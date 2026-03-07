#pragma once

// terminal visuals
#define RESET     "\033[0m"
#define BOLD      "\033[1m"
#define DIM       "\033[2m"

// Gruvbox palette
#define COLOR_WHITE       "\033[38;5;223m"   // gruvbox fg
#define COLOR_GOLD        "\033[38;5;214m"   // gruvbox yellow
#define COLOR_GRAY        "\033[38;5;245m"   // gruvbox gray
#define COLOR_CORAL       "\033[38;5;167m"   // gruvbox red
#define COLOR_GREEN       "\033[38;5;142m"   // gruvbox green
#define COLOR_LIGHT_GREY  "\033[38;5;250m"   // light foreground

#define COLOR_ALLOC   COLOR_CORAL
#define COLOR_FREE    COLOR_GREEN
#define COLOR_TITLE   COLOR_GOLD BOLD
#define COLOR_BORDER  COLOR_GRAY
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
