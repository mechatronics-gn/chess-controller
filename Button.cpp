#include "Button.h"

Button::Button(int pin) {
    this->pin = pin;
    this->last_state = false;
    this->is_new = false;
    pinMode(pin, INPUT_PULLUP);
}

void Button::update() {
    bool data = !digitalRead(pin);
    if (data && !this->last_state) {
        this->is_new = true;
    }
    this->last_state = data;
}

bool Button::is_clicked() {
    if (this->last_state && this->is_new) {
        this->is_new = false;
        return true;
    }
    return false;
}