#define RMT_ENABLED 0
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include "client_main.h"
#include "bridge.h"
#include "level_server.h"
#include "utils.h"
#include "Remotery.h"

#define _CRT_SECURE_NO_WARNINGS

void initLogger(int argc, char **argv) {
    const auto VB = 1;
    loguru::g_preamble_date = false;
    loguru::g_preamble_uptime = false;
    loguru::g_internal_verbosity = VB;
    loguru::g_stderr_verbosity = VB;
    loguru::g_colorlogtostderr = true;
    loguru::init(argc, argv);
}
void runServerOnly() {
    auto *server = new LevelServer(new DataBridge());
    server->start();
}

int main(int argc, char **argv) {
    Remotery *rmt;
    rmt_CreateGlobalInstance(&rmt);
    initLogger(argc, argv);
    loguru::set_thread_name("Client");
    auto bridge = new DataBridge();
    auto client = new ClientMain(bridge);
    auto server = new LevelServer(bridge);
    client->init();
    std::thread serverThread([server]() {
        loguru::set_thread_name("Server");
        server->start();
    });
    client->show();
    server->stop();
    serverThread.join();
    delete server;
    delete client;
    delete bridge;
    rmt_DestroyGlobalInstance(rmt);
    return 0;
}