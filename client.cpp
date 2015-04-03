
#include <wayland-client.h>

#include <cassert>
#include <cstdio>

static void
registry_handle_global(void *data, struct wl_registry *registry,
                        uint32_t id, const char *intf, uint32_t ver)
{
    printf("registry_handle_globals: %s\n", intf);
}

static const struct wl_registry_listener registry_listener = {
    registry_handle_global,
    NULL
};

int main(int argc, char *argv[])
{
    wl_display* const d = wl_display_connect(NULL);
    assert(d);

    wl_registry* const r = wl_display_get_registry(d);
    assert(r);

    wl_registry_add_listener(r, &registry_listener, NULL);

    assert(wl_display_roundtrip(d) != -1);

    wl_display_disconnect(d);

    return 0;
}
