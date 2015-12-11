/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef MARMOTTA_SERVICE_H
#define MARMOTTA_SERVICE_H

#include "service/persistence.h"

#include <grpc/grpc.h>
#include <grpc++/server.h>
#include <grpc++/server_builder.h>
#include <grpc++/server_context.h>
#include <grpc++/security/server_credentials.h>

#include <google/protobuf/empty.pb.h>
#include <google/protobuf/wrappers.pb.h>

#include "service/sail.pb.h"
#include "service/sail.grpc.pb.h"

namespace marmotta {
namespace service {

namespace svc = marmotta::service::proto;

/**
 * An implementation of the gRPC service interface backed by a LevelDB database.
 */
class LevelDBService : public svc::SailService::Service {
 public:
    LevelDBService(const std::string& path, int64_t cacheSize) : persistence(path, cacheSize) { };

    grpc::Status AddNamespaces(grpc::ServerContext* context,
                               grpc::ServerReader<rdf::proto::Namespace>* reader,
                               google::protobuf::Int64Value* result) override;

    grpc::Status GetNamespace(grpc::ServerContext* context,
                               const rdf::proto::Namespace* pattern,
                               rdf::proto::Namespace* result) override;

    grpc::Status GetNamespaces(grpc::ServerContext* context,
                               const google::protobuf::Empty* ignored,
                               grpc::ServerWriter<rdf::proto::Namespace>* result) override;

    grpc::Status AddStatements(grpc::ServerContext* context,
                               grpc::ServerReader<rdf::proto::Statement>* reader,
                               google::protobuf::Int64Value* result) override;

    grpc::Status GetStatements(grpc::ServerContext* context,
                               const rdf::proto::Statement* pattern,
                               grpc::ServerWriter<rdf::proto::Statement>* result) override;

    grpc::Status RemoveStatements(grpc::ServerContext* context,
                                  const rdf::proto::Statement* pattern,
                                  google::protobuf::Int64Value* result) override;

    grpc::Status GetContexts(grpc::ServerContext* context,
                             const google::protobuf::Empty* ignored,
                             grpc::ServerWriter<rdf::proto::Resource>* result) override;

    grpc::Status Update(grpc::ServerContext* context,
                        grpc::ServerReader<service::proto::UpdateRequest>* reader,
                        service::proto::UpdateResponse* result) override;

    grpc::Status Clear(grpc::ServerContext* context,
                       const svc::ContextRequest* contexts,
                       google::protobuf::Int64Value* result) override;

    grpc::Status Size(grpc::ServerContext* context,
                      const svc::ContextRequest* contexts,
                      google::protobuf::Int64Value* result) override;

    grpc::Status TupleQuery(grpc::ServerContext* context,
                               const svc::SparqlRequest* pattern,
                               grpc::ServerWriter<svc::SparqlResponse>* result) override;
 private:
    persistence::LevelDBPersistence persistence;
};

}
}

#endif //MARMOTTA_SERVICE_H
