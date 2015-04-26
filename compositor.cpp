
#include "compositor.hpp"
#include "surface.hpp"

#include <cassert>
#include <cstdio>
#include <iostream>

#include <unistd.h>

namespace yawc {

namespace {

void handle_create_surface(
    struct wl_client *client,
    struct wl_resource *resource, uint32_t id)
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;

    compositor * const comp = static_cast<compositor*>(wl_resource_get_user_data(resource));

    const int ver = wl_resource_get_version(resource);

    try {
        surface* surf = new surface{client, ver, id};
    } catch (std::bad_alloc&) {
        wl_resource_post_no_memory(resource);
    }

}

static const struct wl_compositor_interface compositor_interface = {
    handle_create_surface,
    NULL
};

void handle_bind(struct wl_client *client, void *data,
                 uint32_t ver, uint32_t id)
{
    printf("compositor_bind\n");

    wl_resource* const res = wl_resource_create(client, &wl_compositor_interface, ver, id);
    if (!res) {
        wl_client_post_no_memory(client);
        assert(0 && "Out of memory");
    }

    wl_resource_set_implementation(res, &compositor_interface, data, NULL);
}

}

void compositor::global_create(wl_display* disp) {
    static compositor comp{disp};

    wl_global* const g = wl_global_create(disp, &wl_compositor_interface,
                                          1, &comp, handle_bind);
    assert(g);
}

} // namespace yawc
