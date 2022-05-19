#ifndef __DEBUG_H__
#define __DEBUG_H__
#include <stdint.h>
#include <stdio.h>
#include <windows.h>

enum 
{
  DEBUG_LOG_NONE,
  DEBUG_LOG_VERBOSE,
  DEBUG_LOG_ERROR,
  DEBUG_LOG_COUNT,
};
extern int debug_level;
//int debug_level = DEBUG_LOG_VERBOSE;

//  if (debug_level >= DEBUG_LOG_VERBOSE)
#define debug_verbose(...) \
  if (0) \
  { \
    printf("VERBOSE::%s::%d::%s - ", __FILE__, __LINE__, __func__); \
    printf(__VA_ARGS__); \
    printf("\n"); \
  }

//  if (debug_level >= DEBUG_LOG_VERBOSE) 
#define debug_error(...) \
  if (1) \
  { \
    printf("ERROR::%s::%d::%s - ", __FILE__, __LINE__, __func__); \
    printf(__VA_ARGS__); \
    printf("\n"); \
  }

void debug_print_mem_basic_flags(MEMORY_BASIC_INFORMATION *memInfo);
void debug_print_last_win_error(void);
#endif

