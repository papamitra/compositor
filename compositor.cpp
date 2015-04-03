
#include <wayland-server.h>

#include <cassert>

const wl_message requests[] = {
    {"stop", "", NULL},
};

const wl_message events[] = {
    {"resume", "", NULL},
};

const wl_interface compositor_interface = {
    "compositor", 1,
    1, requests,
    1, events
};

struct compositor_interface {
    void (*stop)(struct wl_client *client,
                 struct wl_resource *resource);
};

static void
handle_stop_display(struct wl_client *client,
                    struct wl_resource *resource)
{
    wl_display *d = static_cast<wl_display*>(wl_resource_get_user_data(resource));

    wl_display_terminate(d);
}

static const struct compositor_interface implementation = {
    handle_stop_display
};

static void
compositor_bind(struct wl_client *client, void *data,
                uint32_t ver, uint32_t id)
{
    struct wl_resource *res;

    res = wl_resource_create(client, &compositor_interface, ver, id);
    if (!res) {
        wl_client_post_no_memory(client);
        assert(0 && "Out of memory");
    }

    wl_resource_set_implementation(res, &implementation, data, NULL);
}

int main(void) {

    wl_display* d = wl_display_create();

    wl_global* const g = wl_global_create(d, &compositor_interface,
                                    1, d, compositor_bind);
    assert(g);

    wl_display_run(d);

    return 0;
}
