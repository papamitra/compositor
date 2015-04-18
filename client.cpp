
#include <wayland-client.h>
#include <wayland-client-protocol.h>

#include <cassert>
#include <cstdio>
#include <string>
#include <memory>

struct client {
    wl_display* display;
    wl_compositor* compositor;
    wl_proxy* object;
};

static void
registry_handle_global(void *data, struct wl_registry *registry,
                        uint32_t id, const char *intf, uint32_t ver)
{
    client* c = static_cast<client*>(data);

    printf("registry_handle_globals: %s\n", intf);
    const std::string ifname = intf;

    if (ifname == "wl_compositor") {
        c->compositor = static_cast<wl_compositor*>(
            wl_registry_bind(registry, id,
                             &wl_compositor_interface, ver));
    }
}

static const struct wl_registry_listener registry_listener = {
    registry_handle_global,
    NULL
};

int main(int argc, char *argv[])
{
    auto c = new client{};

    wl_display* const d = wl_display_connect(NULL);
    assert(d);

    wl_registry* const r = wl_display_get_registry(d);
    assert(r);

    wl_registry_add_listener(r, &registry_listener, c);
    assert(wl_display_roundtrip(d) != -1);

    wl_display_flush(d);

    wl_surface* surface = wl_compositor_create_surface(c->compositor);

    wl_display_roundtrip(d);

//    while( done == false)
//        wl_display_dispatch(d);

    wl_display_disconnect(d);

    delete c;
    return 0;
}
