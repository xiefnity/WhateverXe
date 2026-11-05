#include "kern_display13_train_trace.hpp"
#include <assert.h>
#include <stdio.h>
using namespace Display13TrainTrace;
static Ring ring;
int main() {
    assert(interesting(0) && interesting(0x202) && interesting(0x220) && interesting(0x600));
    assert(interesting(0x700) && interesting(0x2210) && interesting(0x2008));
    assert(!interesting(0x221) && !interesting(0x68000) && !interesting(0x1000));
    Record r {};
    for(uint32_t n=0;n<OutsideLimit;++n)assert(ring.push(r,false));
    assert(!ring.push(r,false) && ring.dropped==1 && ring.count==OutsideLimit);
    for(uint32_t n=0;n<FirstCapacity+LastCapacity;++n) {
        r.word[1]=n;assert(ring.push(r,true));
    }
    assert(ring.stored==FirstCapacity);
    assert(ring.first[0].word[0]==1 && ring.first[FirstCapacity-1].word[0]==FirstCapacity);
    const auto newest=ring.count;
    assert(ring.last[(newest-1)%LastCapacity].word[0]==newest);
    assert(ring.last[(newest-1)%LastCapacity].word[1]==FirstCapacity+LastCapacity-1);
    assert(ring.last[newest%LastCapacity].word[0]==newest-LastCapacity+1);
    puts("PASS Display13 training trace ring keeps first attempts, bounds outside polling, wraps last");
}
