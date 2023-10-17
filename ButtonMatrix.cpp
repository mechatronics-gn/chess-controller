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
    delayMicroseconds(10);
    int high_col = this->high_col();
    if (high_col < 0) return nullptr;

    int floor = 0, rear = 8;
    while (rear > floor + 1) {
        delayMicroseconds(5);
        int mid = (floor + rear) / 2;

        uint8_t data = 0;
        for(int i=floor; i<mid; i++) {
            data |= row_mask[i];
        }        

        this->write_row(data);
        delayMicroseconds(10);
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
        ~shiftIn(this->hc165pin.data, this->hc165pin.clock, LSBFIRST);
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
    shiftOut(this->hc595pin.data, this->hc595pin.clock, LSBFIRST, data);
    digitalWrite(this->hc595pin.latch, HIGH);
}
