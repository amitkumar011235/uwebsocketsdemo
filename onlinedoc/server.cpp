#include <uwebsockets/App.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <mutex>

using namespace std;

// Struct to store user data for each WebSocket connection
struct PerSocketData {};

// Correct template instantiation for WebSocket
using WebSocket = uWS::WebSocket<false, true, PerSocketData>;

// Store active WebSocket connections
std::unordered_set<WebSocket*> clients;

// Shared document content
std::string shared_document = "Welcome to the Collaborative Document Editor!";
std::mutex doc_mutex;

// Helper function to read files (HTML, JS) into strings
std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Error opening file: " << filepath << std::endl;
        return "";
    }
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

int main() {
    uWS::App()
        .get("/*", [](auto* res, auto* req) {
            std::string url = std::string(req->getUrl());  // Convert string_view to string
            std::string content;

            // Serve files based on the URL
            if (url == "/") {
                cout<<"came to index.html"<<endl;
                content = readFile("../index.html");
                res->writeHeader("Content-Type", "text/html");
            } else if (url == "main.js") {
                cout<<"came to main.js"<<endl;
                content = readFile("../main.js");
                res->writeHeader("Content-Type", "application/javascript");
            } else {
                res->writeStatus("404 Not Found")->end("404: File not found");
                return;
            }

            res->end(content);
        })
        .ws<PerSocketData>("/*", {
            .open = [](WebSocket* ws) {
                std::cout << "Client connected!" << std::endl;

                // Send current document state to the new client
                std::lock_guard<std::mutex> lock(doc_mutex);
                ws->send(shared_document, uWS::OpCode::TEXT);

                clients.insert(ws);
            },
            .message = [](WebSocket* ws, std::string_view message, uWS::OpCode opCode) {
                // Update shared document and broadcast to all other clients
                {
                    std::lock_guard<std::mutex> lock(doc_mutex);
                    shared_document = std::string(message);  // Convert string_view to string
                }
                //cout<<shared_document<<endl;
                for (auto* client : clients) {
                    if (client != ws) {
                        client->send(message, opCode);
                    }
                }
            },
            .close = [](WebSocket* ws, int code, std::string_view message) {
                std::cout << "Client disconnected!" << std::endl;
                clients.erase(ws);
            }
        })
        .listen(9001, [](auto* listen_socket) {
            if (listen_socket) {
                std::cout << "Server listening on port 9001" << std::endl;
            } else {
                std::cerr << "Failed to start server!" << std::endl;
            }
        })
        .run();

    return 0;
}
