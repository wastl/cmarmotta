//
// Created by wastl on 15.11.15.
//

#include "service.h"

using grpc::Status;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using google::protobuf::Int64Value;
using google::protobuf::Message;
using marmotta::rdf::proto::Statement;
using marmotta::rdf::proto::Namespace;
using marmotta::rdf::proto::Resource;
using marmotta::service::proto::ContextRequest;
using marmotta::persistence::NamespaceIterator;
using marmotta::persistence::StatementIterator;

namespace marmotta {
namespace service {


Status LevelDBService::AddNamespaces(
        ServerContext* context, ServerReader<Namespace>* reader, Int64Value* result) {

    int64_t count = persistence.AddNamespaces(NamespaceIterator(reader), NamespaceIterator::end());
    result->set_value(count);

    return Status::OK;
}

Status LevelDBService::AddStatements(
        ServerContext* context, ServerReader<Statement>* reader, Int64Value* result) {

    int64_t count = persistence.AddStatements(StatementIterator(reader), StatementIterator::end());
    result->set_value(count);

    return Status::OK;
}


Status LevelDBService::GetStatements(
        ServerContext* context, const Statement* pattern, ServerWriter<Statement>* result) {

    persistence.GetStatements(*pattern, [&result](const Statement& stmt) {
        result->Write(stmt);
    });

    return Status::OK;
}

Status LevelDBService::RemoveStatements(
        ServerContext* context, const Statement* pattern, Int64Value* result) {

    int64_t count = persistence.RemoveStatements(*pattern);
    result->set_value(count);

    return Status::OK;
}

Status LevelDBService::Clear(
        ServerContext* context, const ContextRequest* contexts, Int64Value* result) {


    int64_t count = 0;

    Statement pattern;
    if (contexts->context_size() > 0) {
        for (const Resource &r : contexts->context()) {
            pattern.mutable_context()->CopyFrom(r);
            count += persistence.RemoveStatements(pattern);
        }
    } else {
        count += persistence.RemoveStatements(pattern);
    }
    result->set_value(count);

    return Status::OK;
}

Status LevelDBService::Size(
        ServerContext* context, const ContextRequest* contexts, Int64Value* result) {

    int64_t count = 0;

    if (contexts->context_size() > 0) {
        Statement pattern;
        for (const Resource &r : contexts->context()) {
            pattern.mutable_context()->CopyFrom(r);

            persistence.GetStatements(pattern, [&count](const Statement& stmt) {
                count++;
            });
        }
    } else {
        Statement pattern;

        persistence.GetStatements(pattern, [&count](const Statement& stmt) {
            count++;
        });
    }
    result->set_value(count);

    return Status::OK;

}


}  // namespace service
}  // namespace marmotta