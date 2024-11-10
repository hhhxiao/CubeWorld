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

#define _CRT_SECURE_NO_WARNINGS

void initLogger(int argc, char **argv) {
    loguru::g_preamble_date = false;
    loguru::g_preamble_uptime = false;
    loguru::g_internal_verbosity = 0;
    loguru::g_stderr_verbosity = 0;
    loguru::init(argc, argv);
}

int main(int argc, char **argv) {
    initLogger(argc, argv);
    loguru::set_thread_name("Client");
    DataBridge *bridge = new DataBridge();
    ClientMain *client = new ClientMain(bridge);
    LevelServer *server = new LevelServer(bridge);
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
    return 0;
}