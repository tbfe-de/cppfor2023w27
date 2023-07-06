# Overview of Steps and Sidelines

## Step 1

Add an operation showing the `Clock` value in a specific format.

### Sideline Step 1a

Instead of implementing a member function `Clock::show()` overload
`operator<<`.

### Sideline Step 1b

Instead of checking correctness by "looking at the output" (of
some test value) automatically verify the result by catching the
output in an `std::ostringstream` and compare with expectation
making use of the `assert` macro (as from C).

## Step 2

Add two objects of class `Clock` and output their value in the
loop before the prompt for the next command is shown.

## Step 3

Implement the basic FSM behavior processing commands dependant on
the current game state.

## Step 4

Introduce two operator functions, one to step the clock and
another one to test whether it can still count down further.

## Step 5

Make `Clock::operator bool()` explicit.

### Sideline Step 5

Explore the difference between `__func__` and
`__PRETTY_FUNCTION__` macros including their availability
depending on the C++-standard.

## Step 6

Turn a global function first into a member function, then into an
(assigning) `operator-=`.

## Step 7

Implement the (still missing single digit) commands to simulate
stepping the player clocks depending on the game state.

## Step 8

Replace the class `Clock` with a different implementation based on
chainable objects of class `DownCounter`.

## Step 9

Again replace class `Clock` with an implementation based on two
classes `BaseDownCounter` and `ChainableDownCounter`.

## Step 10

Apply the NVI-idiom (= Non Virtual Interface) to `BaseDownCounter`
and `ChainableDownCounter`.

## Step 11

Add an interface `I_DownCounting` to `BaseDownCounter`.

### Sideline Step 11

Demonstrate the gained flexibility by having other classes
implement that interface

## Step 12

Add a thread to the player clocks, using the Publisher/Subscriber
pattern to add a clock depending on the game state.

## Step 13

Modify the current design (with a `BaseDownCounter`, a
`ChainableDownCounter`, and an `I_DownCounting` interface so that the `reset_` value instead being a
member data becomes a template argument.

## Step 14

Modify the FSM implementing the basic behavior and state dependent
command processing with a table based approached.
