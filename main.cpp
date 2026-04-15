#include <iostream>
#include <string>
using namespace std;

// ── constants ──────────────────────────────────────
const int CACHE_SIZE    = 4;   // 4 cache blocks
const int MEM_CYCLES    = 2;   // memory takes 2 cycles

// ── FSM states ─────────────────────────────────────
enum State { IDLE, COMPARE_TAG, WRITE_BACK, ALLOCATE };

string stateName(State s){
    if(s==IDLE)        return "IDLE";
    if(s==COMPARE_TAG) return "COMPARE_TAG";
    if(s==WRITE_BACK)  return "WRITE_BACK";
    if(s==ALLOCATE)    return "ALLOCATE";
    return "?";
}

// ── one cache block ────────────────────────────────
struct Block {
    bool valid = false;
    bool dirty = false;
    int  tag   = -1;
    int  data  =  0;
};

// ── one CPU request ────────────────────────────────
struct Request {
    bool write;   // true=write, false=read
    int  addr;
    int  data;    // only used for writes
};

// ── the cache + FSM ────────────────────────────────
Block cache[CACHE_SIZE];
State state = IDLE;
int   memTimer = 0;      // counts down memory delay
Request current;         // request being handled
bool    busy = false;    // are we handling a request?

// address helpers (2-bit index, rest is tag)
int idx(int addr){ return addr % CACHE_SIZE; }
int tag(int addr){ return addr / CACHE_SIZE; }

// ── print current cache ────────────────────────────
void printCache(){
    cout << "\n  Cache:\n";
    cout << "  [idx] valid dirty tag  data\n";
    for(int i=0;i<CACHE_SIZE;i++){
        cout << "  [ " << i << " ]   "
             << cache[i].valid << "     "
             << cache[i].dirty << "    "
             << (cache[i].valid ? to_string(cache[i].tag) : "--")
             << "   "
             << (cache[i].valid ? to_string(cache[i].data) : "--")
             << "\n";
    }
    cout << "\n";
}

// ── simulate one clock tick ────────────────────────
void tick(){
    cout << "-- State: " << stateName(state) << " --\n";

    // count down memory if busy
    if(memTimer > 0){
        memTimer--;
        cout << "   memory busy, " << memTimer << " cycles left\n";
        if(memTimer == 0)
            cout << "   memory READY\n";
        return;   // wait for memory
    }

    switch(state){

    case IDLE:
        if(busy){
            cout << "   picked up request: "
                 << (current.write?"WRITE":"READ")
                 << " addr=" << current.addr << "\n";
            state = COMPARE_TAG;
        } else {
            cout << "   nothing to do\n";
        }
        break;

    case COMPARE_TAG: {
        int i = idx(current.addr);
        int t = tag(current.addr);
        Block& b = cache[i];

        bool hit = b.valid && b.tag == t;
        cout << "   index=" << i << " tag=" << t
             << "  =>  " << (hit?"HIT":"MISS") << "\n";

        if(hit){
            if(!current.write){
                cout << "   READ  data=" << b.data << "\n";
            } else {
                b.data  = current.data;
                b.dirty = true;
                cout << "   WRITE data=" << b.data
                     << " (dirty=1)\n";
            }
            cout << "   cache_ready=1  =>  IDLE\n";
            busy  = false;
            state = IDLE;
        } else {
            b.tag = t;   // prepare slot
            if(b.dirty && b.valid){
                cout << "   old block dirty  =>  WRITE_BACK\n";
                state = WRITE_BACK;
            } else {
                cout << "   old block clean  =>  ALLOCATE\n";
                state = ALLOCATE;
            }
        }
        break;
    }

    case WRITE_BACK: {
        int i = idx(current.addr);
        cout << "   writing dirty block to memory...\n";
        cache[i].dirty = false;
        memTimer = MEM_CYCLES;
        state    = ALLOCATE;   // after write, fetch new block
        break;
    }

    case ALLOCATE: {
        int i = idx(current.addr);
        cout << "   fetching block from memory...\n";
        // simulate memory giving us data
        cache[i].data  = 100 + current.addr; // fake memory value
        cache[i].valid = true;
        cache[i].dirty = false;
        memTimer = MEM_CYCLES;
        state    = COMPARE_TAG;  // retry — will hit now
        break;
    }

    }
}

// ── run one request until done ─────────────────────
void runRequest(bool write, int addr, int data=0){
    cout << "\n============================\n";
    cout << "REQUEST: " << (write?"WRITE":"READ")
         << " addr=" << addr;
    if(write) cout << " data=" << data;
    cout << "\n============================\n";

    current = {write, addr, data};
    busy    = true;

    // keep ticking until request is done
    for(int i=0; i<20; i++){
        tick();
        if(!busy) break;
    }

    printCache();
}

// ── main ───────────────────────────────────────────
int main(){
    cout << "=== Cache FSM Simulator ===\n";
    cout << "Cache size: " << CACHE_SIZE << " blocks\n\n";

    // Test 1: read miss (cold cache)
    runRequest(false, 0);

    // Test 2: read hit (same address)
    runRequest(false, 0);

    // Test 3: write hit
    runRequest(true, 0, 42);

    // Test 4: conflict — addr 4 maps to same slot as addr 0
    // addr 0 is dirty, so write-back happens first
    runRequest(false, 4);

    // Test 5: write miss (write allocate)
    runRequest(true, 8, 99);

    return 0;
}