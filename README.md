# Airline Reservation System

A console-based airline reservation application written in C++. Passengers can create accounts and manage travel reservations, while booking agents and administrators access role-based workflows for airline operations.

## Requirements

- GNU Make
- `g++` with C++17 support
- Linux, macOS, or a compatible Unix-like shell (such as WSL on Windows)

## Build and run

From the project root, run:

```sh
make
make run
```

The executable is created at `bin/AirlineReservationSystem`. You can also build and run it in one step with `make run`.

Run the application from the project root so it can access the JSON files in `data/`.

## Clean build files

```sh
make clean
```

This removes the generated `build/` and `bin/` directories.
