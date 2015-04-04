
#include "protocol.h"

struct compositor_interface {
    void (*stop)(struct wl_client *client,
                 struct wl_resource *resource);
};
