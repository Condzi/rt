/**
 * Spinlock implementation from https://rigtorp.se/spinlock
 */

namespace rt {
struct Spinlock {
  std::atomic<bool> lock {false};
};

void
lock(Spinlock &spinlock);

void
unlock(Spinlock &spinlock);
} // namespace rt
