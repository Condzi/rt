/**
 * Functions related to retrieving information about errors and handling crashes.
 */

namespace rt {
[[nodiscard]] String
os_error_to_string(u32 error_code);

[[nodiscard]] u32
os_get_last_error();

// @Note: VEH is added only when debugger is not present because otherwise we trap
//        debugger related exceptions.
void
os_register_vectored_exception_handler();
} // namespace rt