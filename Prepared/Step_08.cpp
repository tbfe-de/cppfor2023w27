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

// Step 8
// TBD: now build the clock based on the class `DownCounter` shown
// below; first review the code to answer the following questions:
//  - with how many arguments can the constructor be called?
//  - how can the current value of the `DownCounter` be set?
//  - what is the effective range of values it can assume?
//  - how to test whether is still counting down or at zero?
//
class DownCounter {
    int value_{};
    const int reset_{};
    DownCounter* next_{};
public:
    DownCounter() =default;
    DownCounter(int reset, DownCounter* next = nullptr)
        : reset_{reset}, next_{next}
    {/*empty*/}
    int get() const { return value_; }
    void set(int);
    bool is_counting() const;
    void step();
};

void DownCounter::set(int value) {
    value_ = (value >= reset_)
                ? reset_-1
                : value;
}

bool DownCounter::is_counting() const {
     return (value_ != 0)
         || (next_ && next_->is_counting());
}

void DownCounter::step() {
    if (value_ > 0)
        --value_;
    else {
        if (next_ && next_->is_counting()) {
            next_->step();
            value_ = reset_-1;
        }
    }
}

#if 0

int main()
{
    // optional step
    // TBD: implement some tests for the `DownCounter`, as well
    // for its "stand-alone" use, (ie. not being part of a counter
    // chain) and also in a chain of two and chain of three.
}

#else

class Clock {
private:
    DownCounter minutes_{1000};
    DownCounter seconds_{60, &minutes_};
    DownCounter tenthsecs_{10, &seconds_};
public:
    Clock() =default; // no arguments C'tor (aka Default C'tor)
    Clock(const Clock&)            =delete; // Copy-C'tor
    Clock(Clock&&)                 =delete; // Move-C'tor
    Clock& operator=(const Clock&) =delete; // Copy-Assign
    Clock& operator=(Clock&&)      =delete; // Move-Assign
    ~Clock() =default;

    void set(int);
    operator bool() const;
    Clock& operator--();
    bool operator-=(int);
    void show(std::ostream& = std::cout) const;
};

void Clock::set(int ts) {
    tenthsecs_.set(ts % 10); ts /= 10;
    seconds_.set(ts % 60); ts /= 60;
    minutes_.set(ts);
}

Clock::operator bool() const {
        return tenthsecs_.is_counting();
    }

Clock& Clock::operator--() {
    tenthsecs_.step();
    return *this;
}

void Clock::show(std::ostream& os) const {
    auto const saved_fill{os.fill('0')};
    // Step 8 continued
    // TBD: complete the output using the correct width and
    // separators for/between minutes, seconds, and 1/10-th
    // seconds
    os << "????????";
    os.fill(saved_fill);
}

std::ostream& operator<<(std::ostream& lhs, const Clock& rhs) {
    rhs.show(lhs);
    return lhs;
}

bool Clock::operator-=(int steps) {
    while (steps > 0) {
        if (!this->operator bool())
            return false;
        --*this;
        --steps;
    }
    return true;
}

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
                    switch (theGameState) {
                    case GameState::BlackDraw:
                        blackPlayerClock -= ticksToSimulate;
                        if (!blackPlayerClock)
                            theGameState = GameState::WhiteWins;
                        break;
                    case GameState::WhiteDraw:
                        whitePlayerClock -= ticksToSimulate;
                        if (!whitePlayerClock)
                            theGameState = GameState::BlackWins;
                        break;
                    default:
                        continue;
                    }
                    break;
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
                case '.':
                    std::cout << "Thanks for using the Chess-Clock" << std::endl;
                    return;
            }
            using std::setfill;
            using std::setw;
            std::cout << "B:" << setfill(' ') << setw(8) << blackPlayerClock
                      << ((theGameState == GameState::BlackDraw) ? "*" : " ")
                      << "| "
                      << "W:" << setfill(' ') << setw(8) << whitePlayerClock
                      << ((theGameState == GameState::WhiteDraw) ? "*" : " ")
                      << std::endl;
            switch (theGameState) {
            case GameState::BlackWins:
                std::cout << "!! Black Player Won !!" << std::endl;
                break;
            case GameState::WhiteWins:
                std::cout << "!! White Player Won !!" << std::endl;
                break;
            }
        }
    }
}

int main()
{
    runChessClock();
}

#endif