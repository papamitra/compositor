
#include "surface.hpp"

#include <wayland-server.h>
#include <wayland-server-protocol.h>

#include <new>
#include <iostream>

#define container_of(ptr, type, member) ({				\
	const __typeof__( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})

namespace yawc {

struct buffer {
    wl_resource* resource;
    wl_listener destroy_listener;
};

namespace {

static void
buffer_destroy_handler(struct wl_listener *listener, void *data)
{
	struct buffer *buf =
		container_of(listener, buffer, destroy_listener);

//	wl_signal_emit(&buffer->destroy_signal, buffer);
    delete buf;
}

buffer* buffer_from_resource(struct wl_resource *resource)
{
	wl_listener* const listener = wl_resource_get_destroy_listener(resource,
                                                                   buffer_destroy_handler);
	if (listener)
		return container_of(listener, struct buffer,
                            destroy_listener);

	auto buffer = new struct buffer;
	buffer->resource = resource;
//	wl_signal_init(&buffer->destroy_signal);
	buffer->destroy_listener.notify = buffer_destroy_handler;
//	buffer->y_inverted = 1;
	wl_resource_add_destroy_listener(resource, &buffer->destroy_listener);

	return buffer;
}

void destroy_surface(struct wl_client *client, struct wl_resource *resource) {
    std::clog << __PRETTY_FUNCTION__ << std::endl;
    wl_resource_destroy(resource);
}

void surface_attach(struct wl_client *client,
                    struct wl_resource *resource,
                    struct wl_resource *buffer_resource,
                    int32_t sx,
                    int32_t sy) {
    std::clog << __PRETTY_FUNCTION__ << std::endl;

    auto *surface = static_cast<struct surface*>(wl_resource_get_user_data(resource));
    buffer* buffer=NULL;

    if (buffer_resource) {
        buffer = buffer_from_resource(buffer_resource);
        if (buffer == NULL) {
            wl_client_post_no_memory(client);
            return;
        }
    }

/* Attach, attach, without commit in between does not send
 * wl_buffer.release. */
//weston_surface_state_set_buffer(&surface->pending, buffer);

    surface->buffer = buffer;
    surface->x = sx;
    surface->y = sy;

}

static void surface_commit(struct wl_client *client, struct wl_resource *resource) {
    std::clog << __PRETTY_FUNCTION__ << std::endl;

    auto surface = static_cast<struct surface*>(wl_resource_get_user_data(resource));

    wl_shm_buffer* buffer = wl_shm_buffer_get(surface->buffer->resource);

    std::clog << "stride == " << wl_shm_buffer_get_stride(buffer) << std::endl;

}

const struct wl_surface_interface interface = {
    destroy_surface,
    surface_attach,
    NULL, //surface_damage,
    NULL, //surface_frame,
    NULL, //surface_set_opaque_region,
    NULL, //surface_set_input_region,
    surface_commit,
};

void destroy_surface_resource(struct wl_resource *resource)
{
    std::clog << __PRETTY_FUNCTION__ << std::endl;

    struct surface *surf = static_cast<surface*>(wl_resource_get_user_data(resource));
    delete surf;
}

}

surface::surface(wl_client* client, int ver, uint32_t id) : resource(nullptr) {

    std::clog << __PRETTY_FUNCTION__ << std::endl;

    resource =
        wl_resource_create(client, &wl_surface_interface,
                           ver, id);

    if (!resource) {
        throw new std::bad_alloc();
    }

    wl_resource_set_implementation(resource, &interface,
                                   this, destroy_surface_resource);
}

surface::~surface() {
    // Do not call below, if called from resource destroy handler.
    // wl_resource_destroy(resource_);
}

}
