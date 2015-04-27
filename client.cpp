
#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <cassert>
#include <cstdio>
#include <string>
#include <memory>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>

#ifndef EGL_PLATFORM_WAYLAND_KHR
#define EGL_PLATFORM_WAYLAND_KHR 0x31D8
#endif

#include <wayland-egl.h>

namespace yawc {

struct client {
    wl_display* display;
    wl_compositor* compositor;
    wl_proxy* object;
};

struct display {
    wl_display* display;
    struct {
        EGLDisplay display;
        EGLContext context;
        EGLConfig config;
    } egl;
};

static inline void *
get_egl_proc_address(const char *address)
{
    const char* exts = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
    if (!exts) {
        return NULL;
    }

    const std::string extensions(exts);

    if (std::string::npos != extensions.find("EGL_EXT_platform_wayland")
        || (std::string::npos != extensions.find("EGL_KHR_platform_wayland"))) {
        return reinterpret_cast<void *>(eglGetProcAddress(address));
    }
    return NULL;
}

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

static inline EGLDisplay
get_egl_display(EGLenum platform, void *native_display,
				const EGLint *attrib_list)
{
	static PFNEGLGETPLATFORMDISPLAYEXTPROC get_platform_display = NULL;

	if (!get_platform_display) {
		get_platform_display =
            reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(get_egl_proc_address(
                                                                  "eglGetPlatformDisplayEXT"));
	}

	if (get_platform_display)
		return get_platform_display(platform,
					    native_display, attrib_list);

	return eglGetDisplay((EGLNativeDisplayType) native_display);
}

static void
init_egl(display& display /*, struct window *window*/)
{
	static const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	EGLint config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_ALPHA_SIZE, 1,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	EGLint n, count, i, size;
	EGLConfig *configs;
	EGLBoolean ret;

	display.egl.display =
		get_egl_display(EGL_PLATFORM_WAYLAND_KHR,
						display.display, NULL);
	assert(display.egl.display);

    EGLint major,minor;
	ret = eglInitialize(display.egl.display, &major, &minor);
	assert(ret == EGL_TRUE);
	ret = eglBindAPI(EGL_OPENGL_ES_API);
	assert(ret == EGL_TRUE);

	if (!eglGetConfigs(display.egl.display, NULL, 0, &count) || count < 1)
		assert(0);

	configs = new EGLConfig[count];
	assert(configs);

	ret = eglChooseConfig(display.egl.display, config_attribs,
			      configs, count, &n);
	assert(ret && n >= 1);

	for (i = 0; i < n; i++) {
		eglGetConfigAttrib(display.egl.display,
				   configs[i], EGL_BUFFER_SIZE, &size);
//FIXME		if (window->buffer_size == size) {
        if (32 == size) {
			display.egl.config = configs[i];
			break;
		}
	}

    delete[] configs;
    if (display.egl.config == NULL) {
        fprintf(stderr, "did not find config with buffer size %d\n",
//FIXME      window->buffer_size);
                32);
        exit(EXIT_FAILURE);
    }

    display.egl.context = eglCreateContext(display.egl.display,
                                           display.egl.config,
                                           EGL_NO_CONTEXT, context_attribs);
    assert(display.egl.context);

#if 0
	display->swap_buffers_with_damage = NULL;
	extensions = eglQueryString(display.egl.display, EGL_EXTENSIONS);
	if (extensions &&
	    strstr(extensions, "EGL_EXT_swap_buffers_with_damage") &&
	    strstr(extensions, "EGL_EXT_buffer_age"))
		display->swap_buffers_with_damage =
			(PFNEGLSWAPBUFFERSWITHDAMAGEEXTPROC)
			eglGetProcAddress("eglSwapBuffersWithDamageEXT");

	if (display->swap_buffers_with_damage)
		printf("has EGL_EXT_buffer_age and EGL_EXT_swap_buffers_with_damage\n");
#endif
}


static inline EGLSurface
create_egl_surface(EGLDisplay dpy, EGLConfig config,
				   void *native_window,
				   const EGLint *attrib_list)
{
	static PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC
		create_platform_window = NULL;

	if (!create_platform_window) {
		create_platform_window =
            reinterpret_cast<PFNEGLCREATEPLATFORMWINDOWSURFACEEXTPROC>(
                get_egl_proc_address("eglCreatePlatformWindowSurfaceEXT"));
	}

	if (create_platform_window)
		return create_platform_window(dpy, config,
					      native_window,
					      attrib_list);

	return eglCreateWindowSurface(dpy, config,
				      (EGLNativeWindowType) native_window,
				      attrib_list);
}

void draw(display& dpy, wl_surface* surface) {
    wl_egl_window* window = wl_egl_window_create(surface, 300,200);
    assert(window);
}

} // namespace yawc

int main(int argc, char *argv[])
{
    auto c = new yawc::client{};
    auto dpy = new yawc::display{};

    dpy->display = wl_display_connect(NULL);
    assert(dpy->display);

    yawc::init_egl(*dpy);

    wl_registry* const r = wl_display_get_registry(dpy->display);
    assert(r);

    wl_registry_add_listener(r, &yawc::registry_listener, c);
    assert(wl_display_roundtrip(dpy->display) != -1);

    wl_display_flush(dpy->display);

    wl_surface* surface = wl_compositor_create_surface(c->compositor);

    yawc::draw(*dpy, surface);
//    while( done == false)
//        wl_display_dispatch(d);

    wl_display_roundtrip(dpy->display);

    wl_display_disconnect(dpy->display);

    delete c;
    return 0;
}
