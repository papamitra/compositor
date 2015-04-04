
#include <wayland-client.h>
#include <wayland-client-protocol.h>

const wl_message requests[] = {
    {"stop", "", NULL},
};

const wl_message events[] = {
    {"stopped", "", NULL},
};

const wl_interface compositor_interface = {
    "compositor", 1,
    1, requests,
    1, events
};

enum {
	STOP = 0
};

enum {
	STOPPED = 0
};
