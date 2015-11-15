//
// Created by wastl on 15.11.15.
//

#ifndef MARMOTTA_PERSISTENCE_H
#define MARMOTTA_PERSISTENCE_H

#include <string>
#include <functional>

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>
#include <leveldb/db.h>
#include <leveldb/cache.h>
#include <leveldb/comparator.h>

#include <google/protobuf/wrappers.pb.h>

#include "model/rdf_model.h"
#include "service/sail.pb.h"
#include "service/sail.grpc.pb.h"

namespace marmotta {
namespace persistence {

namespace svc = marmotta::service::proto;

/**
 * A custom comparator treating the bytes in the key as unsigned char.
 */
class KeyComparator : public leveldb::Comparator {
 public:
    int Compare(const leveldb::Slice& a, const leveldb::Slice& b) const;

    const char* Name() const { return "KeyComparator"; }
    void FindShortestSeparator(std::string*, const leveldb::Slice&) const { }
    void FindShortSuccessor(std::string*) const { }
};



class LevelDBService : public svc::SailService::Service {
 public:
    LevelDBService(const std::string& path, int64_t cacheSize);

    grpc::Status AddNamespaces(grpc::ServerContext* context,
                               grpc::ServerReader<rdf::proto::Namespace>* reader,
                               google::protobuf::Int64Value* result) override;

    grpc::Status AddStatements(grpc::ServerContext* context,
                               grpc::ServerReader<rdf::proto::Statement>* reader,
                               google::protobuf::Int64Value* result) override;

    grpc::Status GetStatements(grpc::ServerContext* context,
                               const rdf::proto::Statement* pattern,
                               grpc::ServerWriter<rdf::proto::Statement>* result) override;

    grpc::Status RemoveStatements(grpc::ServerContext* context,
                                  const rdf::proto::Statement* pattern,
                                  google::protobuf::Int64Value* result) override;

 private:
    std::unique_ptr<KeyComparator> comparator;
    std::unique_ptr<leveldb::Cache> cache;
    std::unique_ptr<leveldb::Options> options;

    // We currently support efficient lookups by subject, context and object.
    std::unique_ptr<leveldb::DB> db_spoc, db_cspo, db_opsc, db_cops, db_ns_prefix, db_ns_url;
};



}  // namespace persistence
}  // namespace marmotta

#endif //MARMOTTA_PERSISTENCE_H
