//
// Created by wastl on 18.04.15.
//
#include <raptor2/raptor2.h>
#include "rdf_serializer.h"

static std::map<std::string, rdf::URI> namespacesMap(std::vector<rdf::Namespace> list) {
    std::map<std::string, rdf::URI> result;
    for(auto it=list.cbegin(); it != list.cend(); it++) {
        result[it->getPrefix()]=it->getUri();
    }
    return result;
}

static int std_iostream_write_byte(void *context, const int byte) {
    std::ostream* out = (std::ostream *) context;
    out->write((char const *) &byte, 1);
    if(*out) {
        return 0;
    } else {
        return 1;
    }
}

static int std_iostream_write_bytes(void *context, const void *ptr, size_t size, size_t nmemb) {
    std::ostream* out = (std::ostream *) context;
    out->write((char const *) ptr, size * nmemb);
    if(*out) {
        return 0;
    } else {
        return 1;
    }
}

static int std_iostream_read_bytes(void *context, void *ptr, size_t size, size_t nmemb) {
    std::istream* in = (std::istream *)context;

    if(!*in) {
        return -1;
    }

    in->read((char *) ptr, size*nmemb);
    return (int) in->gcount();
}

static int std_iostream_read_eof(void *context) {
    std::istream* in = (std::istream *)context;

    if(in->eof()) {
        return 1;
    } else {
        return 0;
    }
}

const raptor_iostream_handler raptor_handler = {
        2, NULL, NULL,
        &std_iostream_write_byte, &std_iostream_write_bytes, NULL,
        &std_iostream_read_bytes, &std_iostream_read_eof
};


namespace rdf {
    namespace serializer {
        inline std::string raptorFormat(Format format) {
            switch (format) {
                case Format::RDFXML:
                    return "rdfxml";
                case Format::RDFXML_ABBREV:
                    return "rdfxml-abbrev";
                case Format::GRAPHVIZ:
                    return "dot";
                case Format::NQUADS:
                    return "nquads";
                case Format::NTRIPLES:
                    return "ntriples";
                case Format::TURTLE:
                    return "turtle";
                case Format::RDFJSON:
                    return "json";
                case Format::SPARQL_JSON:
                    return "json-triples";
            }
            return "";
        }

        Serializer::Serializer(const rdf::URI& baseUri, Format format, std::vector<rdf::Namespace> namespaces)
            : Serializer(baseUri, format, namespacesMap(namespaces)) { }

        Serializer::Serializer(const rdf::URI& baseUri, Format format, std::map<std::string, rdf::URI> namespaces)
            : namespaces(namespaces), format(format) {

            world = raptor_new_world();
            base  = raptor_new_uri(world, (unsigned char const *) baseUri.getUri().c_str());
            initRaptor();
        }


        Serializer::~Serializer() {
            // check for NULL in case a move operation has set the fields to a null pointer
            if(serializer != NULL)
                raptor_free_serializer(serializer);

            if(base != NULL)
                raptor_free_uri(base);

            if(world != NULL)
                raptor_free_world(world);

        }

        Serializer::Serializer(const Serializer &other) {
            format = other.format;
            namespaces = other.namespaces;

            world = raptor_new_world();
            base  = raptor_new_uri(world, raptor_uri_as_string(other.base));
            initRaptor();
        }

        Serializer::Serializer(Serializer &&other) {
            format = other.format;
            namespaces = other.namespaces;
            base = other.base;
            world = other.world;
            serializer = other.serializer;

            other.serializer = NULL;
            other.base = NULL;
            other.world = NULL;
        }

        Serializer &Serializer::operator=(const Serializer &other) {
            format = other.format;
            namespaces = other.namespaces;

            world = raptor_new_world();
            base  = raptor_new_uri(world, raptor_uri_as_string(other.base));
            initRaptor();

            return *this;
        }

        Serializer &Serializer::operator=(Serializer &&other) {
            format = other.format;
            namespaces = other.namespaces;
            serializer = other.serializer;
            base = other.base;
            world = other.world;

            other.serializer = NULL;
            other.base = NULL;
            other.world = NULL;

            return *this;
        }


        void Serializer::initRaptor() {
            serializer = raptor_new_serializer(world, raptorFormat(format).c_str());
            for(std::pair<std::string, rdf::URI> e : namespaces) {
                raptor_uri* uri = raptor_new_uri(world, (unsigned char const *) e.second.getUri().c_str());
                raptor_serializer_set_namespace(serializer, uri, (unsigned char const *) e.first.c_str());
            }
            raptor_world_set_log_handler(world, this, [](void *user_data, raptor_log_message* message){
                std::cerr << message->level << ": " << message->text << std::endl;
            });
        }

        void Serializer::serialize(const Statement &stmt, raptor_iostream *stream) {
            raptor_statement* triple = raptor_new_statement(world);

            switch (stmt.getSubject().type) {
                case rdf::Resource::URI:
                    triple->subject = raptor_new_term_from_uri_string(world,  (unsigned char const *) stmt.getSubject().uri.getUri().c_str());
                    break;
                case rdf::Resource::BNODE:
                    triple->subject = raptor_new_term_from_blank(world, (unsigned char const *) stmt.getSubject().bnode.getId().c_str());
                    break;
                default:
                    throw SerializationError("invalid subject type: " + stmt.getSubject().type);
            }

            triple->predicate = raptor_new_term_from_uri_string(world,  (unsigned char const *) stmt.getPredicate().getUri().c_str());

            switch (stmt.getObject().type) {
                case rdf::Value::URI:
                    triple->object = raptor_new_term_from_uri_string(world,  (unsigned char const *) stmt.getObject().uri.getUri().c_str());
                    break;
                case rdf::Value::BNODE:
                    triple->object = raptor_new_term_from_blank(world, (unsigned char const *) stmt.getObject().bnode.getId().c_str());
                    break;
                case rdf::Value::STRING_LITERAL:
                    triple->object = raptor_new_term_from_counted_literal(
                            world,
                            (unsigned char const *) stmt.getObject().sliteral.getContent().c_str(), stmt.getObject().sliteral.getContent().size(), NULL,
                            (unsigned char const *) stmt.getObject().sliteral.getLanguage().c_str(), stmt.getObject().sliteral.getLanguage().size());
                    break;
                case rdf::Value::DATATYPE_LITERAL:
                    triple->object = raptor_new_term_from_counted_literal(
                            world,
                            (unsigned char const *) stmt.getObject().tliteral.getContent().c_str(), stmt.getObject().tliteral.getContent().size(),
                            raptor_new_uri(world, (unsigned char const *) stmt.getObject().tliteral.getDatatype().getUri().c_str()),
                            (unsigned char const *) "", 0);
                    break;
                default:
                    throw SerializationError("invalid object type: " + stmt.getObject().type);
            }


            switch (stmt.getContext().type) {
                case rdf::Resource::URI:
                    triple->graph = raptor_new_term_from_uri_string(world,  (unsigned char const *) stmt.getContext().uri.getUri().c_str());
                    break;
                case rdf::Resource::BNODE:
                    triple->graph = raptor_new_term_from_blank(world, (unsigned char const *) stmt.getContext().bnode.getId().c_str());
                    break;
                default:
                    throw SerializationError("invalid context type: " + stmt.getContext().type);
            }

            raptor_serializer_serialize_statement(serializer, triple);

            raptor_free_statement(triple);
        }

        raptor_iostream *Serializer::initIOStream(std::ostream &out) {
            return raptor_new_iostream_from_handler(world, &out, &raptor_handler);
        }

        void Serializer::closeIOStream(raptor_iostream *stream) {
            raptor_free_iostream(stream);
        }
    }
}
