/* This file is part of the "ST" project.
 * You may use, distribute or modify this code under the terms
 * of the GNU General Public License version 2.
 * See LICENCE.txt in the root directory of the project.
 *
 * Author: Maxim Atanasov
 * E-mail: maxim.atanasov@protonmail.com
 */

#ifndef TIMER_DEF
#define TIMER_DEF

#include <chrono>

///A timer used for keeping track of time in the main loop.
/**
 * Uses a std::chrono::high_resolution_clock internally to provide quite accurate timekeeping.
 */
class timer{
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> start;
    public:
        timer();
        double time_since_start() const;
};

//INLINED METHODS

/**
 *
 * @return The elapsed time since the initialization of the timer.
 */
inline double timer::time_since_start() const{
    std::chrono::duration<double> elapsed_seconds = std::chrono::high_resolution_clock::now() - start;
    return elapsed_seconds.count() * 1000;
}

#endif
