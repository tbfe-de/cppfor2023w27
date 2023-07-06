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
            switch(command) {
                // Step 3
                // TBD: as the first statement of each case add a
                // continue if the command is not valid in that
                // state; eg. `c` is solely valid in one of the
                // PAUSED states and should do nothing at all in
                // the other states; at the end of each case as
                // far as necessary assign a new value to the
                // variable holding the `GameState`; in between
                // at the actions to be done when the command is
                // received in that particular state
                case 'r':
                    // (TBD see above in Step 3)
                    break;
                case 's': // start clock (white draws first)
                    // (TBD see above in Step 3)
                    break;
                case 'p': // pause game
                    // (TBD see above in Step 3)
                    break;
                case 'c': // continue game
                    // (TBD see above in Step 3)
                    break;
                case 'x': // toggle player (white <--> black)
                    // (TBD see above in Step 3)
                    break;
                // the following commands are used to advance the
                // clock of the active player (if any) as a
                // deterministic way to test correct behavior
                // before the clocks player clocks are run in
                // a separate thread;
                // NOTE: for the moment please leave this section
                // as it is, its functionality will be implemented
                // later
                case '9': // advance by 108000 ticks (= 3 hours)
                case '8': // advance by 36000 ticks (= 1 hour)
                case '7': // advance by 18000 ticks (= 30 minutes )
                case '6': // advance by 3000 ticks (= 5 minutes)
                case '5': // advance by 600 ticks (= 1 minute)
                case '4': // advance by 150 ticks (= 15 seconds)
                case '3': // advance by 50 ticks (= 5 seconds)
                case '2': // advance by 10 ticks (= 1 second)
                case '1': // advance by 1 tick (= 1/10-th of a second)
                case '0': // do not advance any player clock, ie.
                          // show the same value as before with
                          // the next prompt
                    break;
                case '?': // command help (available in any state)
                    // TBD (Step 3 continued): print a brief table
                    // with the command letters and a one line
                    // explanation what they are meant to do
                    break;
                case '.': // end command loop program
                    // TBD (Step 3 continued): print a brief table
                    return;
            }
        }
    }
}

int main()
{
    runChessClock();
}
