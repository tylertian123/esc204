#pragma once

#include "pico/stdlib.h"
#include "pi-pico-LCD/lcd_display.hpp"

#include "gpio.h"

namespace hw {
    class Button {
    private:
        static constexpr unsigned int DEBOUNCE_DELAY = 30;
        static constexpr unsigned int HOLD_DELAY = 1000;
        
        GPIO pin;
        uint32_t last_change;
        // Whether the button has previously been held down
        // This is to prevent a "pressed" when the button is being released from being held down
        bool in_hold = false;

    public:
        /// @brief Whether the button is currently down
        bool down = false;
        /// @brief Set to true once when the button has been pressed and released; needs to be cleared
        bool pressed = false;
        /// @brief Set to true once after the button has been held down (but before release); needs to be cleared
        bool held = false;
        /// @brief Set to true once when the state (up/down) is changed; needs to be cleared
        bool changed = false;
        /// @brief The duration that the button has been held down. Updated automatically; no need to clear.
        uint32_t held_duration = 0;

        explicit Button(uint pin);

        /// @brief Update the state of the button
        /// @param t Current number of milliseconds since boot; will be computed if omitted
        void poll(uint32_t t = 0);
        /// @brief Clear all flags (pressed and held).
        void clear_all();
        /// @brief Equivalent to down.
        operator bool() const;
    };

    class LED {
    private:
        GPIO pin;
        uint32_t last_change;
        // Zero when not in blink mode
        uint16_t blink_duration = 0;
        // Note this counts the number of state changes, not actual blink cycles
        // 0xFFFF when blinking indefinitely
        uint16_t blink_counter = 0;
    
    public:
        /// @brief Pass this to blink() to have the LED blink indefinitely.
        static constexpr uint16_t INDEFINITE = 0xFFFF;

        /// @brief Constructor.
        /// @param pin The pin the LED is on (the LED should be active high)
        /// @param state The initial state of the LED; true if on
        LED(uint pin, bool state = false);
        /// @brief Set the state of the LED. If the LED is blinking, the blinking will be stopped.
        /// @param state The state of the LED; if true, the LED will be on; if false, the LED will be off
        void set(bool state);
        /// @brief Return whether the LED is currently blinking.
        /// @return True if the LED is blinking
        bool blinking() const;
        /// @brief Make the LED blink (asynchronously).
        /// @param duration Duration in milliseconds between switching of on/off
        /// @param times The number of times to blink; INDEFINITE to blink forever
        void blink(uint16_t duration, uint16_t times = INDEFINITE);
        /// @brief Update the state of the LED.
        /// @param t Current number of milliseconds since boot; will be computed if omitted
        void poll(uint32_t t = 0);

        /// @brief Equivalent to set().
        /// @param state The new state of the LED
        /// @return The new state
        bool operator=(bool state);
    };

    class BufferedLCD {
    private:
        char screen[2][16];
        char buf[2][16];

        LCDdisplay &disp;
    public:
        explicit BufferedLCD(LCDdisplay &disp);
        
        /// @brief Clear both the screen and the buffer.
        void clear();
        /// @brief Clear the buffer only.
        void clear_buf();
        /// @brief Write text to the buffer at the specified position.
        /// @param str The text string; if this exceeds the length of the line, it will be cut off, not wrapped
        /// @param col Column (x-coord) to start writing
        /// @param row Row (y-coord) to start writing
        void print(const char *str, uint8_t col = 0, uint8_t row = 0);
        /// @brief Update the screen to match the buffer. Only the different characters will be updated.
        /// @return Whether an update actually occurred (false if no-op).
        bool update();
    };
}
