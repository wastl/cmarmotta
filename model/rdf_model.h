//
// Created by wastl on 18.04.15.
//

#ifndef MARMOTTA_RDF_MODEL_H
#define MARMOTTA_RDF_MODEL_H

#include <string>
#include <iostream>

#include "model/model.pb.h"

namespace marmotta {
namespace rdf {

class Namespace {
 public:

    Namespace(const char* prefix, const char* uri)  {
        // Raptor sends us a nullptr for the base NS.
        if (prefix != nullptr) {
            internal_.set_prefix(prefix);
        }
        internal_.set_uri(uri);
    }

    Namespace(const std::string &prefix, const std::string &uri)  {
        internal_.set_prefix(prefix);
        internal_.set_uri(uri);
    }

    Namespace(const proto::Namespace &ns) : internal_(ns) { };

    Namespace(proto::Namespace &&ns) {
        internal_.Swap(&ns);
    };

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

    URI(proto::URI &&uri) {
        internal_.Swap(&uri);
    }

    URI(const URI &other) : internal_(other.internal_) {};

    URI(URI&& uri) {
        internal_.Swap(&uri.internal_);
    }

    URI & operator=(proto::URI &&other);
    URI & operator=(const URI &other);
    URI & operator=(URI &&other);

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

    friend class Value;
    friend class Resource;
    friend class Statement;
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

    BNode(proto::BNode &&n) {
        internal_.Swap(&n);
    };

    BNode(const BNode &n) : internal_(n.internal_) {};

    BNode(BNode &&n) {
        internal_.Swap(&n.internal_);
    };

    BNode & operator=(proto::BNode &&other);;
    BNode & operator=(const BNode &other);;
    BNode & operator=(BNode &&other);;

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

    friend class Value;
    friend class Resource;
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

    StringLiteral(proto::StringLiteral &&other) {
        internal_.Swap(&other);
    }

    StringLiteral(const StringLiteral &other) : internal_(other.internal_) {};

    StringLiteral(StringLiteral &&other) {
        internal_.Swap(&other.internal_);
    }

    StringLiteral & operator=(proto::StringLiteral &&other);;
    StringLiteral & operator=(const StringLiteral &other);;
    StringLiteral & operator=(StringLiteral &&other);;

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

    friend class Value;
};


class DatatypeLiteral {
 public:
    DatatypeLiteral() { }

    DatatypeLiteral(const std::string &content, URI const &datatype) {
        internal_.set_content(content);
        internal_.mutable_datatype()->MergeFrom(datatype.getMessage());
    }

    DatatypeLiteral(const proto::DatatypeLiteral &other) : internal_(other) { };

    DatatypeLiteral(proto::DatatypeLiteral &&other) {
        internal_.Swap(&other);
    }

    DatatypeLiteral(const DatatypeLiteral &other) : internal_(other.internal_) { };

    DatatypeLiteral(DatatypeLiteral &&other) {
        internal_.Swap(&other.internal_);
    }

    DatatypeLiteral & operator=(proto::DatatypeLiteral &&other);;
    DatatypeLiteral & operator=(const DatatypeLiteral &other);;
    DatatypeLiteral & operator=(DatatypeLiteral &&other);;

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

    friend class Value;
};

class Value {
 public:
    enum {
        URI = 1, BNODE, STRING_LITERAL, DATATYPE_LITERAL, NONE
    } type;

    Value() : type(NONE) { }

    Value(const proto::Value& v);

    Value(proto::Value&& v);

    Value(const marmotta::rdf::URI &uri) : type(URI) {
        internal_.mutable_resource()->mutable_uri()->MergeFrom(uri.getMessage());
    }

    Value(marmotta::rdf::URI &&uri) : type(URI) {
        internal_.mutable_resource()->mutable_uri()->Swap(&uri.internal_);
    }

    Value(const BNode &bnode) : type(BNODE) {
        internal_.mutable_resource()->mutable_bnode()->MergeFrom(bnode.getMessage());
    }

    Value(BNode &&bnode) : type(BNODE) {
        internal_.mutable_resource()->mutable_bnode()->Swap(&bnode.internal_);
    }

    Value(const StringLiteral &sliteral) : type(STRING_LITERAL) {
        internal_.mutable_literal()->mutable_stringliteral()->MergeFrom(sliteral.getMessage());
    };

    Value(StringLiteral &&sliteral) : type(STRING_LITERAL) {
        internal_.mutable_literal()->mutable_stringliteral()->Swap(&sliteral.internal_);
    };

    Value(const DatatypeLiteral &dliteral) : type(DATATYPE_LITERAL) {
        internal_.mutable_literal()->mutable_dataliteral()->MergeFrom(dliteral.getMessage());
    };

    Value(DatatypeLiteral &&dliteral) : type(DATATYPE_LITERAL) {
        internal_.mutable_literal()->mutable_dataliteral()->Swap(&dliteral.internal_);
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

    Value &operator=(rdf::URI &&uri);

    Value &operator=(rdf::BNode &&bnode);

    Value &operator=(rdf::StringLiteral &&literal);

    Value &operator=(rdf::DatatypeLiteral &&literal);

    std::string stringValue() const;

    std::string as_turtle() const;

    const proto::Value& getMessage() const {
        return internal_;
    }
 private:
    proto::Value internal_;

    friend class Statement;
};


class Resource {
 public:
    enum {
        URI, BNODE, NONE
    } type;

    Resource() : type(NONE) { };

    Resource(const proto::Resource& v);

    Resource(proto::Resource&& v);

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

    Resource(rdf::URI &&uri) : type(URI) {
        internal_.mutable_uri()->Swap(&uri.internal_);
    }

    Resource(rdf::BNode &&bnode) : type(BNODE) {
        internal_.mutable_bnode()->Swap(&bnode.internal_);
    }

    Resource & operator=(const rdf::URI &uri);

    Resource & operator=(const rdf::BNode &bnode);

    Resource & operator=(rdf::URI &&uri);

    Resource & operator=(rdf::BNode &&bnode);

    std::string stringValue() const;

    std::string as_turtle() const;

    const proto::Resource& getMessage() const {
        return internal_;
    }
 private:
    proto::Resource internal_;

    friend class Statement;
};


class Statement {
 public:
    Statement(const Statement& other) : internal_(other.internal_) {}
    Statement(Statement&& other) {
        internal_.Swap(&other.internal_);
    }

    Statement(const proto::Statement& other) : internal_(other) {}
    Statement(proto::Statement&& other) {
        internal_.Swap(&other);
    }

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

    Statement(Resource &&subject, URI &&predicate, Value &&object) {
        internal_.mutable_subject()->Swap(&subject.internal_);
        internal_.mutable_predicate()->Swap(&predicate.internal_);
        internal_.mutable_object()->Swap(&object.internal_);
    }


    Statement(Resource &&subject, URI &&predicate, Value &&object, Resource &&context) {
        internal_.mutable_subject()->Swap(&subject.internal_);
        internal_.mutable_predicate()->Swap(&predicate.internal_);
        internal_.mutable_object()->Swap(&object.internal_);
        internal_.mutable_context()->Swap(&context.internal_);
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
};


}  // namespace rdf
}  // namespace marmotta


#endif //MARMOTTA_RDF_MODEL_H
