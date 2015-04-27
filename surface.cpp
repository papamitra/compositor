
#include "surface.hpp"

#include <wayland-server.h>
#include <wayland-server-protocol.h>

#include <new>
#include <iostream>

namespace yawc {

namespace {

void destroy_surface(struct wl_client *client, struct wl_resource *resource) {
    std::clog << __PRETTY_FUNCTION__ << std::endl;
    wl_resource_destroy(resource);
}

void surface_attach(struct wl_client *client,
                    struct wl_resource *resource,
                    struct wl_resource *buffer,
                    int32_t x,
                    int32_t y) {
    std::clog << __PRETTY_FUNCTION__ << std::endl;
}

const struct wl_surface_interface interface = {
    destroy_surface,
    surface_attach,
};

void destroy_surface_resource(struct wl_resource *resource)
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;

    struct surface *surf = static_cast<surface*>(wl_resource_get_user_data(resource));
    delete surf;
}

}

surface::surface(wl_client* client, int ver, uint32_t id) : resource_(nullptr) {

    std::clog << __PRETTY_FUNCTION__ << std::endl;

    resource_ =
        wl_resource_create(client, &wl_surface_interface,
                           ver, id);

    if (!resource_) {
        throw new std::bad_alloc();
    }

    wl_resource_set_implementation(resource_, &interface,
                                   this, destroy_surface_resource);
}

surface::~surface() {
    // Do not call below, if called from resource destroy handler.
    // wl_resource_destroy(resource_);
}

}
