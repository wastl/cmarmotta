//
// Created by wastl on 15.11.15.
//
#include <fstream>

#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>

#include <google/protobuf/wrappers.pb.h>

#include "model/rdf_model.h"
#include "parser/rdf_parser.h"
#include "serializer/rdf_serializer.h"
#include "service/sail.pb.h"
#include "service/sail.grpc.pb.h"


using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::ClientReaderWriter;
using grpc::ClientWriter;
using grpc::Status;

using namespace marmotta;
namespace svc = marmotta::service::proto;


class MarmottaClient {
 public:
    MarmottaClient(std::shared_ptr<Channel> channel)
            : stub_(svc::SailService::NewStub(channel)) {}

    void importDataset(std::istream& in, parser::Format format) {
        ClientContext nscontext, stmtcontext;

        google::protobuf::Int64Value nsstats;
        google::protobuf::Int64Value stmtstats;

        std::unique_ptr<ClientWriter<rdf::proto::Namespace> > nswriter(
                stub_->AddNamespaces(&nscontext, &nsstats));
        std::unique_ptr<ClientWriter<rdf::proto::Statement> > stmtwriter(
                stub_->AddStatements(&stmtcontext, &stmtstats));

        parser::Parser p("http://www.example.com", parser::Format::RDFXML);
        p.setStatementHandler([&stmtwriter](const rdf::Statement& stmt) {
            stmtwriter->Write(stmt.getMessage());
        });
        p.setNamespaceHandler([&nswriter](const rdf::Namespace& ns) {
            nswriter->Write(ns.getMessage());
        });
        p.parse(in);

        stmtwriter->WritesDone();
        nswriter->WritesDone();

        Status nsst = nswriter->Finish();
        Status stmtst = stmtwriter->Finish();

        if (nsst.ok() && stmtst.ok()) {
            std::cout << "Added " << nsstats.value() << " namespaces and "
                                  << stmtstats.value() << " statements" << std::endl;
        } else {
            std::cout << "Failed writing data to server: " << stmtst.error_message() << std::endl;
        }
    }

 private:
    std::unique_ptr<svc::SailService::Stub> stub_;
};


int main(int argc, const char** argv) {
    MarmottaClient client(
            grpc::CreateChannel("localhost:10000", grpc::InsecureCredentials()));

    std::ifstream in(argv[1]);
    std::cout << "Importing " << argv[1] << " ... " << std::endl;
    client.importDataset(in, parser::Format::RDFXML);
    std::cout << "Finished!" << std::endl;

}