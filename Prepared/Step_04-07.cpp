#include <cctype>   // std::islower
                    // std::isdigit
                    // std::tolower
#include <iomanip>  // std::width
                    // std::setfill
#include <iostream> // std::cout
                    // std::endl
#include <string>   // std::string
                    // std::to_string

constexpr int InitialTime{30*60*10};
class Clock {
private:
    int tenthSeconds_{0};
public:
    Clock() =default; // no arguments C'tor (aka Default C'tor)
    Clock(const Clock&)            =delete; // Copy-C'tor
    Clock(Clock&&)                 =delete; // Move-C'tor
    Clock& operator=(const Clock&) =delete; // Copy-Assign
    Clock& operator=(Clock&&)      =delete; // Move-Assign
    ~Clock() =default;

    auto set(int ts) {
        tenthSeconds_ = ts;
    }
    auto get() const {
        return tenthSeconds_;
    }
    void show(std::ostream& = std::cout) const;
};

void Clock::show(std::ostream& os) const {
    auto ts = tenthSeconds_;
    const auto t{ts % 10}; ts /= 10;
    const auto s{ts % 60}; ts /= 60;
    std::string result{};
    result += std::to_string(ts);
    result += ':';
    if (s < 10) result += '0';
    result += std::to_string(s);
    result += '.';
    result += std::to_string(t);
    os << result;
}

std::ostream& operator<<(std::ostream& lhs, const Clock& rhs) {
    rhs.show(lhs);
    return lhs;
}

// Step 4
// TBD: review the helper function `countDownClock` below how it
// (a) decrements the clock
// (b) checks whether the clock is at zero
// then provide two new operators as member functions of `Clock`
// to do the decrement as
//  Clock& Clock::operator--() {
//      … decrement `tenthSeconds_` but NOT below zero
//      return *this; // <--- typically expected return value
//  }
// and the test whether further decrements are possible as
//  Clock::operator bool() const {
//      … `true` if further decrement is possible
//  }
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// NOTE: if you decide to put the implementation of any or both
// operator functions HERE (outside the class) you still need to
// add the respective prototype ABOVE (inside the class), like
// that:
//  class Clock {
//      ...
//  public:
//      ...
//      Clock& operator--();    // <--- prototype to be added IF
//                              // implemented outside the class
//      operator bool() const;  // <--- prototype to be added IF
//                              // implemented outside the class
//      ...
//  };
//
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// What remains for BELOW is to use these two (operator) functions
// just added in the implementation of `countClockDown` below AND
// ALSO below in the two functions for testing the Clock below
// (ie. `test_1` and `test_2`).
//
bool countClockDown(Clock& clk, int steps = 1) {
    while (steps > 0) {
        if (clk.get() == 0)
            return false;
        clk.set(clk.get() - 1);
        --steps;
    }
    return true;
}
// Step 5
// TBD: add the keyword `explicit` to the prototype (definition)
// of `operator bool()` inside class `Clock` and "repair" the
// consequences (= do what is necessary to make the code of
// `Clock` and its tests below compile again).

#if 1

void test_1(Clock& c) {
    std::cout << __func__ << ": "
        "run the countdown in the helper function\n";
    countClockDown(c, 20);
    std::cout << c << std::endl;
    std::cout << "not yet expired: " << (c.get() > 0) << std::endl;
}

void test_2(Clock& c) {
    std::cout << __func__ << ": "
        "run the countdown loop explicitly\n";
    while (c.get() > 0) {
        countClockDown(c);
        std::cout << c << std::endl;
    }
    std::cout << "not yet expired: " << (c.get() > 0) << std::endl;
}
//
// Optional Step
// TBD: in the two test functions above replace the use of
// `__func__` with `__PRETTY_FUNCTION__` and note the influence
// on the output (if you want to be extra diligent try to find out
// (eg. with the help of https://cppreference.com) whether either
// or both of the two predefined macros are part of a C- or
// C++-standard.

// Step 6
// TBD: turn the the above free-standing function `countClockDown`
// into a member function and adapt the calls where it is used;
// then rename the member function to `Clock::operator-=` keeping
// everything as it was before, INCLUDING its calls using the
// dot-notation;
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// QUESTION: can you keep the default value for `steps` or do you
// have to remove it now?
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// finally rewrite the dot-notation in the calls so that they use
// the "-=" assignment form;
//
int main()
{
    std::cout.setf(std::ios::boolalpha);
    Clock theTestClock{};
    std::cout << theTestClock << std::endl;
    theTestClock.set(33);
    test_1(theTestClock);
    test_2(theTestClock);
}

#else

enum class GameState {
    Initial, Startable,
    WhitePaused, BlackPaused,
    WhiteDraw, BlackDraw,
    WhiteWins, BlackWins
};

void runChessClock()
{
    Clock blackPlayerClock{};
    Clock whitePlayerClock{};
    auto theGameState{GameState::Initial};
    char command;
    while (std::cin.get(command)) {
        command = std::tolower(command);
        if (std::islower(command)
         || std::isdigit(command)
         || (command == '?')
         || (command == '.')) {
            using std::setfill;
            using std::setw;
            std::cout << "B:" << setfill(' ') << setw(8) << blackPlayerClock
                      << ((theGameState == GameState::BlackDraw) ? "*" : " ")
                      << "| "
                      << "W:" << setfill(' ') << setw(8) << whitePlayerClock
                      << ((theGameState == GameState::WhiteDraw) ? "*" : " ")
                      << std::endl;
            std::cout << "===> " << command << std::endl;
            int ticksToSimulate{};
            switch(command) {
                case 'r':
                    if (not (theGameState == GameState::Initial
                          || theGameState == GameState::BlackWins
                          || theGameState == GameState::WhiteWins
                          || theGameState == GameState::BlackPaused
                          || theGameState == GameState::WhitePaused))
                          continue;
                    // (TBD see above in Step 3)
                    blackPlayerClock.set(InitialTime);
                    whitePlayerClock.set(InitialTime);
                    theGameState = GameState::Startable;
                    break;
                case 's': // start clock (white draws first)
                    if (not (theGameState == GameState::Startable))
                        continue;
                    theGameState = GameState::WhiteDraw;
                    break;
                case 'p':
                    if (not (theGameState == GameState::BlackDraw
                          || theGameState == GameState::WhiteDraw))
                        continue;
                    switch (theGameState) {
                    case GameState::BlackDraw:
                        theGameState = GameState::BlackPaused;
                        break;
                    case GameState::WhiteDraw:
                        theGameState = GameState::WhitePaused;
                        break;
                    }
                    break;
                case 'c': // coninue game
                    if (not (theGameState == GameState::BlackPaused
                          || theGameState == GameState::WhitePaused))
                        continue;
                    switch (theGameState) {
                    case GameState::BlackPaused:
                        theGameState = GameState::BlackDraw;
                        break;
                    case GameState::WhitePaused:
                        theGameState = GameState::WhiteDraw;
                        break;
                    }
                    break;
                        break;
                case 'x':
                    if (not (theGameState == GameState::BlackDraw
                          || theGameState == GameState::WhiteDraw))
                        continue;
                    switch (theGameState) {
                    case GameState::BlackDraw:
                        theGameState = GameState::WhiteDraw;
                        break;
                    case GameState::WhiteDraw:
                        theGameState = GameState::BlackDraw;
                        break;
                    }
                    break;
                case '9': ticksToSimulate += 72'000; [[fallthrough]]; // (3 hours)
                case '8': ticksToSimulate += 18'000; [[fallthrough]]; // (1 hour)
                case '7': ticksToSimulate += 15'000; [[fallthrough]]; // (30 minutes)
                case '6': ticksToSimulate += 2'500; [[fallthrough]]; // (5 minutes)
                case '5': ticksToSimulate += 450; [[fallthrough]]; // (1 minute)
                case '4': ticksToSimulate += 100; [[fallthrough]]; // (= 15 seconds)
                case '3': ticksToSimulate += 40; [[fallthrough]]; // (= 5 seconds)
                case '2': ticksToSimulate += 9; [[fallthrough]]; // (= 1 second)
                case '1': ticksToSimulate += 1; [[fallthrough]]; // (1/10-th of a second)
                case '0':
                    // Step 7
                    // TBD:
                    // complete the following code fragment and remove
                    // the '//'-commenting
                    switch (theGameState) {
                    // case …???…:
                    //    black… -= ticksToSimulate;
                    //    break;
                    // case …???…:
                    //    white… -= ticksToSimulate;
                    //    break;
                    default:
                        continue;
                    }
                    break;
                    // optional Step: if you get compiler warnings
                    // caused by switch statements that do not
                    // handle all cases you may silence them by
                    // adding a `default:`-label with no action;
                case '?':
                    std::cout << "*** Chess Clock Commands ***\n"
                                 "r - reset player clocks to initial time\n"
                                 "s - start the game (white draws first)\n"
                                 "p - pause the game\n"
                                 "c - continue the game\n"
                                 "--- General Commends ---\n"
                                 "? - show this list of commands\n"
                                 ". - end the chess clock program\n";
                    break;
                case '.': // say godbye and end the chess clock program
                    return;
            }
        }
    }
}

int main()
{
    runChessClock();
}

#endif
