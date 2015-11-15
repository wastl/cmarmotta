//
// Created by wastl on 15.11.15.
//

#include "rdf_operators.h"

namespace marmotta {
namespace rdf {
namespace proto {

bool operator==(const Value &lhs, const Value &rhs) {
    if (lhs.has_resource() && rhs.has_resource()) {
        if (lhs.resource().has_uri() && rhs.resource().has_uri()) {
            return lhs.resource().uri() == rhs.resource().uri();
        } else if (lhs.resource().has_bnode() && rhs.resource().has_bnode()) {
            return lhs.resource().bnode() == rhs.resource().bnode();
        }
    } else if(lhs.has_literal() && rhs.has_literal()) {
        if (lhs.literal().has_stringliteral() && rhs.literal().has_stringliteral()) {
            return lhs.literal().stringliteral() == rhs.literal().stringliteral();
        } else if (lhs.literal().has_dataliteral() && rhs.literal().has_dataliteral()) {
            return lhs.literal().dataliteral() == rhs.literal().dataliteral();
        }
    }
    return false;
}

bool operator==(const Resource &lhs, const Resource &rhs) {
    if (lhs.has_uri() && rhs.has_uri()) {
        return lhs.uri() == rhs.uri();
    } else if (lhs.has_bnode() && rhs.has_bnode()) {
        return lhs.bnode() == rhs.bnode();
    }
    return false;
}

bool operator==(const Statement &lhs, const Statement &rhs) {
    return operator==(lhs.subject(), rhs.subject()) &&
           operator==(lhs.predicate(), rhs.predicate()) &&
           operator==(lhs.object(), rhs.object()) &&
           operator==(lhs.context(), rhs.context());

}


}  // namespace proto
}  // namespace rdf
}  // namespace marmotta
