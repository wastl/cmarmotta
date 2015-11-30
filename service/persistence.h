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
#include "service/sail.pb.h"

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

// Interface for an STL iterator yielding elements of parameterized types.
template <class Proto>
class Iterator {
 public:
    virtual Iterator<Proto>& operator++() = 0;
    virtual Proto& operator*() = 0;
    virtual Proto* operator->() = 0;
    virtual bool operator==(const Iterator<Proto>& other) = 0;
    virtual bool operator!=(const Iterator<Proto>& other) = 0;
};

// Statistical data about updates.
struct UpdateStatistics {
    UpdateStatistics()
            : added_stmts(0), removed_stmts(0), added_ns(0), removed_ns(0) {}

    int64_t added_stmts, removed_stmts, added_ns, removed_ns;
};

/**
 * Persistence implementation based on the LevelDB high performance database.
 */
class LevelDBPersistence {
 public:
    typedef Iterator<rdf::proto::Statement> StatementIterator;
    typedef Iterator<rdf::proto::Namespace> NamespaceIterator;
    typedef Iterator<service::proto::UpdateRequest> UpdateIterator;

    typedef std::function<void(const rdf::proto::Statement&)> StatementHandler;
    typedef std::function<void(const rdf::proto::Namespace&)> NamespaceHandler;


    /**
     * Initialise a new LevelDB database using the given path and cache size (bytes).
     */
    LevelDBPersistence(const std::string& path, int64_t cacheSize);

    /**
     * Add the namespaces between begin and end to the database.
     */
    int64_t AddNamespaces(NamespaceIterator& begin, const NamespaceIterator& end);

    /**
     * Add the statements between begin and end to the database.
     */
    int64_t AddStatements(StatementIterator& begin, const StatementIterator& end);

    /**
     * Get all statements matching the pattern (which may have some fields
     * unset to indicate wildcards). Call the callback function for each
     * result.
     */
    void GetStatements(const rdf::proto::Statement& pattern,
                       StatementHandler callback);

    /**
     * Get all namespaces matching the pattern (which may have some of all
     * fields unset to indicate wildcards). Call the callback function for
     * each result.
     */
    void GetNamespaces(const rdf::proto::Namespace &pattern,
                       NamespaceHandler callback);

    /**
     * Remove all statements matching the pattern (which may have some fields
     * unset to indicate wildcards).
     */
    int64_t RemoveStatements(const rdf::proto::Statement& pattern);

    /**
     * Apply a batch of updates (mixed statement/namespace adds and removes).
     * The updates are collected in LevelDB batches and written atomically to
     * the database when iteration ends.
     */
    UpdateStatistics Update(UpdateIterator& begin, const UpdateIterator& end);

 private:
    int key_length;

    std::unique_ptr<KeyComparator> comparator;
    std::unique_ptr<leveldb::Cache> cache;
    std::unique_ptr<leveldb::Options> options;

    // We currently support efficient lookups by subject, context and object.
    std::unique_ptr<leveldb::DB> db_spoc, db_cspo, db_opsc, db_pcos, db_ns_prefix, db_ns_url;

    /**
     * Add the namespace to the given database batch operations.
     */
    void AddNamespace(const rdf::proto::Namespace& ns,
                      leveldb::WriteBatch& ns_prefix, leveldb::WriteBatch& ns_url);

    /**
     * Add the namespace to the given database batch operations.
     */
    void RemoveNamespace(const rdf::proto::Namespace& ns,
                         leveldb::WriteBatch& ns_prefix, leveldb::WriteBatch& ns_url);

    /**
     * Add the statement to the given database batch operations.
     */
    void AddStatement(const rdf::proto::Statement& stmt,
                      leveldb::WriteBatch& spoc, leveldb::WriteBatch& cspo,
                      leveldb::WriteBatch& opsc, leveldb::WriteBatch&pcos);


    /**
     * Remove all statements matching the pattern (which may have some fields
     * unset to indicate wildcards) from the given database batch operations.
     */
    int64_t RemoveStatements(const rdf::proto::Statement& pattern,
                             leveldb::WriteBatch& spoc, leveldb::WriteBatch& cspo,
                             leveldb::WriteBatch& opsc, leveldb::WriteBatch&pcos);
};



}  // namespace persistence
}  // namespace marmotta

#endif //MARMOTTA_PERSISTENCE_H
