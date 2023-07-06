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

// Step 9 (see also Diagram Chained_Counters.png)
// TBD: change the design to use two classes:
//  - `BaseDownCounter` and
//  - `ChainableDownCounter` (which is derived from the first);
// `BaseDownCounter` can be a reduced form of the `DownCounter`
// below with the pointer `next_` removed; its constructor and
// the implementation of some of its member functions will become
// simpler as stepping up the next_ counter is only done in the
// derived class;
// to simplify this simpler `BaseDownCounter` should make its data
// members - that have been private in `DownCounter` - protected
// in `BaseDownCounter` and the member functions `is_counting()`
// and `step()` virtual.
// (TBD continues below)

class DownCounter {
    int value_{};
    const int reset_{};
    DownCounter* next_{};
public:
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

// TBD (continued)
// the derived class `ChainableDownCounter` shall publicly inherit
// from `BaseDownCounter`, receiving all its member data and
// functions;
// IT ALSO ADDS THE CONNECTION TO THE NEXT STAGE AS REFERENCE:
//  class ChainableDownCounter : public BaseDownCounter {
//      ...
//      BaseCounter& next_;
//  public:
//      ...
//  };
// therefore its CONSTRUCTOR needs to be adapted to receive the
// next counter in the chain via reference;
// `BaseDownCounter::step` and `BaseDownCounter::is_counting` now
// must be overridden with an extended version that does what the
// base class does and ALSO steps the `BaseDownCounter` connected
// via `next_` when necessary.
// NOTE: in this design there is ALWAYS a valid next counter as
// the LAST counter in the chain will be a `BaseCounter`.

#if 1

#include <cassert>

void test_single_stage() {
    DownCounter c0{5};
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
    DownCounter c0{5};
    DownCounter c1{2, &c0};
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
    DownCounter c0{2};      c0.set(1);  assert(c0.get() == 1);
    DownCounter c1{2, &c0}; c1.set(1);  assert(c1.get() == 1);
    DownCounter c2{2, &c1}; c2.set(1);  assert(c2.get() == 1);
    c2.step();                          assert(c0.get() == 1);
                                        assert(c1.get() == 1);
                                        assert(c2.get() == 0);
    c2.step();                          assert(c0.get() == 1);
                                        assert(c1.get() == 0);
                                        assert(c2.get() == 1);
    c2.step();                          assert(c0.get() == 1);
                                        assert(c1.get() == 0);
                                        assert(c2.get() == 0);
    c2.step();                          assert(c0.get() == 0);
                                        assert(c1.get() == 1);
                                        assert(c2.get() == 1);
    c2.step();                          assert(c0.get() == 0);
                                        assert(c1.get() == 1);
                                        assert(c2.get() == 0);
    c2.step();                          assert(c0.get() == 0);
                                        assert(c1.get() == 0);
                                        assert(c2.get() == 1);
    c2.step();                          assert(c0.get() == 0);
                                        assert(c1.get() == 0);
                                        assert(c2.get() == 0);
    c2.step();                          assert(c0.get() == 0);
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
                case 'c': // continue game
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
