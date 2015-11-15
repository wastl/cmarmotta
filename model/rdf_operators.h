//
// Created by wastl on 15.11.15.
//

#ifndef MARMOTTA_RDF_OPERATORS_H
#define MARMOTTA_RDF_OPERATORS_H

#include "model/rdf_model.h"

namespace marmotta {
namespace rdf {
namespace proto {

inline bool operator==(const Namespace &lhs, const Namespace &rhs) {
    return lhs.uri() == rhs.uri();
}

inline bool operator!=(const Namespace &lhs, const Namespace &rhs) {
    return lhs.uri() != rhs.uri();
}

inline bool operator==(const URI &lhs, const URI &rhs) {
    return lhs.uri() == rhs.uri();
}

inline bool operator!=(const URI &lhs, const URI &rhs) {
    return lhs.uri() != rhs.uri();
}

inline bool operator==(const BNode &lhs, const BNode &rhs) {
    return lhs.id() == rhs.id();
}

inline bool operator!=(const BNode &lhs, const BNode &rhs) {
    return lhs.id() != rhs.id();
}


inline bool operator==(const StringLiteral &lhs, const StringLiteral &rhs) {
    return lhs.content() == rhs.content() && lhs.language() == rhs.language();
}

inline bool operator!=(const StringLiteral &lhs, const StringLiteral &rhs) {
    return lhs.content() != rhs.content() || lhs.language() != rhs.language();
}

inline bool operator==(const DatatypeLiteral &lhs, const DatatypeLiteral &rhs) {
    return lhs.content() == rhs.content() && lhs.datatype().uri() == rhs.datatype().uri();
}

inline bool operator!=(const DatatypeLiteral &lhs, const DatatypeLiteral &rhs) {
    return lhs.content() != rhs.content() || lhs.datatype().uri() != rhs.datatype().uri();
}

bool operator==(const Value &lhs, const Value &rhs);

inline bool operator!=(const Value &lhs, const Value &rhs) {
    return !operator==(lhs,rhs);
};


bool operator==(const Resource &lhs, const Resource &rhs);

inline bool operator!=(const Resource &lhs, const Resource &rhs) {
    return !operator==(lhs,rhs);
};

bool operator==(const Statement &lhs, const Statement &rhs);

inline bool operator!=(const Statement &lhs, const Statement &rhs) {
    return !operator==(lhs,rhs);
};


}  // namespace proto


inline bool operator==(const Namespace &lhs, const Namespace &rhs) {
    return lhs.getMessage() == rhs.getMessage();
}

inline bool operator!=(const Namespace &lhs, const Namespace &rhs) {
    return !operator==(lhs,rhs);
}

inline bool operator==(const URI &lhs, const URI &rhs) {
    return lhs.getMessage() == rhs.getMessage();
}

inline bool operator!=(const URI &lhs, const URI &rhs) {
    return !operator==(lhs,rhs);
}

inline bool operator==(const BNode &lhs, const BNode &rhs) {
    return lhs.getMessage() == rhs.getMessage();
}

inline bool operator!=(const BNode &lhs, const BNode &rhs) {
    return !operator==(lhs,rhs);
}

inline bool operator==(const StringLiteral &lhs, const StringLiteral &rhs) {
    return lhs.getMessage() == rhs.getMessage();
}

inline bool operator!=(const StringLiteral &lhs, const StringLiteral &rhs) {
    return !operator==(lhs,rhs);
}

inline bool operator==(const DatatypeLiteral &lhs, const DatatypeLiteral &rhs) {
    return lhs.getMessage() == rhs.getMessage();
}

inline bool operator!=(const DatatypeLiteral &lhs, const DatatypeLiteral &rhs) {
    return !operator==(lhs,rhs);
}

inline bool operator==(const Value &lhs, const Value &rhs) {
    return lhs.getMessage() == rhs.getMessage();
}

inline bool operator!=(const Value &lhs, const Value &rhs) {
    return !operator==(lhs,rhs);
}

bool operator==(const Resource &lhs, const Resource &rhs) {
    return lhs.getMessage() == rhs.getMessage();
}

inline bool operator!=(const Resource &lhs, const Resource &rhs) {
    return !operator==(lhs,rhs);
}

bool operator==(const Statement &lhs, const Statement &rhs) {
    return lhs.getMessage() == rhs.getMessage();
}

inline bool operator!=(const Statement &lhs, const Statement &rhs) {
    return !operator==(lhs,rhs);
}

}  // namespace rdf
}  // namespace marmotta
#endif //MARMOTTA_RDF_OPERATORS_H
