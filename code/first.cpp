#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "first.hpp"

#include "config.hpp"
#include "memory.hpp"

#include "memory.cpp"

using namespace rt;

template <typename ...TArgs> [[noreturn]] void
errf(char const *fmt, TArgs ...args) {
  // @Todo: print last OS error here.
  logf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  logf("\tFatal error!\n  ");
  logf(fmt, args...);
  logf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");

  ::fflush(stdout);
  ::fflush(gLog_File);
  ::exit(0);
}

int 
main(void) {
  gLog_File = fopen(RT_LOG_FILE, "w");

  logf("Logger initialized!\n");

  if (!init_memory()) {
    logf("init_memory failed\n"); // @todo: errf
    return 1;
  }

  check_(false);
  dbg_check_(false);

  logf("Goodbye :)");
  fflush(gLog_File);
  
  return 0;
}