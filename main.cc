#include <fstream>
#include <vector>
#include "model/rdf_model.h"
#include "parser/rdf_parser.h"
#include "serializer/rdf_serializer.h"

using namespace std;
using namespace marmotta;

int main(int argc, const char** argv) {

    std::ifstream in(argv[1]);
    std::ofstream out(argv[2]);

    std::vector<rdf::Statement> statements;

    parser::Parser p("http://www.example.com", parser::Format::RDFXML);
    p.setStatementHandler([&statements](const rdf::Statement& stmt) {
        statements.emplace_back(stmt);
    });
    p.parse(in);

    std::cout << "Statements: " << statements.size() << std::endl;

    serializer::Serializer s("http://www.example.com", serializer::Format::PROTO);
    s.serialize(statements.cbegin(), statements.cend(), out);

    return 0;
}