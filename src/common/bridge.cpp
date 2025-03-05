#include "bridge.h"
#include "level_server.h"

void TimeCommand::consume(LevelServer *server) {
    if (consumed) return;
    CommandInterface::consume(server);
    LD("Set time to %d ", time_);
    server->setTime(time_);
    consumed = true;
}