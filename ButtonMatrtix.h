#ifndef CHESS_CONTROLLER_BUTTON_MATRIX
#define CHESS_CONTROLLER_BUTTON_MATRIX

#include <Arduino.h>

#include "Coord.h"

const uint8_t row_mask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x01, 0x02};
const uint8_t col_mask[8] = {0x02, 0x01, 0x04, 0x08, 0x80, 0x40, 0x10, 0x20};

struct HC165Pin {
    int load;
    int clockEnable;
    int data;
    int clock;
};

struct HC595Pin {
    int latch;
    int clock;
    int data;
};

class ButtonMatrix {
   public:
    ButtonMatrix(struct HC595Pin hc595pin, struct HC165Pin hc165pin);

    /**
     * Returns a Coord of the current pushed button if there is only one.
     * Else returns nullptr.
     * 
     * The object must be deleted.
    */
    struct Coord *pushed();

   private:
    struct HC595Pin hc595pin;
    struct HC165Pin hc165pin;

    /**
     * Return a column with HIGH, if there is only one.
     * Else if there is none, return -1.
     * Else if there is more than one, return -1 * (count of HIGH columns).
     */
    int high_col();

    /**
     * Write HIGH or LOW to each row.
    */
    void write_row(uint8_t data);

    struct Coord *last;
    bool is_new;
};

#endif
