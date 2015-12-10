//
// Created by wastl on 10.12.15.
//

#include "sparql.h"

namespace marmotta {
namespace persistence {
namespace sparql {

using ::marmotta::sparql::StatementIterator;

class WrapProtoStatementIterator : public StatementIterator {

 public:
    WrapProtoStatementIterator(std::unique_ptr<persistence::LevelDBPersistence::StatementIterator> it)
            : it(std::move(it)) { }

    util::CloseableIterator<rdf::Statement> &operator++() override {
        ++(*it);
        parsed = false;
        return *this;
    };

    rdf::Statement &operator*() override {
        if (!parsed) {
            current = std::move(**it);
            parsed = true;
        }
        return current;
    };

    rdf::Statement *operator->() override {
        if (!parsed) {
            current = std::move(**it);
            parsed = true;
        }
        return &current;
    };

    bool hasNext() override {
        return it->hasNext();
    }

 private:
    std::unique_ptr<persistence::LevelDBPersistence::StatementIterator> it;
    rdf::Statement current;
    bool parsed;
};


bool LevelDBTripleSource::HasStatement(
        const rdf::Resource *s, const rdf::URI *p, const rdf::Value *o, const rdf::Resource *c) {
    rdf::proto::Statement pattern;

    if (s != nullptr) {
        *pattern.mutable_subject() = s->getMessage();
    }
    if (p != nullptr) {
        *pattern.mutable_predicate() = p->getMessage();
    }
    if (o != nullptr) {
        *pattern.mutable_object() = o->getMessage();
    }
    if (c != nullptr) {
        *pattern.mutable_context() = c->getMessage();
    }

    bool found = false;
    persistence->GetStatements(pattern, [&found](rdf::proto::Statement) -> bool {
        found = true;
        return false;
    });

    return found;
}

std::unique_ptr<sparql::StatementIterator> LevelDBTripleSource::GetStatements(
        const rdf::Resource *s, const rdf::URI *p, const rdf::Value *o, const rdf::Resource *c) {
    rdf::proto::Statement pattern;

    if (s != nullptr) {
        *pattern.mutable_subject() = s->getMessage();
    }
    if (p != nullptr) {
        *pattern.mutable_predicate() = p->getMessage();
    }
    if (o != nullptr) {
        *pattern.mutable_object() = o->getMessage();
    }
    if (c != nullptr) {
        *pattern.mutable_context() = c->getMessage();
    }

    return std::unique_ptr<sparql::StatementIterator>(
            new WrapProtoStatementIterator(persistence->GetStatements(pattern)));
}

}  // namespace sparql
}  // namespace persistence
}  // namespace marmotta