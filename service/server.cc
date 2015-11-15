//
// Created by wastl on 15.11.15.
//
#include <gflags/gflags.h>

#include "service/service.h"

using grpc::Status;
using grpc::Server;
using grpc::ServerBuilder;


DEFINE_string(host, "0.0.0.0", "Address/name of server to access.");
DEFINE_string(port, "10000", "Port of server to access.");
DEFINE_string(db, "/tmp/testdb", "Path to database. Will be created if non-existant.");
DEFINE_int64(cache_size, 100 * 1048576, "Cache size used by the database (in bytes).");

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);

    marmotta::service::LevelDBService service(FLAGS_db, FLAGS_cache_size);

    ServerBuilder builder;
    builder.AddListeningPort(FLAGS_host + ":" + FLAGS_port, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Persistence Server listening on " << FLAGS_host << ":" << FLAGS_port << std::endl;

    server->Wait();

    return 0;
}