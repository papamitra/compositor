
#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <wayland-server.h>
#include <wayland-server-protocol.h>

#include <cassert>
#include <cstdio>
#include <iostream>

#include <unistd.h>

namespace yawc{

struct compositor {
    wl_display* display;

    compositor(wl_display* display): display(display) {}

    static void initialize(wl_display* display);

};

} //namespace yawc

#endif // COMPOSITOR_H
