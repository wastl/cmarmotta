//
// Created by wastl on 18.04.15.
//

#ifndef MARMOTTA_RDF_MODEL_H
#define MARMOTTA_RDF_MODEL_H

#include <string>
#include <iostream>

#include "model.pb.h"

namespace marmotta {
    namespace rdf {

        class Namespace {
        public:

            Namespace(const std::string &prefix, const std::string &uri)  {
                internal_.set_prefix(prefix);
                internal_.set_uri(uri);
            }

            Namespace(const proto::Namespace &ns) : internal_(ns) { };

            const std::string &getPrefix() const {
                return internal_.prefix();
            }

            void setPrefix(std::string &prefix) {
                internal_.set_prefix(prefix);
            }

            const std::string &getUri() const {
                return internal_.uri();
            }

            void setUri(std::string &uri) {
                internal_.set_uri(uri);
            }

            const proto::Namespace& getMessage() const {
                return internal_;
            }

        private:
            proto::Namespace internal_;

            friend bool operator==(const Namespace &lhs, const Namespace &rhs);
        };


        class URI {
        public:
            URI() { }

            URI(const std::string &uri) {
                internal_.set_uri(uri);
            }

            URI(const char* uri) {
                internal_.set_uri(uri);
            }

            URI(const proto::URI &uri) : internal_(uri) { }

            const std::string &getUri() const {
                return internal_.uri();
            }

            void setUri(std::string &uri) {
                internal_.set_uri(uri);
            }

            const std::string &stringValue() const {
                return internal_.uri();
            }

            std::string as_turtle() const;

            const proto::URI& getMessage() const {
                return internal_;
            }

        private:
            proto::URI internal_;

            friend bool operator==(const URI &lhs, const URI &rhs);
        };


        class BNode {
        public:
            BNode() { }

            BNode(const std::string &id)  {
                internal_.set_id(id);
            }

            BNode(const char* id)  {
                internal_.set_id(id);
            }

            BNode(const proto::BNode &n) : internal_(n) { }

            BNode(const BNode &n) = default;

            BNode(BNode &&n) = default;

            BNode &operator=(const BNode &other) = default;

            BNode &operator=(BNode &&other) = default;

            const std::string &getId() const {
                return internal_.id();
            }

            void setId(std::string &id) {
                internal_.set_id(id);
            }

            const std::string &stringValue() const {
                return internal_.id();
            }

            const proto::BNode& getMessage() const {
                return internal_;
            }

            std::string as_turtle() const;

        private:
            proto::BNode internal_;

            friend bool operator==(const BNode &lhs, const BNode &rhs);
        };


        class StringLiteral {
        public:
            StringLiteral() { }

            StringLiteral(const std::string &content)  {
                internal_.set_content(content);
            }

            StringLiteral(const std::string &content, const std::string &language) {
                internal_.set_content(content);
                internal_.set_language(language);
            }

            StringLiteral(const proto::StringLiteral &other) : internal_(other) { };

            const std::string &getContent() const {
                return internal_.content();
            }

            void setContent(std::string &content) {
                internal_.set_content(content);
            }

            const std::string &getLanguage() const {
                return internal_.language();
            }

            void setLanguage(std::string &language) {
                internal_.set_language(language);
            }

            const std::string &stringValue() const {
                return internal_.content();
            }

            const proto::StringLiteral& getMessage() const {
                return internal_;
            }

            std::string as_turtle() const;

        private:
            proto::StringLiteral internal_;

            friend bool operator==(const StringLiteral &lhs, const StringLiteral &rhs);
        };


        class DatatypeLiteral {
        public:
            DatatypeLiteral() { }

            DatatypeLiteral(const std::string &content, URI const &datatype) {
                internal_.set_content(content);
                internal_.mutable_datatype()->MergeFrom(datatype.getMessage());
            }

            DatatypeLiteral(const proto::DatatypeLiteral &other) : internal_(other) { };


            const std::string &getContent() const {
                return internal_.content();
            }

            void setContent(std::string &content) {
                internal_.set_content(content);
            }

            URI getDatatype() const {
                return URI(internal_.datatype());
            }

            void setDatatype(const URI &datatype) {
                internal_.mutable_datatype()->MergeFrom(datatype.getMessage());
            }

            const std::string &stringValue() const {
                return internal_.content();
            }

            int intValue() const {
                return std::stoi(getContent());
            }

            operator int() const {
                return std::stoi(getContent());
            }

            long long longValue() const {
                return std::stoll(getContent());
            }

            operator long long() const {
                return std::stoll(getContent());
            }

            float floatValue() const {
                return std::stof(getContent());
            }

            operator float() const {
                return std::stof(getContent());
            }

            double doubleValue() const {
                return std::stod(getContent());
            }

            operator double() const {
                return std::stod(getContent());
            }

            const proto::DatatypeLiteral& getMessage() const {
                return internal_;
            }

            std::string as_turtle() const;

        private:
            proto::DatatypeLiteral internal_;

            friend bool operator==(const DatatypeLiteral &lhs, const DatatypeLiteral &rhs);
        };

        class Value {
        public:
            enum {
                URI = 1, BNODE, STRING_LITERAL, DATATYPE_LITERAL, NONE
            } type;

            Value() : type(NONE) { }

            Value(const proto::Value& v) : internal_(v) {
                if (v.has_resource()) {
                    if (v.resource().has_uri())
                        type = URI;
                    else
                        type = BNODE;
                } else if (v.has_literal()) {
                    if (v.literal().has_stringliteral())
                        type = STRING_LITERAL;
                    else
                        type = DATATYPE_LITERAL;
                } else {
                    type = NONE;
                }
            }

            Value(const marmotta::rdf::URI &uri) : type(URI) {
                internal_.mutable_resource()->mutable_uri()->MergeFrom(uri.getMessage());
            }

            Value(const BNode &bnode) : type(BNODE) {
                internal_.mutable_resource()->mutable_bnode()->MergeFrom(bnode.getMessage());
            }

            Value(const StringLiteral &sliteral) : type(STRING_LITERAL) {
                internal_.mutable_literal()->mutable_stringliteral()->MergeFrom(sliteral.getMessage());
            };

            Value(const DatatypeLiteral &dliteral) : type(DATATYPE_LITERAL) {
                internal_.mutable_literal()->mutable_dataliteral()->MergeFrom(dliteral.getMessage());
            };

            Value(const std::string &literal) : type(STRING_LITERAL) {
                internal_.mutable_literal()->mutable_stringliteral()->set_content(literal);
            };

            Value(const char* literal) : type(STRING_LITERAL) {
                internal_.mutable_literal()->mutable_stringliteral()->set_content(literal);
            };


            Value &operator=(const rdf::URI &uri);

            Value &operator=(const rdf::BNode &bnode);

            Value &operator=(const rdf::StringLiteral &literal);

            Value &operator=(const rdf::DatatypeLiteral &literal);

            std::string stringValue() const;

            std::string as_turtle() const;

            const proto::Value& getMessage() const {
                return internal_;
            }
        private:
            proto::Value internal_;

            friend bool operator==(const Value &lhs, const Value &rhs);
        };


        class Resource {
        public:
            enum {
                URI, BNODE, NONE
            } type;

            Resource() : type(NONE) { };

            Resource(const proto::Resource& v) : internal_(v) {
                if (v.has_uri())
                    type = URI;
                else if (v.has_bnode())
                    type = BNODE;
                else
                    type = NONE;
            }

            Resource(const std::string &uri) : type(URI) {
                internal_.mutable_uri()->set_uri(uri);
            };

            Resource(const char* uri) : type(URI) {
                internal_.mutable_uri()->set_uri(uri);
            };

            Resource(const rdf::URI &uri) : type(URI) {
                internal_.mutable_uri()->MergeFrom(uri.getMessage());
            }

            Resource(const rdf::BNode &bnode) : type(BNODE) {
                internal_.mutable_bnode()->MergeFrom(bnode.getMessage());
            }

            Resource &operator=(const rdf::URI &uri) {
                type = URI;
                internal_.mutable_uri()->MergeFrom(uri.getMessage());
                return *this;
            }

            Resource &operator=(const rdf::BNode &bnode) {
                type = BNODE;
                internal_.mutable_bnode()->MergeFrom(bnode.getMessage());
                return *this;
            }


            std::string stringValue() const;

            std::string as_turtle() const;

            const proto::Resource& getMessage() const {
                return internal_;
            }
        private:
            proto::Resource internal_;

            friend bool operator==(const Resource &lhs, const Resource &rhs);
        };


        class Statement {
        public:
            Statement(Resource const &subject, URI const &predicate, Value const &object) {
                internal_.mutable_subject()->MergeFrom(subject.getMessage());
                internal_.mutable_predicate()->MergeFrom(predicate.getMessage());
                internal_.mutable_object()->MergeFrom(object.getMessage());
            }


            Statement(Resource const &subject, URI const &predicate, Value const &object, Resource const &context) {
                internal_.mutable_subject()->MergeFrom(subject.getMessage());
                internal_.mutable_predicate()->MergeFrom(predicate.getMessage());
                internal_.mutable_object()->MergeFrom(object.getMessage());
                internal_.mutable_context()->MergeFrom(context.getMessage());
            }


            Resource getSubject() const {
                return Resource(internal_.subject());
            }

            void setSubject(Resource const &subject) {
                internal_.mutable_subject()->MergeFrom(subject.getMessage());
            }

            URI getPredicate() const {
                return URI(internal_.predicate());
            }

            void setPredicate(URI const &predicate) {
                internal_.mutable_predicate()->MergeFrom(predicate.getMessage());
            }

            Value getObject() const {
                return Value(internal_.object());
            }

            void setObject(Value const &object) {
                internal_.mutable_object()->MergeFrom(object.getMessage());
            }

            Resource getContext() const {
                return Resource(internal_.context());
            }

            void setContext(Resource const &context) {
                internal_.mutable_context()->MergeFrom(context.getMessage());
            }

            bool hasContext() const {
                return internal_.has_context();
            }

            std::string as_turtle() const;

            const proto::Statement& getMessage() const {
                return internal_;
            }
        private:
            proto::Statement internal_;

            friend bool operator==(const Statement &lhs, const Statement &rhs);
        };



        inline bool operator==(const proto::Namespace &lhs, const proto::Namespace &rhs) {
            return lhs.uri() == rhs.uri();
        }

        inline bool operator!=(const proto::Namespace &lhs, const proto::Namespace &rhs) {
            return lhs.uri() != rhs.uri();
        }

        inline bool operator==(const Namespace &lhs, const Namespace &rhs) {
            return lhs.internal_ == rhs.internal_;
        }

        inline bool operator!=(const Namespace &lhs, const Namespace &rhs) {
            return !(lhs == rhs);
        }


        inline bool operator==(const proto::URI &lhs, const proto::URI &rhs) {
            return lhs.uri() == rhs.uri();
        }

        inline bool operator!=(const proto::URI &lhs, const proto::URI &rhs) {
            return lhs.uri() != rhs.uri();
        }


        inline bool operator==(const URI &lhs, const URI &rhs) {
            return lhs.internal_ == rhs.internal_;
        }

        inline bool operator!=(const URI &lhs, const URI &rhs) {
            return !(lhs == rhs);
        }


        inline bool operator==(const proto::BNode &lhs, const proto::BNode &rhs) {
            return lhs.id() == rhs.id();
        }

        inline bool operator!=(const proto::BNode &lhs, const proto::BNode &rhs) {
            return lhs.id() != rhs.id();
        }

        inline bool operator==(const BNode &lhs, const BNode &rhs) {
            return lhs.internal_ == rhs.internal_;
        }

        inline bool operator!=(const BNode &lhs, const BNode &rhs) {
            return !(lhs == rhs);
        }

        inline bool operator==(const proto::StringLiteral &lhs, const proto::StringLiteral &rhs) {
            return lhs.content() == rhs.content() && lhs.language() == rhs.language();
        }

        inline bool operator!=(const proto::StringLiteral &lhs, const proto::StringLiteral &rhs) {
            return lhs.content() != rhs.content() || lhs.language() != rhs.language();
        }

        inline bool operator==(const StringLiteral &lhs, const StringLiteral &rhs) {
            return lhs.internal_ == rhs.internal_;
        }

        inline bool operator!=(const StringLiteral &lhs, const StringLiteral &rhs) {
            return !(lhs == rhs);
        }


        inline bool operator==(const proto::DatatypeLiteral &lhs, const proto::DatatypeLiteral &rhs) {
            return lhs.content() == rhs.content() && lhs.datatype().uri() == rhs.datatype().uri();
        }

        inline bool operator!=(const proto::DatatypeLiteral &lhs, const proto::DatatypeLiteral &rhs) {
            return lhs.content() != rhs.content() || lhs.datatype().uri() != rhs.datatype().uri();
        }

        inline bool operator==(const DatatypeLiteral &lhs, const DatatypeLiteral &rhs) {
            return lhs.internal_ == rhs.internal_;
        }

        inline bool operator!=(const DatatypeLiteral &lhs, const DatatypeLiteral &rhs) {
            return !(lhs == rhs);
        };


        bool operator==(const Value &lhs, const Value &rhs);

        inline bool operator!=(const Value &lhs, const Value &rhs) {
            return !(lhs == rhs);
        };

        bool operator==(const Resource &lhs, const Resource &rhs);

        inline bool operator!=(const Resource &lhs, const Resource &rhs) {
            return !(lhs == rhs);
        };

        bool operator==(const Statement &lhs, const Statement &rhs);

        inline bool operator!=(const rdf::Statement &lhs, const rdf::Statement &rhs) {
            return !(lhs == rhs);
        };

    }  // namespace rdf
}  // namespace marmotta


#endif //MARMOTTA_RDF_MODEL_H
