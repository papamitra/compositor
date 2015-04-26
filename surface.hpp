#ifndef SURFACE_H
#define SURFACE_H

#include <wayland-server.h>

namespace yawc {

class surface {
    wl_resource* resource_;
public:
    surface(wl_client* client, int ver, uint32_t id);
    ~surface();
};

} // namespace yawc


#endif // SURFACE_H
