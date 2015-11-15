//
// Created by wastl on 18.04.15.
//

#ifndef MARMOTTA_RDF_PARSER_H
#define MARMOTTA_RDF_PARSER_H

#include <string>
#include <functional>

#include <raptor2/raptor2.h>
#include <model/rdf_model.h>

namespace marmotta {
namespace parser {

enum Format {
    RDFXML, TURTLE, NTRIPLES, NQUADS, RDFJSON, RDFA, TRIG, GUESS
};

/**
 * Return the format matching the string name passed as argument.
 */
Format FormatFromString(const std::string& name);

class Parser {
 public:

    Parser(const rdf::URI& baseUri) : Parser(baseUri, Format::GUESS) {};
    Parser(const rdf::URI& baseUri, Format format);

    // TODO: copy and move constructors

    ~Parser();

    void setStatementHandler(std::function<void(const rdf::Statement&)> const &handler) {
        Parser::stmt_handler = handler;
    }

    void setNamespaceHandler(std::function<void(const rdf::Namespace&)> const &handler) {
        Parser::ns_handler = handler;
    }


    void parse(std::istream& in);

 private:
    raptor_parser* parser;
    raptor_world*  world;
    raptor_uri*    base;

    std::function<void(const rdf::Statement&)> stmt_handler;
    std::function<void(const rdf::Namespace&)> ns_handler;

    static void raptor_stmt_handler(void* user_data, raptor_statement* statement);
    static void raptor_ns_handler(void* user_data, raptor_namespace *nspace);
};

class ParseError : std::exception {
 public:
    ParseError(const char* message) : message(message) { }
    ParseError(std::string &message) : message(message) { }

    const std::string &getMessage() const {
        return message;
    }

 private:
    std::string message;
};
}
}

#endif //MARMOTTA_RDF_PARSER_H
