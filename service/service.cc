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

namespace marmotta {
namespace service {

// A STL iterator wrapper around a client reader.
template <class Proto>
class ReaderIterator : public persistence::Iterator<Proto> {
 public:
    ReaderIterator() : finished(true) { }

    ReaderIterator(grpc::ServerReader<Proto>* r) : reader(r), finished(false) {
        // Immediately move to first element.
        operator++();
    }

    persistence::Iterator<Proto>& operator++() override {
        if (!finished) {
            finished = !reader->Read(&buffer);
        }
        return *this;
    }

    Proto& operator*() override {
        return buffer;
    }

    Proto* operator->() override {
        return &buffer;
    }

    bool operator==(const persistence::Iterator<Proto>& other) override {
        return finished == static_cast<const ReaderIterator<Proto>&>(other).finished;
    }

    bool operator!=(const persistence::Iterator<Proto>& other) override {
        return finished != static_cast<const ReaderIterator<Proto>&>(other).finished;
    }

    static ReaderIterator<Proto> end() {
        return ReaderIterator<Proto>();
    }

 private:
    grpc::ServerReader<Proto>* reader;
    Proto buffer;
    bool finished;
};

typedef ReaderIterator<rdf::proto::Statement> StatementIterator;
typedef ReaderIterator<rdf::proto::Namespace> NamespaceIterator;



Status LevelDBService::AddNamespaces(
        ServerContext* context, ServerReader<Namespace>* reader, Int64Value* result) {

    auto begin = NamespaceIterator(reader);
    auto end   = NamespaceIterator::end();
    int64_t count = persistence.AddNamespaces(begin, end);
    result->set_value(count);

    return Status::OK;
}

Status LevelDBService::AddStatements(
        ServerContext* context, ServerReader<Statement>* reader, Int64Value* result) {

    auto begin = StatementIterator(reader);
    auto end   = StatementIterator::end();
    int64_t count = persistence.AddStatements(begin, end);
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