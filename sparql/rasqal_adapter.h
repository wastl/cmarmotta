//
// Created by wastl on 09.12.15.
//

#ifndef MARMOTTA_RASQAL_ADAPTER_H
#define MARMOTTA_RASQAL_ADAPTER_H

#include "sparql/rasqal_model.h"

namespace marmotta {
namespace sparql {

/**
 * An interface for iterators over statements. Needs to be implemented by concrete
 * subclasses of TripleSource.
 */
class StatementIterator {
 public:

    virtual ~StatementIterator() {}

    /**
     * Increment iterator to next statement.
     */
    virtual StatementIterator& operator++() = 0;

    /**
     * Dereference iterator, returning a reference to the current statement.
     */
    virtual rdf::Statement& operator*() = 0;

    /**
     * Dereference iterator, returning a pointer to the current statement.
     */
    virtual rdf::Statement* operator->() = 0;

    /**
     * Return true in case the iterator has more elements.
     */
    virtual bool hasNext() = 0;
};

/**
 * An abstract superclass for more easily interfacing from the C++ Marmotta model
 * with C-based Rasqal.
 */
class TripleSource {
 public:

    /**
     * Check for presence of a complete statement.
     *
     * Parameters with nullptr value are interpreted as wildcards.
     */
    virtual bool HasStatement(
            const rdf::Resource* s, const rdf::URI* p, const rdf::Value* o,
            const rdf::Resource* c) = 0;

    /**
     * Return an iterator over statements matching the given subject, predicate,
     * object and context. The caller takes ownership of the pointer.
     *
     * Parameters with nullptr value are interpreted as wildcards.
     */
    virtual std::unique_ptr<StatementIterator> GetStatements(
            const rdf::Resource* s, const rdf::URI* p, const rdf::Value* o,
            const rdf::Resource* c) = 0;
};

/**
 * Class SparqlService provides a SPARQL wrapper around a triple source using
 * Rasqal.
 */
class SparqlService {
 public:
    SparqlService(std::unique_ptr<TripleSource> source);

    /**
     * Free any C-style resources, particularly the rasqal world.
     */
    ~SparqlService();

    /**
     * Return a reference to the triple source managed by this service.
     */
    TripleSource& Source() {
        return *source;
    }

 private:
    std::unique_ptr<TripleSource> source;

    rasqal_world* world;
};

}  // namespace sparql
}  // namespace marmotta


#endif //MARMOTTA_RASQAL_ADAPTER_H
