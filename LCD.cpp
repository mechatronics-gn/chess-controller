#include "LCD.h"

LCD::LCD(uint8_t addr) {
    this->addr = addr;

    LiquidCrystal_I2C *lc = new LiquidCrystal_I2C(addr, LCD_COLS, LCD_ROWS);
    lc->init();
    lc->begin(16, 2);
    lc->clear();
    this->lc = lc;

    this->state = LCD_STATE_HELLO;

    char *tmp = malloc(sizeof(char));
    tmp[0] = "\0";
    this->algebraic_notation = tmp;
    this->algebraic_notation_lock = false;
}

void LCD::set_state(uint8_t state) {
    this->state = state;
}

void LCD::set_edit_value(uint8_t x, uint8_t y) {
    if (!this->state) return;

    switch (this->state) {
        case LCD_EDIT_TARGET_FROM:
            this->from_x = x;
            this->from_y = y;
            break;
        case LCD_EDIT_TARGET_TO:
            this->to_x = x;
            this->to_y = y;
            break;
    }
}

void LCD::set_edit_target(uint8_t target) {
    this->edit_target = target;
}

void LCD::set_move_algebraic_notation(char *notation) {
    while (this->algebraic_notation_lock) {}
    this->algebraic_notation_lock = true;
    char *tmp = this->algebraic_notation;
    this->algebraic_notation = notation;
    free(tmp);
    this->algebraic_notation_lock = false;
}

void LCD::update() { // 미래의 나 화이팅.
    
}

char *coord_display(uint8_t x, uint8_t y) {
    char *ret = malloc(3 * sizeof(char));
    ret[0] = (char)x + 97; // 0 -> 'a'
    ret[1] = (char)x + 49; // 0 -> '1'
    ret[2] = '\0';

    return ret;
}
