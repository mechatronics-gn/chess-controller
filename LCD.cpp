#include "LCD.h"

LCD::LCD(uint8_t addr, Timer *timer) {
    this->addr = addr;
    this->timer = timer;

    LiquidCrystal_I2C *lc = new LiquidCrystal_I2C(addr, LCD_COLS, LCD_ROWS);
    lc->init();
    lc->begin(16, 2);
    lc->clear();
    this->lc = lc;

    this->state = LCD_STATE_HELLO;

    this->is_from_set = false;
    this->is_to_set = false;

    char *tmp = new char[1];
    tmp[0] = '\0';
    this->algebraic_notation = tmp;
    this->algebraic_notation_lock = false;
}

void LCD::set_state(uint8_t state) { this->state = state; }

void LCD::set_edit_value(struct Coord coord) {
    if (!this->edit_target) return;

    switch (this->edit_target) {
        case LCD_EDIT_TARGET_FROM:
            this->from = coord;
            this->is_from_set = true;
            break;
        case LCD_EDIT_TARGET_TO:
            this->to = coord;
            this->is_to_set = true;
            break;
    }
}

void LCD::set_edit_target(uint8_t target) { this->edit_target = target; }

void LCD::reset_edit_target(uint8_t target) {
    switch (target) {
        case LCD_EDIT_TARGET_FROM:
            this->is_from_set = false;
            break;
        case LCD_EDIT_TARGET_TO:
            this->is_to_set = false;
            break;
    }
}

void LCD::set_move_algebraic_notation(char *notation) {
    while (this->algebraic_notation_lock) {
    }
    this->algebraic_notation_lock = true;
    char *tmp = this->algebraic_notation;
    this->algebraic_notation = notation;
    delete tmp;
    this->algebraic_notation_lock = false;
}

void LCD::update() {
    this->lc->backlight();
    this->lc->setCursor(0, 0);

    if (this->state == LCD_STATE_HELLO) {
        this->lc->print("     Hello!     ");
    } else if (this->state == LCD_STATE_STANDBY) {
        this->lc->print(" Press to Start ");
        goto PRINT_TIME;
    } else if (this->state == LCD_STATE_MOVING_CPU) {
        if (!this->algebraic_notation_lock) {
            this->algebraic_notation_lock = true;
            this->lc->print(sprintf(" CPU: %s", this->algebraic_notation));
            this->algebraic_notation_lock = false;
        } else {
            this->lc->print(" CPU:           ");
        }
        goto PRINT_TIME;
    } else if (this->state == LCD_STATE_MOVING_PLAYER) {
        if (!this->algebraic_notation_lock) {
            this->algebraic_notation_lock = true;
            this->lc->print(sprintf(" You: %s", this->algebraic_notation));
            this->algebraic_notation_lock = false;
        } else {
            this->lc->print(" You:           ");
        }
        goto PRINT_TIME;
    } else if (this->state == LCD_STATE_WAIT_PLAYER) {
        char *from_coord = this->is_from_set ? coord_display(this->from)
                           : (millis() / 1000) % 4 > 2
                               ? new char[3]{'_', '_', '\0'}
                               : new char[3]{' ', ' ', '\0'};
        char *to_coord = this->is_to_set ? coord_display(this->to)
                         : (this->is_from_set && (millis() / 1000) % 4 > 2)
                             ? new char[3]{'_', '_', '\0'}
                             : new char[3]{' ', ' ', '\0'};
        this->lc->print(sprintf(" You: %s -> %s  ", from_coord, to_coord));
        delete from_coord;
        delete to_coord;
        goto PRINT_TIME;
    } else if (this->state == LCD_STATE_VICTORY) {
        this->lc->print("    Victory!    ");
        this->lc->setCursor(0, 1);
        this->lc->print("                ");
    } else if (this->state == LCD_STATE_GAMEOVER) {
        this->lc->print("   Game  Over   ");
        this->lc->setCursor(0, 1);
        this->lc->print("                ");
    } else if (this->state == LCD_STATE_WARN_PLAYER) {
        this->lc->print(" Illegal! Retry ");
        goto PRINT_TIME;
    }

    return;
PRINT_TIME:
    this->lc->setCursor(4, 1);
    char *time = time_display(this->timer->read());
    this->lc->print(time);
    delete time;
    return;
}

char *coord_display(struct Coord coord) {
    char *ret = new char[3];
    ret[0] = (char)coord.x + 97;  // 0 -> 'a'
    ret[1] = (char)coord.y + 49;  // 0 -> '1'
    ret[2] = '\0';

    return ret;
}

char *time_display(uint32_t time) {
    unsigned long mins = time / 60000;
    unsigned long secs = (time % 60000) / 1000;
    unsigned long msecs = time % 1000;

    char *ret = new char[9];
    sprintf(ret, "%.2lu:%.2lu:%.2lu", mins, secs, msecs / 10);

    return ret;
}
