//
// Created by wastl on 15.11.15.
//
#include <iostream>

#include <gflags/gflags.h>

#include "util/split.h"
#include "sharding/sharding.h"

using grpc::Status;
using grpc::Server;
using grpc::ServerBuilder;


DEFINE_string(host, "0.0.0.0", "address/name of server to access.");
DEFINE_string(port, "10000", "port of server to access.");
DEFINE_string(backends, "",
              "comma-separated list of host:port pairs of backends to use");

int main(int argc, char** argv) {
    google::ParseCommandLineFlags(&argc, &argv, true);

    marmotta::sharding::ShardingService service(
            marmotta::util::split(FLAGS_backends, ','));

    ServerBuilder builder;
    builder.AddListeningPort(FLAGS_host + ":" + FLAGS_port, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Sharding Server listening on " << FLAGS_host << ":" << FLAGS_port << std::endl;

    server->Wait();

    return 0;
}