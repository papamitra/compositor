
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

	buffer* buf = new buffer;
	buf->resource = resource;
//	wl_signal_init(&buffer->destroy_signal);
	buf->destroy_listener.notify = buffer_destroy_handler;
//	buffer->y_inverted = 1;
	wl_resource_add_destroy_listener(resource, &buf->destroy_listener);

	return buf;
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

    surface *surf = static_cast<surface*>(wl_resource_get_user_data(resource));
    buffer* buf=NULL;

	if (buffer_resource) {
		buf = buffer_from_resource(buffer_resource);
		if (buf == NULL) {
			wl_client_post_no_memory(client);
			return;
		}
	}

	/* Attach, attach, without commit in between does not send
	 * wl_buffer.release. */
//	weston_surface_state_set_buffer(&surface->pending, buffer);

    surf->buf = buf;
	surf->x = sx;
	surf->y = sy;
    
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
