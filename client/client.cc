//
// Created by wastl on 15.11.15.
//
#include <fstream>

#include <grpc/grpc.h>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
#include <grpc++/support/sync_stream.h>

#include <google/protobuf/text_format.h>
#include <google/protobuf/empty.pb.h>
#include <google/protobuf/wrappers.pb.h>

#include <gflags/gflags.h>

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
using google::protobuf::TextFormat;

using namespace marmotta;
namespace svc = marmotta::service::proto;

// A STL iterator wrapper around a client reader.
template <class T, class Proto>
class ClientReaderIterator {
 public:
    ClientReaderIterator() : finished(true) { }

    ClientReaderIterator(ClientReader<Proto>* r) : reader(r), finished(false) {
        // Immediately move to first element.
        operator++();
    }

    ClientReaderIterator& operator++() {
        if (!finished) {
            finished = !reader->Read(&buffer);
            if (finished) {
                reader->Finish();
            }
        }
        return *this;
    }

    T operator*() {
        return T(buffer);
    }

    bool operator==(const ClientReaderIterator<T, Proto>& other) {
        return finished == other.finished;
    }

    bool operator!=(const ClientReaderIterator<T, Proto>& other) {
        return finished != other.finished;
    }

    static ClientReaderIterator<T, Proto> end() {
       return ClientReaderIterator<T, Proto>();
    }

 private:
    ClientReader<Proto>* reader;
    Proto buffer;
    bool finished;
};

typedef ClientReaderIterator<rdf::Statement, rdf::proto::Statement> StatementReader;
typedef ClientReaderIterator<rdf::Namespace, rdf::proto::Namespace> NamespaceReader;

class MarmottaClient {
 public:
    MarmottaClient(const std::string& server)
            : stub_(svc::SailService::NewStub(
            grpc::CreateChannel(server, grpc::InsecureChannelCredentials()))) {}

    void importDataset(std::istream& in, parser::Format format) {
        ClientContext nscontext, stmtcontext;

        google::protobuf::Int64Value nsstats;
        google::protobuf::Int64Value stmtstats;

        std::unique_ptr<ClientWriter<rdf::proto::Namespace> > nswriter(
                stub_->AddNamespaces(&nscontext, &nsstats));
        std::unique_ptr<ClientWriter<rdf::proto::Statement> > stmtwriter(
                stub_->AddStatements(&stmtcontext, &stmtstats));

        parser::Parser p("http://www.example.com", format);
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


    void patternQuery(const rdf::Statement &pattern, std::ostream &out, serializer::Format format) {
        ClientContext context;

        std::unique_ptr<ClientReader<rdf::proto::Statement> > reader(
            stub_->GetStatements(&context, pattern.getMessage()));

        serializer::Serializer serializer("http://www.example.com", format);
        serializer.serialize(StatementReader(reader.get()), StatementReader::end(), out);
    }

    void patternDelete(const rdf::Statement &pattern) {
        ClientContext context;
        google::protobuf::Int64Value result;

        Status status = stub_->RemoveStatements(&context, pattern.getMessage(), &result);
        if (status.ok()) {
            std::cout << "Deleted " << result.value() << " statements." << std::endl;
        } else {
            std::cerr << "Failed deleting statements: " << status.error_message() << std::endl;
        }
    }

    void listNamespaces(std::ostream &out) {
        ClientContext context;

        google::protobuf::Empty pattern;

        std::unique_ptr<ClientReader<rdf::proto::Namespace> > reader(
                stub_->GetNamespaces(&context, pattern));

        NamespaceReader it(reader.get());
        for (; it != NamespaceReader::end(); ++it) {
            out << (*it).getPrefix() << " = " << (*it).getUri() << std::endl;
        }
    }

    int64_t size(const svc::ContextRequest& r) {
        ClientContext context;
        google::protobuf::Int64Value result;

        Status status = stub_->Size(&context, r, &result);
        if (status.ok()) {
            return result.value();
        } else {
            return -1;
        }
    }
 private:
    std::unique_ptr<svc::SailService::Stub> stub_;
};


DEFINE_string(format, "rdfxml", "RDF format to use for parsing/serializing.");
DEFINE_string(host, "localhost", "Address/name of server to access.");
DEFINE_string(port, "10000", "Port of server to access.");
DEFINE_string(output, "", "File to write result to.");

int main(int argc, char** argv) {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    google::ParseCommandLineFlags(&argc, &argv, true);

    MarmottaClient client(FLAGS_host + ":" + FLAGS_port);

    if ("import" == std::string(argv[1])) {
        std::ifstream in(argv[2]);
        std::cout << "Importing " << argv[2] << " ... " << std::endl;
        client.importDataset(in, parser::FormatFromString(FLAGS_format));
        std::cout << "Finished!" << std::endl;
    }

    if ("select" == std::string(argv[1])) {
        rdf::proto::Statement query;
        TextFormat::ParseFromString(argv[2], &query);
        if (FLAGS_output != "") {
            std::ofstream out(FLAGS_output);
            client.patternQuery(rdf::Statement(query), out, serializer::FormatFromString(FLAGS_format));
        } else {
            client.patternQuery(rdf::Statement(query), std::cout, serializer::FormatFromString(FLAGS_format));
        }
    }

    if ("delete" == std::string(argv[1])) {
        rdf::proto::Statement query;
        TextFormat::ParseFromString(argv[2], &query);
        client.patternDelete(rdf::Statement(query));
    }

    if ("size" == std::string(argv[1])) {
        svc::ContextRequest query;
        TextFormat::ParseFromString(argv[2], &query);
        std::cout << "Size: " << client.size(query) << std::endl;
    }


    if ("namespaces" == std::string(argv[1])) {
        if (FLAGS_output != "") {
            std::ofstream out(FLAGS_output);
            client.listNamespaces(out);
        } else {
            client.listNamespaces(std::cout);
        }
    }

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}