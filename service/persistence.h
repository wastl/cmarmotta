//
// Created by wastl on 15.11.15.
//

#ifndef MARMOTTA_PERSISTENCE_H
#define MARMOTTA_PERSISTENCE_H

#include <memory>
#include <string>
#include <functional>

#include <leveldb/db.h>
#include <leveldb/cache.h>
#include <leveldb/comparator.h>

#include "model/rdf_model.h"

namespace marmotta {
namespace persistence {

/**
 * A custom comparator treating the bytes in the key as unsigned char.
 */
class KeyComparator : public leveldb::Comparator {
 public:
    int Compare(const leveldb::Slice& a, const leveldb::Slice& b) const;

    const char* Name() const { return "KeyComparator"; }
    void FindShortestSeparator(std::string*, const leveldb::Slice&) const { }
    void FindShortSuccessor(std::string*) const { }
};

// A STL iterator wrapper around a client reader.
template <class Proto>
class Iterator {
 public:
    virtual Iterator<Proto>& operator++() = 0;
    virtual Proto& operator*() = 0;
    virtual Proto* operator->() = 0;
    virtual bool operator==(const Iterator<Proto>& other) = 0;
    virtual bool operator!=(const Iterator<Proto>& other) = 0;
};

typedef Iterator<rdf::proto::Statement> StatementIterator;
typedef Iterator<rdf::proto::Namespace> NamespaceIterator;

class LevelDBPersistence {
 public:
    LevelDBPersistence(const std::string& path, int64_t cacheSize);

    int64_t AddNamespaces(NamespaceIterator& begin, const NamespaceIterator& end);

    int64_t AddStatements(StatementIterator& begin, const StatementIterator& end);

    void GetStatements(const rdf::proto::Statement& pattern, std::function<void(const rdf::proto::Statement&)> callback);

    void GetNamespaces(const rdf::proto::Namespace& pattern, std::function<void(const rdf::proto::Namespace&)> callback);

    int64_t RemoveStatements(const rdf::proto::Statement& pattern);

 private:
    std::unique_ptr<KeyComparator> comparator;
    std::unique_ptr<leveldb::Cache> cache;
    std::unique_ptr<leveldb::Options> options;

    // We currently support efficient lookups by subject, context and object.
    std::unique_ptr<leveldb::DB> db_spoc, db_cspo, db_opsc, db_cops, db_ns_prefix, db_ns_url;
};



}  // namespace persistence
}  // namespace marmotta

#endif //MARMOTTA_PERSISTENCE_H
