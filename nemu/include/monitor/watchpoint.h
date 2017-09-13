#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  char * expr;
  uint32_t original_value;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */


} WP;

bool free_wp(int NO);
WP * new_wp();


#endif
