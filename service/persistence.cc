//
// Created by wastl on 15.11.15.
//

#include "persistence.h"
#include "leveldb/write_batch.h"
#include "model/rdf_operators.h"

#include <grpc++/support/sync_stream.h>

using grpc::Status;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReader;
using grpc::ServerWriter;
using google::protobuf::Int64Value;
using google::protobuf::Message;
using marmotta::rdf::proto::Statement;
using marmotta::rdf::proto::Namespace;

namespace marmotta {
namespace persistence {

static std::hash<std::string> g_hash_fn;

/**
* Encode a 64bit integer in the first 8 bytes of the buffer.
*/
void encodeInt(char* buffer, size_t data) {
    buffer[0] = (char)((data >> 56) & 0xFF);
    buffer[1] = (char)((data >> 48) & 0xFF);
    buffer[2] = (char)((data >> 40) & 0xFF);
    buffer[3] = (char)((data >> 32) & 0xFF);
    buffer[4] = (char)((data >> 24) & 0xFF);
    buffer[5] = (char)((data >> 16) & 0xFF);
    buffer[6] = (char)((data >> 8) & 0xFF);
    buffer[7] = (char)(data & 0xFF);
}

// Creates an index key based on hashing values of the 4 messages in proper order.
void computeKey(const std::string* a, const std::string* b, const std::string* c, const std::string* d, char* result) {
    int offset = 0;
    for (auto m : {a, b, c, d}) {
        if (m != nullptr) {
            size_t h = g_hash_fn(*m);
            encodeInt(&result[offset], h);
        } else {
            return;
        }
        offset += 8;
    }
    return;
}

/**
 * Helper class to define proper cache keys and identify the index to use based on
 * fields available in the pattern.
 */
class PatternQuery {
 public:
    enum IndexType {
        SPOC, CSPO, OPSC, COPS
    };

    PatternQuery(const Statement& pattern) : pattern(pattern) {
        if (pattern.has_subject()) {
            s.reset(new std::string());
            pattern.subject().SerializeToString(s.get());
        }
        if (pattern.has_predicate()) {
            p.reset(new std::string());
            pattern.predicate().SerializeToString(p.get());
        }
        if (pattern.has_object()) {
            o.reset(new std::string());
            pattern.object().SerializeToString(o.get());
        }
        if (pattern.has_context()) {
            c.reset(new std::string());
            pattern.context().SerializeToString(c.get());
        }

        if (pattern.has_context()) {
            if (pattern.has_object()) {
                type_ = COPS;
            }
            type_ = CSPO;
        } else if (pattern.has_object()) {
            type_ = OPSC;
        }
        type_ = SPOC;
    }

    /**
     * Return the lower key for querying the index (range [MinKey,MaxKey) ).
     */
    char* MinKey() const {
        char* result = (char*)calloc(32, sizeof(char));
        compute(result);
        return result;
    }

    /**
     * Return the upper key for querying the index (range [MinKey,MaxKey) ).
     */
    char* MaxKey() const {
        char* result = (char*)malloc(32 * sizeof(char));
        for (int i=0; i<32; i++) {
            result[i] = (char)0xFF;
        }

        compute(result);
        return result;
    }

    IndexType Type() const {
        return type_;
    }

    PatternQuery& Type(IndexType t) {
        type_ = t;
        return *this;
    }

 private:
    const Statement& pattern;
    std::unique_ptr<std::string> s, p, o, c;

    // Creates a cache key based on hashing values of the 4 messages in proper order.
    void compute(char* result) const {
        switch(Type()) {
            case SPOC:
                computeKey(s.get(), p.get(), o.get(), c.get(), result);
                break;
            case CSPO:
                computeKey(c.get(), s.get(), p.get(), o.get(), result);
                break;
            case OPSC:
                computeKey(o.get(), p.get(), s.get(), c.get(), result);
                break;
            case COPS:
                computeKey(c.get(), o.get(), p.get(), s.get(), result);
                break;
        }
    }

    IndexType type_;
};

/**
 * Check if a statement matches with a partial pattern.
 */
bool matches(const Statement& stmt, const Statement& pattern) {
    // equality operators defined in rdf_model.h
    if (pattern.has_context() && stmt.context() != pattern.context()) {
        return false;
    }
    if (pattern.has_subject() && stmt.subject() != pattern.subject()) {
        return false;
    }
    if (pattern.has_predicate() && stmt.predicate() != pattern.predicate()) {
        return false;
    }
    if (pattern.has_object() && stmt.object() != pattern.object()) {
        return false;
    }
    return true;
}


/**
 * Build database with default options.
 */
leveldb::DB* buildDB(const std::string& path, const std::string& suffix, const leveldb::Options& options) {
    leveldb::DB* db;
    leveldb::Status status = leveldb::DB::Open(options, path + "_" + suffix + ".db", &db);
    assert(status.ok());
    return db;
}

leveldb::Options* buildOptions(KeyComparator* cmp, leveldb::Cache* cache) {
    leveldb::Options *options = new leveldb::Options();
    options->create_if_missing = true;
    options->comparator = cmp;
    options->block_cache = cache;
    return options;
}

LevelDBService::LevelDBService(const std::string &path, int64_t cacheSize)
        : comparator(new KeyComparator())
        , cache(leveldb::NewLRUCache(cacheSize))
        , options(buildOptions(comparator.get(), cache.get()))
        , db_spoc(buildDB(path, "spoc", *options)), db_cspo(buildDB(path, "cspo", *options))
        , db_opsc(buildDB(path, "opsc", *options)), db_cops(buildDB(path, "cops", *options))
        , db_ns_prefix(buildDB(path, "ns_prefix", *options)), db_ns_url(buildDB(path, "ns_url", *options)) { }

Status LevelDBService::AddNamespaces(
        ServerContext* context, ServerReader<Namespace>* reader, Int64Value* result) {

    int64_t count = 0;

    rdf::proto::Namespace ns;

    leveldb::WriteBatch batch_prefix, batch_url;
    std::string buffer;
    while (reader->Read(&ns)) {
        ns.SerializeToString(&buffer);
        batch_prefix.Put(ns.prefix(), buffer);
        batch_url.Put(ns.uri(), buffer);
        count++;
    }
    db_ns_prefix->Write(leveldb::WriteOptions(), &batch_prefix);
    db_ns_url->Write(leveldb::WriteOptions(), &batch_url);

    result->set_value(count);

    return Status::OK;
}

Status LevelDBService::AddStatements(
        ServerContext* context, ServerReader<Statement>* reader, Int64Value* result) {

    int64_t count = 0;

    rdf::proto::Statement stmt;

    leveldb::WriteBatch batch_spoc, batch_cspo, batch_opsc, batch_cops;
    std::string buffer, bufs, bufp, bufo, bufc;
    while (reader->Read(&stmt)) {
        stmt.SerializeToString(&buffer);

        stmt.subject().SerializeToString(&bufs);
        stmt.predicate().SerializeToString(&bufp);
        stmt.object().SerializeToString(&bufo);
        stmt.context().SerializeToString(&bufc);

        char* k_spoc = (char*)calloc(32, sizeof(char));
        computeKey(&bufs, &bufp, &bufo, &bufc, k_spoc);
        batch_spoc.Put(leveldb::Slice(k_spoc, 32), buffer);
        free(k_spoc);

        char* k_cspo = (char*)calloc(32, sizeof(char));
        computeKey(&bufc, &bufs, &bufp, &bufo, k_cspo);
        batch_cspo.Put(leveldb::Slice(k_cspo, 32), buffer);
        free(k_cspo);

        char* k_opsc = (char*)calloc(32, sizeof(char));
        computeKey(&bufo, &bufp, &bufs, &bufc, k_opsc);
        batch_opsc.Put(leveldb::Slice(k_opsc, 32), buffer);
        free(k_opsc);

        char* k_cops = (char*)calloc(32, sizeof(char));
        computeKey(&bufc, &bufo, &bufp, &bufs, k_cops);
        batch_cops.Put(leveldb::Slice(k_cops, 32), buffer);
        free(k_cops);

        count++;
    }
    db_cops->Write(leveldb::WriteOptions(), &batch_cops);
    db_opsc->Write(leveldb::WriteOptions(), &batch_opsc);
    db_cspo->Write(leveldb::WriteOptions(), &batch_cspo);
    db_spoc->Write(leveldb::WriteOptions(), &batch_spoc);

    result->set_value(count);

    return Status::OK;
}

Status LevelDBService::GetStatements(
        ServerContext* context, const Statement* pattern, ServerWriter<Statement>* result) {

    PatternQuery query(*pattern);


    leveldb::DB* db;
    char *loKey = query.MinKey();
    char *hiKey = query.MaxKey();

    switch (query.Type()) {
        case PatternQuery::SPOC:
            db = db_spoc.get();
            break;
        case PatternQuery::CSPO:
            db = db_cspo.get();
            break;
        case PatternQuery::OPSC:
            db = db_opsc.get();
            break;
        case PatternQuery::COPS:
            db = db_cops.get();
            break;
    };

    Statement stmt;
    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for (it->Seek(leveldb::Slice(loKey, 32));
         it->Valid() && it->key().compare(leveldb::Slice(hiKey, 32)) < 0;
         it->Next()) {
        stmt.ParseFromString(it->value().ToString());
        if (matches(stmt, *pattern)) {
            result->Write(stmt);
        }
    }

    free(loKey);
    free(hiKey);

    return Status::OK;
}

Status LevelDBService::RemoveStatements(
        ServerContext* context, const Statement* pattern, Int64Value* result) {

    PatternQuery query(*pattern);


    leveldb::DB* db;
    char *loKey = query.MinKey();
    char *hiKey = query.MaxKey();

    switch (query.Type()) {
        case PatternQuery::SPOC:
            db = db_spoc.get();
            break;
        case PatternQuery::CSPO:
            db = db_cspo.get();
            break;
        case PatternQuery::OPSC:
            db = db_opsc.get();
            break;
        case PatternQuery::COPS:
            db = db_cops.get();
            break;
    };

    int64_t count = 0;

    Statement stmt;
    leveldb::WriteBatch batch_spoc, batch_cspo, batch_opsc, batch_cops;
    std::string bufs, bufp, bufo, bufc;

    leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
    for (it->Seek(leveldb::Slice(loKey, 32));
         it->Valid() && it->key().compare(leveldb::Slice(hiKey, 32)) < 0;
         it->Next()) {
        stmt.ParseFromString(it->value().ToString());
        if (matches(stmt, *pattern)) {
            stmt.subject().SerializeToString(&bufs);
            stmt.predicate().SerializeToString(&bufp);
            stmt.object().SerializeToString(&bufo);
            stmt.context().SerializeToString(&bufc);

            char* k_spoc = (char*)calloc(32, sizeof(char));
            computeKey(&bufs, &bufp, &bufo, &bufc, k_spoc);
            batch_spoc.Delete(leveldb::Slice(k_spoc, 32));
            free(k_spoc);

            char* k_cspo = (char*)calloc(32, sizeof(char));
            computeKey(&bufc, &bufs, &bufp, &bufo, k_cspo);
            batch_cspo.Delete(leveldb::Slice(k_cspo, 32));
            free(k_cspo);

            char* k_opsc = (char*)calloc(32, sizeof(char));
            computeKey(&bufo, &bufp, &bufs, &bufc, k_opsc);
            batch_opsc.Delete(leveldb::Slice(k_opsc, 32));
            free(k_opsc);

            char* k_cops = (char*)calloc(32, sizeof(char));
            computeKey(&bufc, &bufo, &bufp, &bufs, k_cops);
            batch_cops.Delete(leveldb::Slice(k_cops, 32));
            free(k_cops);

            count++;
        }
    }
    db_cops->Write(leveldb::WriteOptions(), &batch_cops);
    db_opsc->Write(leveldb::WriteOptions(), &batch_opsc);
    db_cspo->Write(leveldb::WriteOptions(), &batch_cspo);
    db_spoc->Write(leveldb::WriteOptions(), &batch_spoc);

    result->set_value(count);

    free(loKey);
    free(hiKey);

    return Status::OK;
}


int KeyComparator::Compare(const leveldb::Slice& a, const leveldb::Slice& b) const {
    for (int i=0; i<32; i++) {
        unsigned char ac = (unsigned char)a.data()[i];
        unsigned char bc = (unsigned char)b.data()[i];
        if (ac < bc) {
            return -1;
        } else if (ac > bc) {
            return 1;
        }
    }
    return 0;
}
}  // namespace persistence
}  // namespace marmotta


