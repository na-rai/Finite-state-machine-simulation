# Finite-state-machine-simulation

Project Title: Cache FSM Simulator (Write-Back/Write-Allocate)
# Overview

This project is a cycle-accurate behavioral simulation of a Direct-Mapped Cache controller. It implements a Finite State Machine (FSM) that manages data movement between the CPU and Main Memory, handling hits, misses, and dirty-block evictions.

### **How to Build and Run**

This simulation is written in standard C++ and has no external dependencies.

1. **Prerequisites:** A C++ compiler (GCC, Clang, or MSVC).

2. **Compilation:**
   ```bash
   g++ -o cache_sim main.cpp
    
3. **Execution**
   ```bash
   ./cache_sim
   
### **Testing & Simulation Flow**


The simulation executes a pre-defined sequence of memory requests to demonstrate how the FSM handles different cache scenarios. Each request shows the state transitions and the resulting state of the cache table.

#### **1. Read Miss (Cold Start)**
* **Request:** `READ addr=0`
* **Behavior:** The cache is initially empty (Valid = 0). The FSM moves from `COMPARE_TAG` to `ALLOCATE` to fetch data from Main Memory.
* **Outcome:** Data is loaded into index 0, and the valid bit is set to 1.

#### **2. Read Hit**
* **Request:** `READ addr=0`
* **Behavior:** Since the data was just loaded, the `COMPARE_TAG` state finds a match immediately.
* **Outcome:** Data is returned in a single cycle without accessing memory.

#### **3. Write Hit (Dirty Block)**
* **Request:** `WRITE addr=0, data=42`
* **Behavior:** The FSM finds the tag, updates the data, and sets the **Dirty Bit** to 1.
* **Outcome:** Memory is NOT updated yet (Write-Back policy), saving bus cycles.

#### **4. Conflict Miss & Write-Back**
* **Request:** `READ addr=4`
* **Behavior:** Both `addr=0` and `addr=4` map to `index=0`. Since the existing block is **Dirty**, the FSM must first transition to `WRITE_BACK` to save the old data to memory before it can `ALLOCATE` space for the new request.
* **Outcome:** This demonstrates the most complex path in the FSM logic.

#### **5. Write Miss (Write-Allocate)**
* **Request:** `WRITE addr=8, data=99`
* **Behavior:** The FSM misses, fetches the block from memory first (`ALLOCATE`), and then performs the write.
* **Outcome:** Ensures cache consistency even on write failures.
