#ifndef CHESS_CONTROLLER_TIMER
#define CHESS_CONTROLLER_TIMER

#include <Arduino.h>

class Timer {
   public:
    Timer();

    void pause();
    void resume();

    /**
     * Reset the timer to given time (in milliseconds).
     * An additional call to resume() is required to run the timer.
     */
    void reset(uint32_t time);

    uint32_t read();

   private:
    /**
     * millis() of when the timer resumed recently.
    */
    uint32_t offset_millis;
    /**
     * Left time when the timer resumed recently in milliseconds.
    */
    uint32_t leftover_millis;
    /**
     * Whether the timer is currently running.
    */
    bool is_running;
};

#endif