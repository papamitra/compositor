#ifndef SURFACE_H
#define SURFACE_H

#include <wayland-server.h>

namespace yawc {

struct buffer;
struct surface {
    wl_resource* resource;
    int32_t x;
    int32_t y;
    buffer* buf;

    surface(wl_client* client, int ver, uint32_t id);
    ~surface();
};

} // namespace yawc


#endif // SURFACE_H
