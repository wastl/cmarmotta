// Binary to start a persistence server implementing the sail.proto API.
//
// Created by wastl on 15.11.15.
//
#include <gflags/gflags.h>
#include <glog/logging.h>
#include <sys/stat.h>
#include <signal.h>

#include "service/service.h"

using grpc::Status;
using grpc::Server;
using grpc::ServerBuilder;


DEFINE_string(host, "0.0.0.0", "Address/name of server to access.");
DEFINE_string(port, "10000", "Port of server to access.");
DEFINE_string(db, "/tmp/testdb", "Path to database. Will be created if non-existant.");
DEFINE_int64(cache_size, 100 * 1048576, "Cache size used by the database (in bytes).");

std::unique_ptr<Server> server;

void stopServer(int signal) {
    if (server.get() != nullptr) {
        LOG(INFO) << "Persistence Server shutting down";
        server->Shutdown();
    }
}

int main(int argc, char** argv) {
    // Initialize Google's logging library.
    google::InitGoogleLogging(argv[0]);
    google::ParseCommandLineFlags(&argc, &argv, true);

    mkdir(FLAGS_db.c_str(), 0700);
    marmotta::service::LevelDBService service(FLAGS_db, FLAGS_cache_size);

    ServerBuilder builder;
    builder.AddListeningPort(FLAGS_host + ":" + FLAGS_port, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    server = builder.BuildAndStart();
    std::cout << "Persistence Server listening on " << FLAGS_host << ":" << FLAGS_port << std::endl;

    LOG(INFO) << "Persistence Server listening on " << FLAGS_host << ":" << FLAGS_port;

    signal(SIGINT, stopServer);

    server->Wait();

    return 0;
}