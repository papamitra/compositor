
#include "compositor.hpp"

#include <cassert>

int main(void) {

    wl_display* disp = wl_display_create();

    yawc::compositor::global_create(disp);

    const int stat = wl_display_add_socket(disp, "wayland-0");
    assert(stat == 0);

    wl_display_run(disp);

    return 0;
}

