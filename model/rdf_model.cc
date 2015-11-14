//
// Created by wastl on 18.04.15.
//
#include <new>

#include "rdf_model.h"

namespace marmotta {
    namespace rdf {

        static std::string as_turtle_(const proto::URI& uri) {
            return "<" + uri.uri() + ">";
        }

        static std::string as_turtle_(const proto::BNode& bnode) {
            return "_:" + bnode.id();
        }

        static std::string as_turtle_(const proto::StringLiteral& literal) {
            if (literal.language() == "") {
                return "\"" + literal.content() + "\"";
            } else {
                return "\"" + literal.content() + "\"@" + literal.language();
            }
        }

        static std::string as_turtle_(const proto::DatatypeLiteral& literal) {
            return "\"" + literal.content() + "\"^^" + as_turtle_(literal.datatype());
        }

        static std::string as_turtle_(const proto::Resource& resource) {
            if (resource.has_uri()) {
                return as_turtle_(resource.uri());
            }
            if (resource.has_bnode()) {
                return as_turtle_(resource.bnode());
            }
            return "";
        }

        static std::string as_turtle_(const proto::Value& value) {
            if (value.has_resource()) {
                if (value.resource().has_uri()) {
                    return as_turtle_(value.resource().uri());
                }
                if (value.resource().has_bnode()) {
                    return as_turtle_(value.resource().bnode());
                }
            }
            if (value.has_literal()) {
                if (value.literal().has_stringliteral()) {
                    return as_turtle_(value.literal().stringliteral());
                }
                if (value.literal().has_dataliteral()) {
                    return as_turtle_(value.literal().dataliteral());
                }
            }
            return "";
        }

        std::string URI::as_turtle() const {
            return as_turtle_(internal_);
        }

        std::string BNode::as_turtle() const {
            return as_turtle_(internal_);
        }

        std::string StringLiteral::as_turtle() const {
            return as_turtle_(internal_);
        }

        std::string DatatypeLiteral::as_turtle() const {
            return as_turtle_(internal_);
        }



        std::string Resource::stringValue() const {
            switch (type) {
                case URI:
                    return internal_.uri().uri();
                case BNODE:
                    return internal_.bnode().id();
                default:
                    return "";
            }
        }

        std::string Resource::as_turtle() const {
            return as_turtle_(internal_);
        }


        Value &Value::operator=(const marmotta::rdf::URI &_uri) {
            type = URI;
            internal_.mutable_resource()->mutable_uri()->MergeFrom(_uri.getMessage());
            return *this;
        }


        Value &Value::operator=(const BNode &_bnode) {
            type = BNODE;
            internal_.mutable_resource()->mutable_bnode()->MergeFrom(_bnode.getMessage());
            return *this;
        }

        Value &Value::operator=(const StringLiteral &literal) {
            type = STRING_LITERAL;
            internal_.mutable_literal()->mutable_stringliteral()->MergeFrom(literal.getMessage());
            return *this;
        }

        Value &Value::operator=(const DatatypeLiteral &literal) {
            type = DATATYPE_LITERAL;
            internal_.mutable_literal()->mutable_dataliteral()->MergeFrom(literal.getMessage());
            return *this;
        }

        std::string Value::stringValue() const {
            switch (type) {
                case URI:
                    return internal_.resource().uri().uri();
                case BNODE:
                    return internal_.resource().bnode().id();
                case STRING_LITERAL:
                    return internal_.literal().stringliteral().content();
                case DATATYPE_LITERAL:
                    return internal_.literal().dataliteral().content();
                default:
                    return "";
            }
        }

        std::string Value::as_turtle() const {
            return as_turtle_(internal_);
        }


        std::string Statement::as_turtle() const {
            if (hasContext()) {
                return as_turtle_(internal_.context()) + " { " +
                       as_turtle_(internal_.subject()) + " " +
                       as_turtle_(internal_.predicate()) + " " +
                       as_turtle_(internal_.object()) + ". }";
            } else {
                return as_turtle_(internal_.subject()) + " " +
                       as_turtle_(internal_.predicate()) + " " +
                       as_turtle_(internal_.object()) + ".";
            }
        }

        bool operator==(const Value &lhs, const Value &rhs) {
            if (lhs.type != rhs.type) {
                return false;
            }

            switch (lhs.type) {
                case Value::URI:
                    return lhs.internal_.resource().uri() == rhs.internal_.resource().uri();
                case Value::BNODE:
                    return lhs.internal_.resource().bnode() == rhs.internal_.resource().bnode();
                case Value::STRING_LITERAL:
                    return lhs.internal_.literal().stringliteral() == rhs.internal_.literal().stringliteral();
                case Value::DATATYPE_LITERAL:
                    return lhs.internal_.literal().dataliteral() == rhs.internal_.literal().dataliteral();
                default:
                    return false;
            }
        }

        bool operator==(const Resource &lhs, const Resource &rhs) {
            if (lhs.type != rhs.type) {
                return false;
            }

            switch (lhs.type) {
                case Resource::URI:
                    return lhs.internal_.uri() == rhs.internal_.uri();
                case Resource::BNODE:
                    return lhs.internal_.bnode() == rhs.internal_.bnode();
                default:
                    return false;
            }
        }


        bool operator==(const Statement &lhs, const Statement &rhs) {
            return lhs.internal_.subject() == rhs.internal_.subject() &&
                   lhs.internal_.predicate() == rhs.internal_.predicate() &&
                   lhs.internal_.object() == rhs.internal_.object() &&
                   lhs.internal_.context() == rhs.internal_.context();

        }

    }  // namespace rdf
}  // namespace marmotta
