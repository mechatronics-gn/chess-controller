#include "Timer.h"

Timer::Timer() {
    this->offset_millis = millis();
    this->leftover_millis = 0;
    this->is_running = false;
}

void Timer::pause() {
    this->is_running = false;
    this->leftover_millis -= millis() - this->offset_millis;
}

void Timer::resume() {
    this->offset_millis = millis();
    this->is_running = true;
}

void Timer::reset(uint32_t time) {
    this->is_running = false;
    this->leftover_millis = time;
}

uint32_t Timer::read() {
    if (this->is_running) {
        return this->offset_millis + this->leftover_millis - millis();
    } else {
        return this->leftover_millis;
    }
}