//
// Created by wastl on 18.04.15.
//

#ifndef MARMOTTA_RDF_SERIALIZER_H
#define MARMOTTA_RDF_SERIALIZER_H

#include <string>
#include <map>
#include <memory>
#include <vector>

#include <model/rdf_model.h>
#include <raptor2/raptor2.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

namespace marmotta {
namespace serializer {

enum Format {
    RDFXML, RDFXML_ABBREV, TURTLE, NTRIPLES, NQUADS, RDFJSON, SPARQL_JSON, GRAPHVIZ, PROTO, PROTO_TEXT
};


/**
 * Return the format matching the string name passed as argument.
 */
Format FormatFromString(const std::string& name);

/**
 * Serialize statements in various RDF text formats. This class and its subclasses are not thread safe.
 */
class SerializerBase {
 public:
    SerializerBase(const rdf::URI& baseUri, Format format)
            : SerializerBase(baseUri, format, std::map<std::string, rdf::URI>()) {};
    SerializerBase(const rdf::URI& baseUri, Format format, std::vector<rdf::Namespace> namespaces);
    SerializerBase(const rdf::URI& baseUri, Format format, std::map<std::string, rdf::URI> namespaces);

    virtual ~SerializerBase() {};

    void serialize(const rdf::Statement& stmt, std::ostream& out) {
        prepare(out);
        serialize(stmt);
        close();
    };

    template <typename Iterator>
    void serialize(Iterator begin, Iterator end, std::ostream& out) {
        prepare(out);
        for(auto it=begin; it != end; ++it) {
            serialize(*it);
        }
        close();
    };

 protected:
    rdf::URI baseUri;
    Format format;
    std::map<std::string, rdf::URI> namespaces;

    virtual void prepare(std::ostream& out) = 0;
    virtual void serialize(const rdf::Statement& stmt) = 0;
    virtual void close() = 0;
};


class RaptorSerializer : public SerializerBase {
 public:
    RaptorSerializer(const rdf::URI& baseUri, Format format);
    RaptorSerializer(const rdf::URI& baseUri, Format format, std::vector<rdf::Namespace> namespaces);
    RaptorSerializer(const rdf::URI& baseUri, Format format, std::map<std::string, rdf::URI> namespaces);
    ~RaptorSerializer() override;

 private:
    raptor_serializer* serializer;
    raptor_world*      world;
    raptor_uri*        base;
    raptor_iostream*   stream;

    void prepare(std::ostream& out) override;
    void serialize(const rdf::Statement& stmt) override;
    void close() override;

    void initRaptor();
};

class SerializationError : std::exception {
 public:
    SerializationError(const char* message) : message(message) { }
    SerializationError(std::string &message) : message(message) { }

    const std::string &getMessage() const {
        return message;
    }

 private:
    std::string message;
};


/**
 * Serialize statements as binary proto wire format according to model.proto.
 */
class ProtoSerializer : public SerializerBase {
 public:
    ProtoSerializer(const rdf::URI& baseUri, Format format)
            : ProtoSerializer(baseUri, format, std::map<std::string, rdf::URI>()) {};
    ProtoSerializer(const rdf::URI& baseUri, Format format, std::vector<rdf::Namespace> namespaces)
            : SerializerBase(baseUri, format, namespaces) {};
    ProtoSerializer(const rdf::URI& baseUri, Format format, std::map<std::string, rdf::URI> namespaces)
            : SerializerBase(baseUri, format, namespaces) {};

 private:
    void prepare(std::ostream& out) override;
    void serialize(const rdf::Statement& stmt) override;
    void close() override;

    google::protobuf::io::OstreamOutputStream* out_;
    marmotta::rdf::proto::Statements stmts_;
};


class Serializer {
 public:
    Serializer(const rdf::URI& baseUri, Format format)
            : Serializer(baseUri, format, std::map<std::string, rdf::URI>()) {};
    Serializer(const rdf::URI& baseUri, Format format, std::vector<rdf::Namespace> namespaces);
    Serializer(const rdf::URI& baseUri, Format format, std::map<std::string, rdf::URI> namespaces);

    ~Serializer() {};

    void serialize(const rdf::Statement& stmt, std::ostream& out) {
        impl->serialize(stmt, out);
    };

    template <typename Iterator>
    void serialize(Iterator begin, Iterator end, std::ostream& out) {
        impl->serialize(begin, end, out);
    };

 private:
    std::unique_ptr<SerializerBase> impl;
};

};

}

#endif //MARMOTTA_RDF_SERIALIZER_H
