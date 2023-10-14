#ifndef CHESS_CONTROLLER_BUTTON
#define CHESS_CONTROLLER_BUTTON

#include <Arduino.h>

class Button {
   public:
    Button(int pin);

    /**
     * This returns true only if the button was clicked and was not clicked
     * recently.
     */
    bool is_clicked();

    /**
     * This should be called every loop.
     */
    void update();

   private:
    int pin;

    bool last_state;
    bool is_new;
};

#endif