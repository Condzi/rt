#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "first.hpp"

#include "config.hpp"
#include "memory.hpp"

#include "memory.cpp"

using namespace rt;

int 
main(void) {
  gLog_File = fopen(RT_LOG_FILE, "w");

  logf("Logger initialized!\n");
  check_(false);
  dbg_check_(false);

  logf("Goodbye :)");
  fflush(gLog_File);
  
  return 0;
}