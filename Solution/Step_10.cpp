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

class BaseDownCounter {
private:
    int value_{};
    const int reset_{};
    virtual bool chained_is_counting() const { return false; }
    virtual void chained_needs_step() {}
public:
    BaseDownCounter(int reset)
        : reset_{reset}
    {/*empty*/}
    int get() const { return value_; }
    void set(int);
    bool is_counting() const;
    void step();
};

void BaseDownCounter::set(int value) {
    value_ = (value >= reset_)
                ? reset_-1
                : value;
}

bool BaseDownCounter::is_counting() const {
     return (value_ != 0)
         || chained_is_counting();
}

void BaseDownCounter::step() {
    if (value_ > 0)
        --value_;
    else {
        if (chained_is_counting()) {
            value_ = reset_-1;
            chained_needs_step();
        }
    }
}

class ChainableDownCounter : public BaseDownCounter {
    BaseDownCounter& next_;
public:
    ChainableDownCounter(int limit, BaseDownCounter& next)
        : BaseDownCounter{limit}, next_{next}
    {/*empty*/}
    void chained_needs_step() override {
        next_.step();
    }
    bool chained_is_counting() const override {
        return next_.is_counting();
    }
};


#if 1

#include <cassert>

void test_single_stage() {
    BaseDownCounter c0{5};
    assert(c0.get() == 0); assert(c0.is_counting() == false);
    c0.set(3);
    assert(c0.get() == 3); assert(c0.is_counting() == true);
    c0.step();
    assert(c0.get() == 2); assert(c0.is_counting() == true);
    c0.step();
    assert(c0.get() == 1); assert(c0.is_counting() == true);
    c0.step();
    assert(c0.get() == 0); assert(c0.is_counting() == false);
    c0.step();
    assert(c0.get() == 0); assert(c0.is_counting() == false);
    c0.set(7);
    assert(c0.get() == 4); assert(c0.is_counting() == true);
}

void test_double_stage() {
    BaseDownCounter c0{5};
    ChainableDownCounter c1{2, c0};
    c0.set(3);      assert(c0.get() == 3);
                    assert(c1.get() == 0);
    c1.step();      assert(c0.get() == 2);
                    assert(c1.get() == 1);
    c1.step();      assert(c0.get() == 2);
                    assert(c1.get() == 0);
    c1.step();      assert(c0.get() == 1);
                    assert(c1.get() == 1);
    c1.step();      assert(c0.get() == 1);
                    assert(c1.get() == 0);
    c1.step();      assert(c0.get() == 0);
                    assert(c1.get() == 1);
    c1.step();      assert(c0.get() == 0);
                    assert(c1.get() == 0);
    c1.step();      assert(c0.get() == 0);
                    assert(c1.get() == 0);
}

void test_triple_stage() {
    BaseDownCounter c0{2};          c0.set(1); assert(c0.get() == 1);
    ChainableDownCounter c1{2, c0}; c1.set(1); assert(c1.get() == 1);
    ChainableDownCounter c2{2, c1}; c2.set(1); assert(c2.get() == 1);
    c2.step();                                 assert(c0.get() == 1);
                                               assert(c1.get() == 1);
                                               assert(c2.get() == 0);
    c2.step();                                 assert(c0.get() == 1);
                                               assert(c1.get() == 0);
                                               assert(c2.get() == 1);
    c2.step();                                 assert(c0.get() == 1);
                                               assert(c1.get() == 0);
                                               assert(c2.get() == 0);
    c2.step();                                 assert(c0.get() == 0);
                                               assert(c1.get() == 1);
                                               assert(c2.get() == 1);
    c2.step();                                 assert(c0.get() == 0);
                                               assert(c1.get() == 1);
                                               assert(c2.get() == 0);
    c2.step();                                 assert(c0.get() == 0);
                                               assert(c1.get() == 0);
                                               assert(c2.get() == 1);
    c2.step();                                 assert(c0.get() == 0);
                                               assert(c1.get() == 0);
                                               assert(c2.get() == 0);
    c2.step();                                 assert(c0.get() == 0);
                                               assert(c1.get() == 0);
                                               assert(c2.get() == 0);
}

int main()
{
    test_single_stage();
    test_double_stage();
    test_triple_stage();
    std::cout << "*** ALL TESTS PASSED ***" << std::endl;
}

#else

class Clock {
private:
    BaseDownCounter minutes_{1000};
    ChainableDownCounter seconds_{60, minutes_};
    ChainableDownCounter tenthsecs_{10, seconds_};
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
    auto const saved_fill{os.fill()};
    // Step 8 continued
    // TBD: complete the output using the correct width and seperators
    // for/between minutes, seconds, and 1/10-th seconds
    using std::setw;
    using std::setfill;
    os << setfill(' ') << setw(3) << minutes_.get() << ':'
       << setfill('0') << setw(2) << seconds_.get() << '.'
                       << setw(1) << tenthsecs_.get();
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
                case '9': ticksToSimulate||(ticksToSimulate = 108'000); // (3 hours)
                case '8': ticksToSimulate||(ticksToSimulate = 36'000); // (1 hour)
                case '7': ticksToSimulate||(ticksToSimulate = 18'000); // (30 minutes)
                case '6': ticksToSimulate||(ticksToSimulate = 3'000); // (5 minutes)
                case '5': ticksToSimulate||(ticksToSimulate = 600); // (1 minute)
                case '4': ticksToSimulate||(ticksToSimulate = 150); // (15 seconds)
                case '3': ticksToSimulate||(ticksToSimulate = 50); // (5 seconds)
                case '2': ticksToSimulate||(ticksToSimulate = 10); // (1 second)
                case '1': ticksToSimulate||(ticksToSimulate = 1); // (0.1 second)
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
            std::cout << "B:" << blackPlayerClock
                      << ((theGameState == GameState::BlackDraw) ? "*" : " ")
                      << "| "
                      << "W:" << whitePlayerClock
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
