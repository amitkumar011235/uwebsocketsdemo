#include <uwebsockets/App.h>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>

// Struct to store user data for each WebSocket connection
struct PerSocketData {
    std::string role;
};

// Store connected WebSocket clients
std::unordered_map<uWS::WebSocket<false, true, PerSocketData>*, std::string> clients;

int main() {
    // Define WebSocket behavior with PerSocketData as template
    uWS::App()
        .ws<PerSocketData>("/*", {
            .open = [](auto* ws) {
                std::cout << "New WebSocket connection\n";
                ws->getUserData()->role = "";  // Initialize role with empty string
                clients[ws] = "unknown";  // Add client to the map
            },
            .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
                std::cout << "Received message: " << message << "\n";
                ws->getUserData()->role = std::string(message);  // Store role

                // Broadcast the message to other clients
                for (auto& [client, role] : clients) {
                    if (client != ws) {  // Avoid sending message to the same client
                        client->send(message, opCode);
                    }
                }
            },
            .close = [](auto* ws, int code, std::string_view message) {
                std::cout << "WebSocket connection closed\n";
                clients.erase(ws);  // Remove client from the map
            }
        })

        // Serve static HTML files
        .get("/*", [](auto* res, auto* req) {
            std::ifstream file("../public/index.html");
            if (!file.is_open()) {
                res->writeStatus("404 Not Found")->end("HTML file not found.");
                return;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string html = buffer.str();
            res->writeHeader("Content-Type", "text/html")->end(html);
        })

        // Start the server and listen on port 9001
        .listen(9001, [](auto* listen_socket) {
            if (listen_socket) {
                std::cout << "Server is listening on port 9001\n";
            }
        })
        .run();

    return 0;
}
