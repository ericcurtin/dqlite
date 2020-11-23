#include "fault.h"
#include "munit.h"

void testFault_init(struct testFault *f)
{
    f->countdown = -1;
    f->n = -1;
    f->enabled = false;
}

bool testFault_tick(struct testFault *f)
{
    if (MUNIT_UNLIKELY(!f->enabled)) {
        return false;
    }

    /* If the initial delay parameter was set to -1, then never fail. This is
     * the most common case. */
    if (MUNIT_LIKELY(f->countdown < 0)) {
        return false;
    }

    /* If we did not yet reach 'delay' ticks, then just decrease the countdown.
     */
    if (f->countdown > 0) {
        f->countdown--;
        return false;
    }

    munit_assert_int(f->countdown, ==, 0);

    /* We reached 'delay' ticks, let's see how many times we have to trigger the
     * fault, if any. */

    if (f->n < 0) {
        /* Trigger the fault forever. */
        return true;
    }

    if (f->n > 0) {
        /* Trigger the fault at least this time. */
        f->n--;
        return true;
    }

    munit_assert_int(f->n, ==, 0);

    /* We reached 'repeat' ticks, let's stop triggering the fault. */
    f->countdown--;

    return false;
}

void testFault_config(struct testFault *f, int delay, int repeat)
{
    f->countdown = delay;
    f->n = repeat;
}

void testFault_enable(struct testFault *f)
{
    f->enabled = true;
}
