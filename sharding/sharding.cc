//
// Created by wastl on 19.11.15.
//
#include <thread>

#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <grpc++/support/sync_stream.h>

#include "sharding.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;
using google::protobuf::Int64Value;

namespace marmotta {
namespace sharding {

template<typename Request,
        Status (svc::SailService::Stub::*ClientMethod)(ClientContext*, const Request&, Int64Value*)>
void Fanout(const Request& request,
            std::vector<std::unique_ptr<svc::SailService::Stub>> &stubs,
            Int64Value *result) {
    util::ThreadPool workers(stubs.size());

    std::vector<ClientContext> contexts(stubs.size());
    std::vector<Int64Value> responses(stubs.size());
    std::vector<Status> statuses(stubs.size());


    for (int i=0; i<stubs.size(); i++) {
        workers.Schedule([&]() {
            statuses[i] = ((*stubs[i]).*ClientMethod)(&contexts[i], request, &responses[i]);
        });
    }

    // need to wait until all are completed now.
    workers.Join();

    int64_t r = 0;
    for (const auto& v : responses) {
        r += v.value();
    }
    result->set_value(r);
};

ShardingService::ShardingService(std::vector<std::string> backends) {
    for (auto backend : backends) {
        stubs.push_back(svc::SailService::NewStub(
                grpc::CreateChannel(backend, grpc::InsecureCredentials())));
    }
}

grpc::Status ShardingService::AddNamespaces(grpc::ServerContext *context,
                                            grpc::ServerReader<rdf::proto::Namespace> *reader,
                                            google::protobuf::Int64Value *result) {

    util::ThreadPool workers(stubs.size());

    std::vector<ClientContext> contexts(stubs.size());
    std::vector<Int64Value> stats(stubs.size());

    std::vector<std::unique_ptr<ClientWriter<rdf::proto::Namespace>>> writers;
    for (int i=0; i<stubs.size(); i++) {
        writers.push_back(stubs[i]->AddNamespaces(&contexts[i], &stats[i]));
    }

    // Iterate over all namespaces and schedule a write task.
    rdf::proto::Namespace ns;
    while (reader->Read(&ns)) {
        for (auto& w : writers) {
            // Copy by value so multi-threading doesn't update ns in the meantime.
            workers.Schedule([ns, &w](){
                w->Write(ns);
            });
        }
    }

    workers.Join();

    for (auto& w : writers) {
        w->WritesDone();
        w->Finish();
    }

    result->set_value(stats[0].value());

    return Status::OK;
}

grpc::Status ShardingService::AddStatements(grpc::ServerContext *context,
                                            grpc::ServerReader<rdf::proto::Statement> *reader,
                                            google::protobuf::Int64Value *result) {
    return Status::OK;
}

grpc::Status ShardingService::GetStatements(grpc::ServerContext *context, const rdf::proto::Statement *pattern,
                                            grpc::ServerWriter<rdf::proto::Statement> *result) {
    return Status::OK;
}

grpc::Status ShardingService::RemoveStatements(grpc::ServerContext *context, const rdf::proto::Statement *pattern,
                                               google::protobuf::Int64Value *result) {
    return Status::OK;
}

grpc::Status ShardingService::Clear(grpc::ServerContext *context, const svc::ContextRequest *contexts,
                                    google::protobuf::Int64Value *result) {
    return Status::OK;
}

grpc::Status ShardingService::Size(grpc::ServerContext *context, const svc::ContextRequest *contexts,
                                   google::protobuf::Int64Value *result) {
    Fanout<svc::ContextRequest, &svc::SailService::Stub::Size>(*contexts, stubs, result);
    return Status::OK;
}
}
}