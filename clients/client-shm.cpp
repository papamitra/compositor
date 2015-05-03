
#include <wayland-client.h>
#include <wayland-client-protocol.h>

#include <iostream>
#include <string>

#include <cassert>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/mman.h>

namespace yawc {

struct display;
struct client {
    struct display* display;
    wl_compositor* compositor;
    wl_proxy* object;
};

struct display {
    wl_display* display;
    uint32_t formats;
    wl_shm *shm;
};

struct buffer {
    wl_buffer* buffer;
    void *shm_data;
};

static int create_anonymous_file(size_t size) {
    static const char* fname = "/tmp/yawc-temp-XXXXXX";

    char* fname_dup = strdup(fname);

    int fd = mkostemp(fname_dup, O_CLOEXEC);
    if(fd < 0) {
//        unlink(fd);
        return -1;
    }

    int ret = ftruncate(fd, size);
    if (ret < 0) {

    }

    delete fname_dup;

    return fd;
}

static void
buffer_release(void *data, struct wl_buffer *buffer)
{
    std::cerr << __PRETTY_FUNCTION__ << std::endl;
//	struct buffer *mybuf = data;

//	mybuf->busy = 0;
}

static const struct wl_buffer_listener buffer_listener = {
	buffer_release
};

static int
create_shm_buffer(struct display *display, struct buffer *buffer,
                  int width, int height, uint32_t format)
{
    std::cerr << __PRETTY_FUNCTION__ << std::endl;
    struct wl_shm_pool *pool;
    int fd, size, stride;
    void *data;

    stride = width * 4;
    size = stride * height;

    fd = create_anonymous_file(size);
    if (fd < 0) {
        fprintf(stderr, "creating a buffer file for %d B failed: %m\n",
                size);
        return -1;
    }

    data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        fprintf(stderr, "mmap failed: %m\n");
        close(fd);
        return -1;
    }

    pool = wl_shm_create_pool(display->shm, fd, size);
    buffer->buffer = wl_shm_pool_create_buffer(pool, 0,
                                            width, height,
                                            stride, format);
    wl_buffer_add_listener(buffer->buffer, &buffer_listener, buffer);
    wl_shm_pool_destroy(pool);
    close(fd);

    buffer->shm_data = data;

    return 0;
}

static void
shm_format(void *data, struct wl_shm *wl_shm, uint32_t format)
{
	struct display *d = static_cast<display*>(data);

	d->formats |= (1 << format);
}

struct wl_shm_listener shm_listener = {
	shm_format
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
    } else if (ifname == "wl_shm") {
        c->display->shm = static_cast<wl_shm*>(wl_registry_bind(registry,
                                                            id, &wl_shm_interface, 1));
        wl_shm_add_listener(c->display->shm, &shm_listener, c->display);
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
    auto display = new yawc::display{};
    c->display = display;

    display->display = wl_display_connect(NULL);
    assert(display->display);

    wl_registry* const r = wl_display_get_registry(display->display);

    wl_registry_add_listener(r, &yawc::registry_listener, c);
    assert(wl_display_roundtrip(display->display) != -1);

    wl_display_flush(display->display);

    wl_surface* surface = wl_compositor_create_surface(c->compositor);

    auto buffer = new yawc::buffer;
    yawc::create_shm_buffer(display, buffer, 200,100, WL_SHM_FORMAT_XRGB8888);

    memset(buffer->shm_data, 0x00,
           200 * 100 * 4);

    wl_surface_attach(surface, buffer->buffer, 0, 0);

    wl_surface_commit(surface);

    assert(wl_display_roundtrip(display->display) != -1);
    yawc::draw(*display, surface);

    for(;;);
}
