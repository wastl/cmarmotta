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

#include <google/protobuf/wrappers.pb.h>

#include "model/rdf_model.h"
#include "service/sail.pb.h"
#include "service/sail.grpc.pb.h"

namespace marmotta {
namespace persistence {

namespace svc = marmotta::service::proto;

class LevelDBService : public svc::SailService::Service {
 public:
    enum IndexType {
        SPOC, CSPO, OPSC, COPS
    };

    LevelDBService(const std::string& path);

    grpc::Status AddNamespaces(grpc::ServerContext* context,
                               grpc::ServerReader<rdf::proto::Namespace>* reader,
                               google::protobuf::Int64Value* result) override;

    grpc::Status AddStatements(grpc::ServerContext* context,
                               grpc::ServerReader<rdf::proto::Statement>* reader,
                               google::protobuf::Int64Value* result) override;

    grpc::Status GetStatements(grpc::ServerContext* context,
                               const rdf::proto::Statement* pattern,
                               grpc::ServerWriter<rdf::proto::Statement>* result) override;

 private:
    // We currently support efficient lookups by subject, context and object.
    std::unique_ptr<leveldb::DB> db_spoc, db_cspo, db_opsc, db_cops, db_ns_prefix, db_ns_url;

    std::hash<std::string> hash_fn;

    char* cacheKey(const std::string* a, const std::string* b,
                   const std::string* c, const std::string* d) const;

    char* maxKey(const std::string* a, const std::string* b,
                 const std::string* c, const std::string* d) const;

    IndexType indexForPattern(const rdf::proto::Statement* pattern) const;

};

}  // namespace persistence
}  // namespace marmotta

#endif //MARMOTTA_PERSISTENCE_H
