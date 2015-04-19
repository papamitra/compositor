
#include "compositor.hpp"

#include <cassert>
#include <cstdio>
#include <iostream>

#include <unistd.h>

namespace yawc {

struct surface {
    wl_resource* resource;
};

static const struct wl_surface_interface surface_interface = {};

namespace {

void handle_create_surface(
    struct wl_client *client,
    struct wl_resource *resource, uint32_t id)
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;

	compositor *comp = static_cast<compositor*>(wl_resource_get_user_data(resource));
	surface *sf = new surface{};

	if (!sf) {
		wl_resource_post_no_memory(resource);
		return;
	}

	sf->resource =
		wl_resource_create(client, &wl_surface_interface,
				   wl_resource_get_version(resource), id);
	if (!sf->resource) {
        delete sf;
        wl_resource_post_no_memory(resource);
        return;
    }
    wl_resource_set_implementation(sf->resource, &surface_interface,
                                   sf, nullptr /*TODO: destroy_surface*/);
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
