
#include <wayland-server.h>
#include <wayland-server-protocol.h>

#include <cassert>
#include <cstdio>
#include <iostream>

#include <unistd.h>

namespace yawc {

struct surface {
    wl_resource* resource;
};

static void
compositor_create_surface(struct wl_client *client,
			  struct wl_resource *resource, uint32_t id)
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;
#if 0
//	struct weston_compositor *ec = wl_resource_get_user_data(resource);
	surface *surf = new surface{};

	if (surf == NULL) {
		wl_resource_post_no_memory(resource);
		return;
	}

	surf->resource =
		wl_resource_create(client, &wl_surface_interface,
				   wl_resource_get_version(resource), id);
	if (surface->resource == NULL) {
		weston_surface_destroy(surface);
		wl_resource_post_no_memory(resource);
		return;
	}
	wl_resource_set_implementation(surface->resource, &surface_interface,
				       surface, destroy_surface);
#endif
}

static const struct wl_compositor_interface compositor_interface = {
    compositor_create_surface,
    NULL
};

static void
compositor_bind(struct wl_client *client, void *data,
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

} // namespace yawc

int main(void) {

    wl_display* d = wl_display_create();

    wl_global* const g = wl_global_create(d, &wl_compositor_interface,
                                          1, d, yawc::compositor_bind);
    assert(g);

    const int stat = wl_display_add_socket(d, "wayland-0");
    assert(stat == 0);

    wl_display_run(d);

    return 0;
}

