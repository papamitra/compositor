
#include <wayland-client.h>
#include <wayland-client-protocol.h>

#include <string>

#include <cassert>
#include <cstdio>

namespace yawc {

struct client {
    wl_display* display;
    wl_compositor* compositor;
    wl_proxy* object;
};

struct display {
    wl_display* display;
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

void draw(display& dpy, wl_surface* surface) {
//    wl_egl_window* window = wl_egl_window_create(surface, 300,200);
//    assert(window);
}

} // namespace yawc

int main(int argc, char *argv[])
{
    auto c = new yawc::client{};
    auto dpy = new yawc::display{};

    dpy->display = wl_display_connect(NULL);
    assert(dpy->display);

    wl_registry* const r = wl_display_get_registry(dpy->display);
    
    wl_registry_add_listener(r, &yawc::registry_listener, c);
    assert(wl_display_roundtrip(dpy->display) != -1);

    wl_display_flush(dpy->display);

    wl_surface* surface = wl_compositor_create_surface(c->compositor);

    yawc::draw(*dpy, surface);

}
