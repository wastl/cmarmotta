//
// Created by wastl on 15.11.15.
//
#include "persistence.h"

#include <grpc++/support/sync_stream.h>

using grpc::Status;
using grpc::Server;
using grpc::ServerBuilder;

int main(int argc, const char** argv) {
    std::string server_address("0.0.0.0:10000");
    if (argc > 1) {
        server_address = argv[1];
    }

    marmotta::persistence::LevelDBService service("/tmp/testdb", 100 * 1048576);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Persistence Server listening on " << server_address << std::endl;

    server->Wait();

    return 0;
}