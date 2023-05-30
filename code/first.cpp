#include <cstdio>
#include <cstdlib>

#include "first.hpp"

#include "config.hpp"

int 
main(void) {
  g_LogFile = fopen(RT_LOG_FILE, "w");

  logf("Logger initialized!\n");
  check_(false);
  dbg_check_(false);

  logf("Goodbye :)");
  fflush(g_LogFile);
  
  return 0;
}