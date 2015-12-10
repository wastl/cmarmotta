//
// Created by wastl on 10.12.15.
//

#ifndef MARMOTTA_SPARQL_H
#define MARMOTTA_SPARQL_H

#include "sparql/rasqal_adapter.h"
#include "service/persistence.h"

namespace marmotta {
namespace persistence {
namespace sparql {

class LevelDBTripleSource : public ::marmotta::sparql::TripleSource {
 public:

    LevelDBTripleSource(LevelDBPersistence *persistence) : persistence(persistence) { }


    bool HasStatement(const rdf::Resource *s, const rdf::URI *p, const rdf::Value *o, const rdf::Resource *c) override;

    std::unique_ptr<::marmotta::sparql::StatementIterator>
            GetStatements(const rdf::Resource *s, const rdf::URI *p, const rdf::Value *o, const rdf::Resource *c) override;

 private:
    // A pointer to the persistence instance wrapped by this triple source.
    LevelDBPersistence* persistence;
};


}
}
}

#endif //MARMOTTA_SPARQL_H
