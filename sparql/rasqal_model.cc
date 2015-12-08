//
// Created by wastl on 08.12.15.
//

#include <raptor2/raptor2.h>
#include <rasqal/rasqal.h>
#include "rasqal_model.h"

namespace marmotta {
namespace sparql {
namespace rasqal {


rdf::Resource ConvertResource(rasqal_literal *node) {
    switch (node->type) {
        case RASQAL_LITERAL_URI:
            return rdf::URI(std::string((const char*)node->string, node->string_len));
        case RASQAL_LITERAL_BLANK:
            return rdf::BNode(std::string((const char*)node->string, node->string_len));
        default:
            return rdf::Resource();
    }
}


rdf::Value ConvertValue(rasqal_literal *node) {
    std::string label((const char*)node->string, node->string_len);
    rdf::Value r;
    char* s;
    switch (node->type) {
        case RASQAL_LITERAL_URI:
            return rdf::URI(label);
        case RASQAL_LITERAL_BLANK:
            return rdf::BNode(label);
        case RASQAL_LITERAL_STRING:
            if (node->language) {
                return rdf::StringLiteral(label, node->language);
            } else {
                return rdf::StringLiteral(label);

            }
        case RASQAL_LITERAL_XSD_STRING:
            return rdf::DatatypeLiteral(
                    label, rdf::URI((const char*)raptor_uri_as_string(node->datatype)));
        case RASQAL_LITERAL_BOOLEAN:
            return rdf::DatatypeLiteral(
                    node->value.integer==0?"false":"true",
                    rdf::URI((const char*)raptor_uri_as_string(node->datatype)));
        case RASQAL_LITERAL_INTEGER:
            return rdf::DatatypeLiteral(
                    std::to_string(node->value.integer),
                    rdf::URI((const char*)raptor_uri_as_string(node->datatype)));
        case RASQAL_LITERAL_FLOAT:
        case RASQAL_LITERAL_DOUBLE:
            return rdf::DatatypeLiteral(
                    std::to_string(node->value.floating),
                    rdf::URI((const char*)raptor_uri_as_string(node->datatype)));
        case RASQAL_LITERAL_DECIMAL:
            s = rasqal_xsd_decimal_as_string(node->value.decimal);
            r = rdf::DatatypeLiteral(
                    s, rdf::URI((const char*)raptor_uri_as_string(node->datatype)));
            free(s);
            return std::move(r); // r is an lvalue, explicit move
        case RASQAL_LITERAL_DATETIME:
            s = rasqal_xsd_datetime_to_string(node->value.datetime);
            r = rdf::DatatypeLiteral(
                    s, rdf::URI((const char*)raptor_uri_as_string(node->datatype)));
            free(s);
            return std::move(r); // r is an lvalue, explicit move
        case RASQAL_LITERAL_DATE:
            s = rasqal_xsd_date_to_string(node->value.date);
            r = rdf::DatatypeLiteral(
                    s, rdf::URI((const char*)raptor_uri_as_string(node->datatype)));
            free(s);
            return std::move(r); // r is an lvalue, explicit move
        default:
            return rdf::Value();
    }
}


rdf::URI ConvertURI(rasqal_literal *node) {
    switch (node->type) {
        case RASQAL_LITERAL_URI:
            return rdf::URI(std::string((const char*)node->string, node->string_len));
        default:
            return rdf::URI();
    }
}


rdf::Statement ConvertStatement(rasqal_triple *triple) {
    if (triple->flags == RASQAL_TRIPLE_SPOG) {
        return rdf::Statement(
                ConvertResource(triple->subject),
                ConvertURI(triple->predicate),
                ConvertValue(triple->object),
                ConvertResource(triple->origin)
        );
    } else {
        return rdf::Statement(
                ConvertResource(triple->subject),
                ConvertURI(triple->predicate),
                ConvertValue(triple->object)
        );

    }
}

rasqal_literal *AsStringLiteral(rasqal_world* world, const rdf::Value &v) {
    rdf::StringLiteral l(v.getMessage().literal().stringliteral());

    return rasqal_new_string_literal(
            world, (const unsigned char*)l.getContent().c_str(), l.getLanguage().c_str(), nullptr, nullptr);
}

rasqal_literal *AsDatatypeLiteral(rasqal_world* world, const rdf::Value &v) {
    rdf::DatatypeLiteral l(v.getMessage().literal().dataliteral());


    // TODO

    return nullptr;
}

rasqal_literal *AsLiteral(rasqal_world* world, const rdf::Resource &r) {
    raptor_world* raptorWorld = rasqal_world_get_raptor(world);
    switch (r.type) {
        case rdf::Resource::URI:
            return rasqal_new_uri_literal(world, raptor_new_uri(
                    raptorWorld, (const unsigned char*)r.stringValue().c_str()));
        case rdf::Resource::BNODE:
            return rasqal_new_simple_literal(
                    world, RASQAL_LITERAL_BLANK, (const unsigned char*)r.stringValue().c_str());
        default:
            return nullptr;
    }
}

rasqal_literal *AsLiteral(rasqal_world* world, const rdf::Value &v) {
    raptor_world* raptorWorld = rasqal_world_get_raptor(world);
    switch (v.type) {
        case rdf::Value::URI:
            return rasqal_new_uri_literal(world, raptor_new_uri(
                    raptorWorld, (const unsigned char*)v.stringValue().c_str()));
        case rdf::Value::BNODE:
            return rasqal_new_simple_literal(
                    world, RASQAL_LITERAL_BLANK, (const unsigned char*)v.stringValue().c_str());
        case rdf::Value::STRING_LITERAL:
            return AsStringLiteral(world, v);
        case rdf::Value::DATATYPE_LITERAL:
            return AsDatatypeLiteral(world, v);
        default:
            return nullptr;
    }
}

rasqal_literal *AsLiteral(rasqal_world* world, const rdf::URI &u) {
    raptor_world* raptorWorld = rasqal_world_get_raptor(world);
    return rasqal_new_uri_literal(world, raptor_new_uri(
            raptorWorld, (const unsigned char*)u.stringValue().c_str()));
}
}  // namespace rasqal
}  // namespace sparql
}  // namespace marmotta

