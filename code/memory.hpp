/**
 * We assume that allocation functions never fail. If somehow the allocation fails,
 * we just crash (with some good error message). In case of temp memory, we fallback
 * (and leak) the permanent memory.
 *                                                        - kkubacki, 30th May 2023
*/

namespace rt {

[[nodiscard]] bool
init_memory();

[[nodiscard]] void* 
alloc_perm(s64 size);

[[nodiscard]] void* 
alloc_temp(s64 size);

[[nodiscard]] s64
get_temp_mem_mark();

// Move the memory mark back. It will make the memory available to allocation again.
void
pop_temp_mem_mark(s64 size);

void 
clear_temp_mem();
} // namespace rt