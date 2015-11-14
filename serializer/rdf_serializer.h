//
// Created by wastl on 18.04.15.
//

#ifndef MARMOTTA_RDF_SERIALIZER_H
#define MARMOTTA_RDF_SERIALIZER_H

#include <string>
#include <map>
#include <vector>

#include <model/rdf_model.h>
#include <raptor2/raptor2.h>

namespace marmotta {
    namespace serializer {

        enum Format {
            RDFXML, RDFXML_ABBREV, TURTLE, NTRIPLES, NQUADS, RDFJSON, SPARQL_JSON, GRAPHVIZ
        };

        class Serializer {
        public:
            Serializer(const rdf::URI& baseUri, Format format)
                    : Serializer(baseUri, format, std::map<std::string, rdf::URI>()) {};
            Serializer(const rdf::URI& baseUri, Format format, std::vector<rdf::Namespace> namespaces);
            Serializer(const rdf::URI& baseUri, Format format, std::map<std::string, rdf::URI> namespaces);

            ~Serializer();

            Serializer(const Serializer& other);
            Serializer(Serializer&& other);

            Serializer& operator=(const Serializer& other);
            Serializer& operator=(Serializer&& other);



            void serialize(const rdf::Statement& stmt, std::ostream& out) {
                raptor_iostream* s = initIOStream(out);
                raptor_serializer_start_to_iostream(serializer, base, s);
                serialize(stmt, s);
                raptor_serializer_serialize_end(serializer);
                closeIOStream(s);
            }

            template <typename Iterator>
            void serialize(Iterator begin, Iterator end, std::ostream& out) {
                raptor_iostream* s = initIOStream(out);
                raptor_serializer_start_to_iostream(serializer, base, s);
                for(auto it=begin; it != end; it++) {
                    serialize(*it, s);
                }
                raptor_serializer_serialize_end(serializer);
                closeIOStream(s);
            }

        private:
            raptor_serializer* serializer;
            raptor_world*      world;
            raptor_uri*        base;

            Format format;
            std::map<std::string, rdf::URI> namespaces;

            void serialize(const rdf::Statement& stmt, raptor_iostream* stream);

            void initRaptor();
            raptor_iostream* initIOStream(std::ostream &out);
            void closeIOStream(raptor_iostream* stream);
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

    }

}

#endif //MARMOTTA_RDF_SERIALIZER_H
