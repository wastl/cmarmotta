/*
 * Implementation of a proxy service doing hash-based sharding of statements
 * for storage and retrieval. The shards are passed as vector of host:port
 * pairs to the constructor.
 */
#ifndef MARMOTTA_SHARDING_H
#define MARMOTTA_SHARDING_H

#include <vector>
#include <string>

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

#include <google/protobuf/wrappers.pb.h>

#include "service/sail.pb.h"
#include "service/sail.grpc.pb.h"
#include "model/model.pb.h"


namespace marmotta {
namespace sharding {

namespace svc = marmotta::service::proto;

/**
 * Implementation of a proxy service doing hash-based sharding of statements
 * for storage and retrieval. The shards are passed as vector of host:port
 * pairs to the constructor.
 */
class ShardingService : public svc::SailService::Service {
 public:

    /**
     * Instantiate new sharding service, connecting to the backends provided
     * as argument (vector of host:port pairs).
     */
    ShardingService(std::vector<std::string> backends);

    /**
     * Add namespaces. Since namespaces are potentially needed in all backends,
     * they will be added to all.
     */
    grpc::Status AddNamespaces(grpc::ServerContext* context,
                               grpc::ServerReader<rdf::proto::Namespace>* reader,
                               google::protobuf::Int64Value* result) override;

    /**
     * Add a sequence of statements. Computes a hash over the serialized
     * proto message modulo the number of backends to determine which backend
     * to write to.
     */
    grpc::Status AddStatements(grpc::ServerContext* context,
                               grpc::ServerReader<rdf::proto::Statement>* reader,
                               google::protobuf::Int64Value* result) override;

    /**
     * Retrieve statements matching a certain pattern. Queries all backends in
     * parallel and multiplexes the results.
     */
    grpc::Status GetStatements(grpc::ServerContext* context,
                               const rdf::proto::Statement* pattern,
                               grpc::ServerWriter<rdf::proto::Statement>* result) override;

    /**
     * Remove statements matching a certain pattern. Forwards the request to
     * all backends in parallel.
     */
    grpc::Status RemoveStatements(grpc::ServerContext* context,
                                  const rdf::proto::Statement* pattern,
                                  google::protobuf::Int64Value* result) override;

    /**
     * Process a sequence of updates. For statement updates, computes a hash over the
     * serialized proto message modulo the number of backends to determine which backend
     * to write to. For namespace updates, writes to all backends.
     */
    grpc::Status Update(grpc::ServerContext* context,
                        grpc::ServerReader<service::proto::UpdateRequest>* reader,
                        service::proto::UpdateResponse* result) override;

    /**
     * Clear all statements matching the given context request. Forwards the
     * request to all backends in parallel.
     */
    grpc::Status Clear(grpc::ServerContext* context,
                       const svc::ContextRequest* contexts,
                       google::protobuf::Int64Value* result) override;

    /**
     * Get the size of the combined repository. Forwards the request to all
     * backends in parallel and adds the results.
     */
    grpc::Status Size(grpc::ServerContext* context,
                      const svc::ContextRequest* contexts,
                      google::protobuf::Int64Value* result) override;

 private:
    using StubType = std::unique_ptr<svc::SailService::Stub>;
    using StubList = std::vector<StubType>;

    template <class T>
    using Writer = std::unique_ptr<grpc::ClientWriter<T>>;

    template <class T>
    using WriterList = std::vector<Writer<T>>;

    // Vector holding the RPC stubs to the backends.
    std::vector<std::string> backends;

    // Hash function, computed over binary representation of statement message,
    // modulo the number of backends.
    std::hash<std::string> hash_fn;

    // Make a stub for the backend with the given index.
    StubType makeStub(int backend);
};


}  // namespace sharding
}  // namespace marmotta

#endif //MARMOTTA_SHARDING_H
