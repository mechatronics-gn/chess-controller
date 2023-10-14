#include "ButtonMatrtix.h"

ButtonMatrix::ButtonMatrix(struct HC595Pin hc595pin, struct HC165Pin hc165pin) {
    this->hc595pin = hc595pin;
    this->hc165pin = hc165pin;

    pinMode(hc595pin.latch, OUTPUT);
    pinMode(hc595pin.clock, OUTPUT);
    pinMode(hc595pin.data, OUTPUT);

    pinMode(hc165pin.load, OUTPUT);
    pinMode(hc165pin.clockEnable, OUTPUT);
    pinMode(hc165pin.clock, OUTPUT);
    pinMode(hc165pin.data, INPUT);
}

struct Coord *ButtonMatrix::pushed() {
    /* First check if only one column is HIGH */
    this->write_row(0xFF);
    int high_col = this->high_col();
    if (high_col < 0) return nullptr;

    int floor = 0, rear = 8;
    while (rear > floor + 1) {
        delayMicroseconds(5);
        int mid = (floor + rear) / 2;

        /* Some clever bitwise magic */
        uint8_t floor_data = (1 << (8 - floor)) - 1;
        uint8_t mid_data = (1 << (8 - mid)) - 1;
        uint8_t rear_data = (1 << (8 - rear)) - 1;

        this->write_row(floor_data - mid_data);
        int left = this->high_col();
        if (left == high_col) {
            rear = mid;
        } else if (left == -1) {
            floor = mid;
        } else {
            return nullptr;
        }
    }

    return new Coord{.x = (uint8_t)high_col, .y = (uint8_t)floor};
}

int ButtonMatrix::high_col() {
    digitalWrite(this->hc165pin.load, LOW);
    delayMicroseconds(5);
    digitalWrite(this->hc165pin.load, HIGH);
    delayMicroseconds(5);

    digitalWrite(this->hc165pin.clock, HIGH);
    digitalWrite(this->hc165pin.clockEnable, LOW);
    uint8_t data =
        ~shiftIn(this->hc165pin.data, this->hc165pin.clock, MSBFIRST);
    digitalWrite(this->hc165pin.clockEnable, HIGH);

    int ret = -1;
    int cnt = 0;
    for (int i = 0; i < 8; i++) {
        if (data & col_mask[i]) {
            ret = i;
            cnt++;
        }
    }

    if (cnt <= 1) {
        return ret;
    } else {
        return -cnt;
    }
}

void ButtonMatrix::write_row(uint8_t data) {
    digitalWrite(this->hc595pin.latch, LOW);
    shiftOut(this->hc595pin.data, this->hc595pin.clock, MSBFIRST, data);
    digitalWrite(this->hc595pin.latch, HIGH);
}