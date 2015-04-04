
#include "protocol.h"

struct compositor_listener {
    void (*stopped)(void* data, void*);
};
