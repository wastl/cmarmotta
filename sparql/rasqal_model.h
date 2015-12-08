//
// Created by wastl on 08.12.15.
//

#ifndef MARMOTTA_RASQAL_MODEL_H
#define MARMOTTA_RASQAL_MODEL_H

#include <memory>
#include <rasqal/rasqal.h>

#include "model/rdf_model.h"

namespace marmotta {
namespace sparql {
namespace rasqal {

/*
 * Convert a rasqal literal into a Marmotta Resource. Returns empty in case
 * the node cannot be converted.
 */
rdf::Resource ConvertResource(rasqal_literal* node);

/*
 * Convert a rasqal literal into a Marmotta Value. Returns empty in case
 * the node cannot be converted.
 */
rdf::Value ConvertValue(rasqal_literal* node);

/*
 * Convert a rasqal literal into a Marmotta URI. Returns empty in case
 * the node cannot be converted.
 */
rdf::URI ConvertURI(rasqal_literal* node);

/*
 * Convert a rasqal triple into a Marmotta Statement. Returns empty in case
 * the node cannot be converted.
 */
rdf::Statement ConvertStatement(rasqal_triple* triple);

/*
 * Convert a Marmotta Resource into a rasqal literal.
 */
rasqal_literal* AsLiteral(rasqal_world* world, const rdf::Resource& r);

/*
 * Convert a Marmotta Value into a rasqal literal.
 */
rasqal_literal* AsLiteral(rasqal_world* world, const rdf::Value& v);

/*
 * Convert a Marmotta URI into a rasqal literal.
 */
rasqal_literal* AsLiteral(rasqal_world* world, const rdf::URI& u);


}  // namespace rasqal
}  // namespace sparql
}  // namespace marmotta

#endif //MARMOTTA_RASQAL_MODEL_H
