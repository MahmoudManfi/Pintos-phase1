#include "devices/timer.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include "devices/pit.h"
#include "threads/interrupt.h"
#include "threads/synch.h"
#include "threads/thread.h"

/* See [8254] for hardware details of the 8254 timer chip. */

#if TIMER_FREQ < 19
#error 8254 timer requires TIMER_FREQ >= 19
#endif
#if TIMER_FREQ > 1000
#error TIMER_FREQ <= 1000 recommended
#endif

/* Number of timer ticks since OS booted. */
static int64_t ticks;

/* Number of loops per timer tick.
   Initialized by timer_calibrate(). */
static unsigned loops_per_tick;

static intr_handler_func timer_interrupt;

static bool too_many_loops(unsigned loops);

static void busy_wait(int64_t loops);

static void real_time_sleep(int64_t num, int32_t denom);

static void real_time_delay(int64_t num, int32_t denom);

/* Begining our functions. */

void wakeup(void);

struct semaphore sema;

void advanced();

/* the comparetor function to compare the time. */
bool time_compare(const struct list_elem *first, const struct list_elem *second, void *aux);

/* the change the varible min_time with the nex mimimum from the list. */
void update_next(void);

int64_t min_time; /*the minimum time at which a thread will be unblocked. */

/* to block the thread for a period of time. */
void block_thread_time(int64_t);

/* to unblock the thread the block for a period of time. */
void unblock_thread_time();

/* The blocked list. */
struct list blocked_list;

/* Ending our functions. */

/* Sets up the timer to interrupt TIMER_FREQ times per second,
   and registers the corresponding interrupt. */
void
timer_init(void) {
    pit_configure_channel(0, 2, TIMER_FREQ);
    intr_register_ext(0x20, timer_interrupt, "8254 Timer");

    min_time = INT64_MAX;
    list_init(&blocked_list);
    sema_init(&sema,1);
}

/* Calibrates loops_per_tick, used to implement brief delays. */
void
timer_calibrate(void) {
    unsigned high_bit, test_bit;

    ASSERT(intr_get_level() == INTR_ON);
    printf("Calibrating timer...  ");

    /* Approximate loops_per_tick as the largest power-of-two
       still less than one timer tick. */
    loops_per_tick = 1u << 10;
    while (!too_many_loops(loops_per_tick << 1)) {
        loops_per_tick <<= 1;
        ASSERT(loops_per_tick != 0);
    }

    /* Refine the next 8 bits of loops_per_tick. */
    high_bit = loops_per_tick;
    for (test_bit = high_bit >> 1; test_bit != high_bit >> 10; test_bit >>= 1)
        if (!too_many_loops(loops_per_tick | test_bit))
            loops_per_tick |= test_bit;

    printf("%'"
    PRIu64
    " loops/s.\n", (uint64_t) loops_per_tick * TIMER_FREQ);
}

/* Returns the number of timer ticks since the OS booted. */
int64_t
timer_ticks(void) {
    enum intr_level old_level = intr_disable();
    int64_t t = ticks;
    intr_set_level(old_level);
    return t;
}

/* Returns the number of timer ticks elapsed since THEN, which
   should be a value once returned by timer_ticks(). */
int64_t
timer_elapsed(int64_t then) {
    return timer_ticks() - then;
}

/* Sleeps for approximately TICKS timer ticks.  Interrupts must
   be turned on. */
void
timer_sleep(int64_t ticks) {

    if (ticks <= 0) {
        return;
    }

    int64_t waited_time = timer_ticks() + ticks;

    ASSERT(intr_get_level() == INTR_ON);

    // Thread should wait.
    block_thread_time(waited_time);

    // Each thread wakeup another thread.
    unblock_thread_time(waited_time);

}

/* Sleeps for approximately MS milliseconds.  Interrupts must be
   turned on. */
void
timer_msleep(int64_t ms) {
    real_time_sleep(ms, 1000);
}

/* Sleeps for approximately US microseconds.  Interrupts must be
   turned on. */
void
timer_usleep(int64_t us) {
    real_time_sleep(us, 1000 * 1000);
}

/* Sleeps for approximately NS nanoseconds.  Interrupts must be
   turned on. */
void
timer_nsleep(int64_t ns) {
    real_time_sleep(ns, 1000 * 1000 * 1000);
}

/* Busy-waits for approximately MS milliseconds.  Interrupts need
   not be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_msleep()
   instead if interrupts are enabled. */
void
timer_mdelay(int64_t ms) {
    real_time_delay(ms, 1000);
}

/* Sleeps for approximately US microseconds.  Interrupts need not
   be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_usleep()
   instead if interrupts are enabled. */
void
timer_udelay(int64_t us) {
    real_time_delay(us, 1000 * 1000);
}

/* Sleeps execution for approximately NS nanoseconds.  Interrupts
   need not be turned on.

   Busy waiting wastes CPU cycles, and busy waiting with
   interrupts off for the interval between timer ticks or longer
   will cause timer ticks to be lost.  Thus, use timer_nsleep()
   instead if interrupts are enabled.*/
void
timer_ndelay(int64_t ns) {
    real_time_delay(ns, 1000 * 1000 * 1000);
}

/* Prints timer statistics. */
void
timer_print_stats(void) {
    printf("Timer: %"
    PRId64
    " ticks\n", timer_ticks());
}

/* Timer interrupt handler. */
static void
timer_interrupt(struct intr_frame *args UNUSED) {
    ticks++;

    if (ticks >= min_time)
        wakeup();
    if (thread_mlfqs)
        advanced();

    thread_tick();
}

/* Returns true if LOOPS iterations waits for more than one timer
   tick, otherwise false. */
static bool
too_many_loops(unsigned loops) {
    /* Wait for a timer tick. */
    int64_t start = ticks;
    while (ticks == start)
        barrier();

    /* Run LOOPS loops. */
    start = ticks;
    busy_wait(loops);

    /* If the tick count changed, we iterated too long. */
    barrier();
    return start != ticks;
}

/* Iterates through a simple loop LOOPS times, for implementing
   brief delays.

   Marked NO_INLINE because code alignment can significantly
   affect timings, so that if this function was inlined
   differently in different places the results would be difficult
   to predict. */
static void NO_INLINE
busy_wait (int64_t
loops)
{
while (loops-- > 0)

barrier();

}

/* Sleep for approximately NUM/DENOM seconds. */
static void
real_time_sleep(int64_t num, int32_t denom) {
    /* Convert NUM/DENOM seconds into timer ticks, rounding down.

          (NUM / DENOM) s
       ---------------------- = NUM * TIMER_FREQ / DENOM ticks.
       1 s / TIMER_FREQ ticks
    */
    int64_t ticks = num * TIMER_FREQ / denom;

    ASSERT(intr_get_level() == INTR_ON);
    if (ticks > 0) {
        /* We're waiting for at least one full timer tick.  Use
           timer_sleep() because it will yield the CPU to other
           processes. */
        timer_sleep(ticks);
    } else {
        /* Otherwise, use a busy-wait loop for more accurate
           sub-tick timing. */
        real_time_delay(num, denom);
    }
}

/* Busy-wait for approximately NUM/DENOM seconds. */
static void
real_time_delay(int64_t num, int32_t denom) {
    /* Scale the numerator and denominator down by 1000 to avoid
       the possibility of overflow. */
    ASSERT(denom % 1000 == 0);
    busy_wait(loops_per_tick * num / 1000 * TIMER_FREQ / (denom / 1000));
}

void block_thread_time(int64_t sleep_time) 
{

  struct thread *t = thread_current();

  t->waited_time = sleep_time;
  
  sema_down(&sema);

  if (min_time > sleep_time) {
    min_time = sleep_time;
  }
  
  list_insert_ordered(&blocked_list, &t->blocked_elem, time_compare, NULL);

  sema_up(&sema);

  enum intr_level old_level = intr_disable();
  
  thread_block();

  intr_set_level(old_level);

}

void unblock_thread_time(int64_t sleep_time) 
{
  sema_down(&sema);
  if (min_time <= sleep_time)
    {
      wakeup();
    }
  sema_up(&sema);
}


bool
time_compare(const struct list_elem *first, const struct list_elem *second, void *aux) {
    struct thread *t1 = list_entry(first,
    struct thread,blocked_elem);
    struct thread *t2 = list_entry(second,
    struct thread,blocked_elem);

    if ((t1->waited_time) == (t2->waited_time)) {
        return (t1->priority) > (t2->priority);
    }
    return (t1->waited_time) < (t2->waited_time);
}

/*
to update the minimum time to choose the next element.
*/
void
update_next()
{
  
  if (list_empty(&blocked_list)) 
  {
    min_time = INT64_MAX;
    return;
  }

  struct thread *t = list_entry(list_front(&blocked_list), struct thread , blocked_elem);
  
  min_time = t->waited_time;

}

void
wakeup() {

    struct list_elem *next = list_pop_front(&blocked_list);

    struct thread *t = list_entry(next, struct thread , blocked_elem);

    update_next();

    thread_unblock(t);

} 

void
advanced() {

    enum intr_level old_level = intr_disable();

    if (ticks%TIMER_FREQ == 0) {
        update_load_avg();
        update_recent_cpu();
    } else {
        thread_current()->recent_cpu = add(convert_tofixed_point(1),thread_current()->recent_cpu);
    }

    intr_set_level(old_level);

}