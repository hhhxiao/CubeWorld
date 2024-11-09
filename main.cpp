#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "game_main.h"

#include <moodycamel/concurrentqueue.h>
#include <parallel_hashmap/phmap.h>
int main() {
    GameMain main;
    main.init();
    main.show();
    // flat_hash_map<std::string, std::string> email = {
    //     {"tom", "tom@gmail.com"}, {"jeff", "jk@gmail.com"}, {"jim", "jimg@microsoft.com"}};

    // // Iterate and print keys and values
    // for (const auto& n : email) std::cout << n.first << "'s email is: " << n.second << "\n";

    // // Add a new entry
    // email["bill"] = "bg@whatever.com";

    // // and print it
    // std::cout << "bill's email is: " << email["bill"] << "\n";
    return 0;
}