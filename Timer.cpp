#include "Timer.h"

Timer::Timer() {
    this->offset_millis = millis();
    this->leftover_millis = 0;
    this->is_running = false;
}

void Timer::pause() {
    this->is_running = false;
    if(this->leftover_millis >= millis() - this->offset_millis)
        this->leftover_millis -= millis() - this->offset_millis;
    else
        this->leftover_millis = 0;
}

void Timer::resume() {
    this->offset_millis = millis();
    this->is_running = true;
}

void Timer::reset(uint32_t time) {
    this->offset_millis = millis();
    this->is_running = false;
    this->leftover_millis = time;
}

uint32_t Timer::read() {
    if (this->is_running) {
        if(this->offset_millis + this->leftover_millis > millis())
            return this->offset_millis + this->leftover_millis - millis();
        else return 0;
    } else {
        return this->leftover_millis;
    }
}
