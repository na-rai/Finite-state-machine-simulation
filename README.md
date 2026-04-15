# Finite-state-machine-simulation

Project Title: Cache FSM Simulator (Write-Back/Write-Allocate)
# Overview

This project is a cycle-accurate behavioral simulation of a Direct-Mapped Cache controller. It implements a Finite State Machine (FSM) that manages data movement between the CPU and Main Memory, handling hits, misses, and dirty-block evictions.

# How to Build and Run

This simulation is written in standard C++ and has no external dependencies.

    Prerequisites: A C++ compiler (GCC, Clang, or MSVC).

    # Compilation:
    Bash

    g++ -o cache_sim main.cpp

    # Execution:
    Bash

    ./cache_sim
    
