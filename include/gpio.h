#pragma once

#include "pico/stdlib.h"

namespace util {
    class GPIO {
    private:
        const uint pin;
        bool out_mode;
    
    public:
        GPIO(uint pin, bool out);
        GPIO(uint pin, bool out, bool pull_up, bool pull_down);
        
        void reinit(bool out);
        void set_pull(bool up, bool down);

        bool operator=(bool val);
        operator bool() const;
    };
}
