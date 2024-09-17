#include "broadcast.h"

static int _isExit = 0;

void fw_broadcast_exit() {
    _isExit = 1;
}

int fw_broadcast_isExit() {
    return _isExit;
}
