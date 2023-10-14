#ifndef CHESS_CONTROLLER_LCD
#define CHESS_CONTROLLER_LCD

#include <LiquidCrystal_I2C.h>

#include "Coord.h"
#include "Timer.h"

#define LCD_COLS (16)
#define LCD_ROWS (2)

/* This displays a simple hello message */
#define LCD_STATE_HELLO (0)
/* This shows the timer, which is not passing, before starting the game */
#define LCD_STATE_STANDBY (1)
/* This waits until cpu move by motor is done */
#define LCD_STATE_MOVING_CPU (2)
/* This waits until player move by motor is done */
#define LCD_STATE_MOVING_PLAYER (3)
/* This waits player input */
#define LCD_STATE_WAIT_PLAYER (4)
/* This is player victory */
#define LCD_STATE_VICTORY (5)
/* This is cpu victory */
#define LCD_STATE_GAMEOVER (6)
/* This warns player for trying an illegal move */
#define LCD_STATE_WARN_PLAYER (7)

#define LCD_EDIT_TARGET_NONE (0)
#define LCD_EDIT_TARGET_FROM (1)
#define LCD_EDIT_TARGET_TO (2)

class LCD {
   public:
    LCD(uint8_t addr, Timer *timer);

    void set_state(uint8_t state);

    /**
     * Set the edit target of which we are aiming to change coord value.
     */
    void set_edit_target(uint8_t target);
    void set_edit_value(struct Coord coord);
    void reset_edit_target(uint8_t target);

    /**
     * Notation must be shorter than 8 characters (excluding \0)
    */
    void set_move_algebraic_notation(char *notation);

    void update();

   private:
    uint8_t addr;
    LiquidCrystal_I2C *lc;
    Timer *timer;

    uint8_t state;

    bool is_from_set;
    struct Coord from;
    bool is_to_set;
    struct Coord to;

    uint8_t edit_target;

    char *algebraic_notation;
    bool algebraic_notation_lock;
};

char *coord_display(struct Coord coord);

char *time_display(uint32_t time);

#endif
