#include "Button.h"
#include "ButtonMatrtix.h"
#include "LCD.h"
#include "Timer.h"

/* Inbound messages */

/* Start a new game. Followed by a 4-byte int */
#define MESSAGE_IN_RESET_GAME (0x00)
/* Latest given coords are legal */
#define MESSAGE_IN_GIVEN_COORDS_OK (0x01)
/* Latest given coords are illegal */
#define MESSAGE_IN_GIVEN_COORDS_ERROR (0x02)
/* An algebraic expression to display. Followed by a string */
#define MESSAGE_IN_ALGEBRAIC_EXPRESSION (0x03)
/* It is your turn */
#define MESSAGE_IN_YOUR_TURN (0x04)
/* It is cpu turn */
#define MESSAGE_IN_CPU_TURN (0x05)
/* It is your win */
#define MESSAGE_IN_YOUR_WIN (0x06)
/* It is cpu win */
#define MESSAGE_IN_CPU_WIN (0x07)

/* Outbound messages */

/* Start a new game */
#define MESSAGE_OUT_RESET_GAME (0xAB)
/* Write my coords */
#define MESSAGE_OUT_COORDS (0xAC)

/* Game input processing states */

/* None */
#define GAME_STATE_NONE (-1)
/* Wait for game to start*/
#define GAME_STATE_STANDBY (0)
/* Wait for 'from' coord input */
#define GAME_STATE_WAIT_INPUT_FROM (1)
/* Wait for 'to' coord input */
#define GAME_STATE_WAIT_INPUT_TO (2)
/* Victory or game over */
#define GAME_STATE_DONE (3)
int state = GAME_STATE_NONE;

LCD *lcd;
ButtonMatrix *btnmtx;
Timer *timer;
Button *cancelbtn;

struct Coord from, to;

void setup() {
    // put your setup code here, to run once:
    struct HC595Pin hc595 = {.latch = 9, .clock = 10, .data = 8};
    struct HC165Pin hc165 = {
        .load = 6, .clockEnable = 4, .data = 5, .clock = 7};
    btnmtx = new ButtonMatrix(hc595, hc165);
    cancelbtn = new Button(2);
    timer = new Timer();
    lcd = new LCD(0x27, timer);
    lcd->set_state(LCD_STATE_HELLO);

    lcd->update();
    cancelbtn->update();

    Serial.begin(9600);
    Serial.write(MESSAGE_OUT_RESET_GAME); /* Hey, I'm here! */
    delay(1000);
}

void loop() {
    // put your main code here, to run repeatedly:
    /* // Test btnmtx
    auto *x = btnmtx->pushed()/;
    if(x != nullptr) {
        Serial.print("Coord: (");
        Serial.print(x->x);
        Serial.print(", ");
        Serial.print(x->y);
        Serial.println(")");
        delete x;
    }
    delay(1);
    */

    /* Phase 1: Read from Serial */
    if (Serial.available()) {
        byte inc = Serial.read();
        if (inc == MESSAGE_IN_RESET_GAME) {
            /* Four bytes for next game duration (Big Endian) */
            byte arr[4];
            Serial.readBytes(arr, 4);
            uint32_t game_duration = 0;
            for (int i = 0; i < 4; i++) {
                game_duration += arr[i];
                game_duration << 8;
            }

            timer->reset(game_duration);
            state = GAME_STATE_STANDBY;
            lcd->set_state(LCD_STATE_STANDBY);
        } else if (inc == MESSAGE_IN_GIVEN_COORDS_OK) {
            state = GAME_STATE_NONE;
            lcd->set_move_algebraic_notation(new char[1]{'\0'});
            lcd->set_state(LCD_STATE_MOVING_PLAYER);
        } else if (inc == MESSAGE_IN_GIVEN_COORDS_ERROR) {
            state = GAME_STATE_WAIT_INPUT_FROM;
            timer->resume();
            lcd->set_state(LCD_STATE_WARN_PLAYER);
        } else if (inc == MESSAGE_IN_ALGEBRAIC_EXPRESSION) {
            byte len = Serial.read();
            char *exp = new char[len + 1];
            Serial.readBytes(exp, len);
            exp[len] = '\0';
            lcd->set_move_algebraic_notation(exp);
        } else if (inc == MESSAGE_IN_YOUR_TURN) {
            state = GAME_STATE_WAIT_INPUT_FROM;
            timer->resume();
            lcd->set_state(LCD_STATE_WAIT_PLAYER);
        } else if (inc == MESSAGE_IN_CPU_TURN) {
            state = GAME_STATE_NONE;
            lcd->set_move_algebraic_notation(new char[1]{'\0'});
            lcd->set_state(LCD_STATE_MOVING_CPU);
        } else if (inc == MESSAGE_IN_YOUR_WIN) {
            state = GAME_STATE_DONE;
            timer->pause();
            lcd->set_state(LCD_STATE_VICTORY);
        } else if (inc == MESSAGE_IN_CPU_WIN) {
            state = GAME_STATE_DONE;
            timer->pause();
            lcd->set_state(LCD_STATE_GAMEOVER);
        }
    }

    /* Phase 2: Do local work */
    auto *x = btnmtx->pushed();
    if (x != nullptr) {
        if (state == GAME_STATE_STANDBY) {
            state = GAME_STATE_WAIT_INPUT_FROM;
            lcd->set_state(LCD_STATE_WAIT_PLAYER);
            timer->resume();
        } else if (state == GAME_STATE_WAIT_INPUT_FROM) {
            from = *x;
            lcd->set_edit_target(LCD_EDIT_TARGET_FROM);
            lcd->set_edit_value(from);

            state = GAME_STATE_WAIT_INPUT_TO;
        } else if (state == GAME_STATE_WAIT_INPUT_TO) {
            to = *x;
            lcd->set_edit_target(LCD_EDIT_TARGET_TO);
            lcd->set_edit_value(to);

            /* Send current coords to Serial*/
            Serial.write(MESSAGE_OUT_COORDS);
            Serial.write(from.x);
            Serial.write(from.y);
            Serial.write(to.x);
            Serial.write(to.y);
            state = GAME_STATE_NONE;
            timer->pause();

            goto SKIP_CANCELBTN;
        } else if (state == GAME_STATE_DONE) {
            delay(1000);
            Serial.write(MESSAGE_OUT_RESET_GAME);
        }
    }

    if (cancelbtn->is_clicked()) {
        if (state == GAME_STATE_WAIT_INPUT_TO) {
            lcd->reset_edit_target(LCD_EDIT_TARGET_FROM);
            state = GAME_STATE_WAIT_INPUT_FROM;
        } else if (state == GAME_STATE_DONE) {
            delay(1000);
            Serial.write(MESSAGE_OUT_RESET_GAME);
        }
    }

SKIP_CANCELBTN:
    if (x != nullptr) delete x;

    /* Phase 3: Update LCD and button */
    lcd->update();
    cancelbtn->update();

    delay(10);
}
