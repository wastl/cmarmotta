#include <fstream>
#include <vector>
#include "model/rdf_model.h"
#include "parser/rdf_parser.h"
#include "serializer/rdf_serializer.h"

using namespace std;


int main(int argc, const char** argv) {

    std::ifstream in(argv[1]);

    std::vector<rdf::Statement> statements;

    rdf::parser::Parser p("http://www.example.com", rdf::parser::Format::RDFXML);
    p.setStatementHandler([&statements](const rdf::Statement& stmt) {
        statements.emplace_back(stmt);
    });
    p.parse(in);

    rdf::serializer::Serializer s("http://www.example.com", rdf::serializer::Format::TURTLE);
    s.serialize(statements.cbegin(), statements.cend(), std::cout);

    return 0;
}