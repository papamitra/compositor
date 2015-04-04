
#include <wayland-client.h>

#include <cassert>
#include <cstdio>
#include <string>
#include <memory>

#include "protocol-client.h"

static bool done = false;

static void handle_stopped(void*, void*) {
    printf("handle_stopped\n");
    done = true;
}

static const struct compositor_listener compositor_listener = {
	handle_stopped
};

struct client {
    wl_display* display;
    wl_proxy* object;
};

static void
registry_handle_global(void *data, struct wl_registry *registry,
                        uint32_t id, const char *intf, uint32_t ver)
{
    client* c = static_cast<client*>(data);

    printf("registry_handle_globals: %s\n", intf);
    const std::string ifname = intf;

    if (ifname != "compositor") return;

    void* interface = wl_registry_bind(registry, id, &compositor_interface, ver);
    assert(interface);
    c->object = static_cast<wl_proxy*>(interface);

    wl_proxy_add_listener(static_cast<wl_proxy *>(interface),
                          (void (**)(void)) &compositor_listener, data);

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

    wl_proxy_marshal((struct wl_proxy *) c->object, STOP);

    wl_display_flush(d);

    while( done == false)
        wl_display_dispatch(d);

    wl_display_disconnect(d);

    delete c;
    return 0;
}
