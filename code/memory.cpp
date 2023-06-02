namespace rt {
struct Memory_State {
  Buffer temp_memory;
  s64 temp_mark;
} static gMemory_State;

[[nodiscard]] bool
init_memory() {
  void *temp_memory_from_system = ::malloc(TEMP_MEM_SIZE);
  dbg_check_(temp_memory_from_system);
  if (!temp_memory_from_system) {
    return false;
  }

  ::memset(&gMemory_State, 0, sizeof(gMemory_State));
  ::memset(temp_memory_from_system, 0, TEMP_MEM_SIZE);

  gMemory_State.temp_memory.count = TEMP_MEM_SIZE;
  gMemory_State.temp_memory.bytes = (u8*)temp_memory_from_system;

  return true;
}

[[nodiscard]] void* 
alloc_perm(s64 size) {
  dbg_check_(size > 0);

  void *mem = ::malloc(size);
  if (!mem) {
    errf("::malloc failed, size=%lld", size);
  }

  ::memset(mem, 0, size);
  return mem;
}

[[nodiscard]] void* 
alloc_temp(s64 size) {
  dbg_check_(size > 0);
  dbg_check_(gMemory_State.temp_mark + size > TEMP_MEM_SIZE);

  if (gMemory_State.temp_mark + size > TEMP_MEM_SIZE) {
    return alloc_perm(size);
  }

  u8 *mem = gMemory_State.temp_memory.bytes + gMemory_State.temp_mark;
  gMemory_State.temp_mark += size;

  // @Note: the memory is cleared every time we do clear_temp_mem()
  return mem;
}

[[nodiscard]] s64
get_temp_mem_mark() {
  return gMemory_State.temp_mark;
}

void
pop_temp_mem_mark(s64 size) {
  dbg_check_(size >= 0 && size <= gMemory_State.temp_mark);

  gMemory_State.temp_mark -= size;
  ::memset(gMemory_State.temp_memory.bytes + gMemory_State.temp_mark, 0, size);
}

void 
clear_temp_mem() {
  ::memset(gMemory_State.temp_memory.bytes, 0, gMemory_State.temp_mark);
}
} // namespace rt