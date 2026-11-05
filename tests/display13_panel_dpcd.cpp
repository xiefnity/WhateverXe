#include "kern_display13_panel.hpp"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main() {
    const auto &p=Display13Panel::extendedCapsPatch;
    assert(p.size==10 && p.offset==0x1DF86);
    // Only the js on the extended block's own bit changes; the base-bit test is before this span.
    for(unsigned n=0;n<p.size;++n)assert((p.before[n]!=p.after[n])==(n==4 || n==5));
    assert(p.before[4]==0x78 && p.before[5]==0x04 && p.after[4]==0x90 && p.after[5]==0x90);
    // The span must not overlap the existing LightUpEDP corrections.
    for(const auto &l:Display13Panel::lightPatches)assert(l.offset>=p.offset+p.size);
    puts("PASS Display13 extended DPCD patch removes only the extended-bit branch");
}
