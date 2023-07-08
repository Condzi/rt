/**
 * Tracking time & retrieving info about current date
 */
namespace rt {
struct Time_of_Day {
  s32 day, month, year;
  s32 hours, minutes, seconds;
};

void
os_start_app_timer();

[[nodiscard]] f32
os_get_app_uptime();

// hh:mm:ss, count=const=8.
[[nodiscard]] String
os_get_app_uptime_as_string();

[[nodiscard]] Time_of_Day
os_get_time_of_day();
} // namespace rt