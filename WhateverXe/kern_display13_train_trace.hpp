// Bounded observation of the Display13 eDP training path. Records requests and
// results at existing route boundaries; it makes no policy or hardware decision.
#ifndef KERN_DISPLAY13_TRAIN_TRACE_HPP
#define KERN_DISPLAY13_TRAIN_TRACE_HPP
#include <stdint.h>
namespace Display13TrainTrace {
enum Kind : uint32_t {AUX=1,Signal=2,Pattern=3,Enable=4,Begin=5,Regs=6,End=7,Step=8};
struct Record {uint64_t ns,thread;uint32_t word[16];};
static_assert(sizeof(Record)==80,"Display13 training trace schema");
constexpr uint32_t Schema=2,FirstCapacity=2048,LastCapacity=512,OutsideLimit=256;
// Capability, link and status DPCD ranges only. EDID, HDCP and sideband are excluded.
inline bool interesting(uint32_t address) {
    return address<=0x220 || address==0x600 || (address>=0x700 && address<=0x7FF) ||
        (address>=0x2200 && address<=0x22FF) || (address>=0x2000 && address<=0x2010);
}
// Experimental ladder: each attempt adds one step the working Linux path performs.
// A rung that trains is held, so a later modeset does not move past what worked.
enum Rung : uint32_t {SinkState=1,RearmPattern=2,AssrOff=3,PowerStateCycle=4,PanelPowerCycle=5};
inline uint32_t rung(uint32_t attempt,uint32_t held) {
    if(held)return held;
    return attempt<1?uint32_t(SinkState):attempt>PanelPowerCycle?uint32_t(PanelPowerCycle):attempt;
}
struct Ring {
    Record first[FirstCapacity];
    Record last[LastCapacity];
    uint32_t count,stored,outside,dropped;
    // Polling outside a training attempt must not consume the first-attempt history.
    bool push(const Record &input,bool inside) {
        if(!inside) {
            if(outside>=OutsideLimit){++dropped;return false;}
            ++outside;
        }
        auto r=input;r.word[0]=++count;
        if(stored<FirstCapacity)first[stored++]=r;
        last[(count-1)%LastCapacity]=r;
        return true;
    }
};
}
#endif
