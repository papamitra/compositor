
#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <wayland-server.h>
#include <wayland-server-protocol.h>

#include <cassert>
#include <cstdio>
#include <iostream>

#include <unistd.h>

namespace yawc{

class compositor {
    wl_display* display_;

    compositor(wl_display* disp): display_(disp) {}
    
public:

    static void initialize(wl_display* disp);

};

} //namespace yawc

#endif // COMPOSITOR_H
