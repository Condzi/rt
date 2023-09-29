namespace rt {
void
lock(Spinlock &spinlock) {
  for (;;) {
    if (!spinlock.lock.exchange(true, std::memory_order_acquire)) {
      return;
    }

    while (spinlock.lock.load(std::memory_order_relaxed)) {
      // @Note: we want to notify the OS that this is a tight loop.
      // Is this the best way to do this?
      ::YieldProcessor();
    }
  }
};

void
unlock(Spinlock &spinlock) {
  spinlock.lock.store(false, std::memory_order_release);
}
} // namespace rt
