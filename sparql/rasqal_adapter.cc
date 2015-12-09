//
// Created by wastl on 09.12.15.
//
#include <functional>

#include <rasqal/rasqal.h>
#include "rasqal_adapter.h"

namespace marmotta {
namespace sparql {

rasqal_triple_parts bind_match(
        struct rasqal_triples_match_s* rtm, void *user_data,
        rasqal_variable *bindings[4], rasqal_triple_parts parts) {
    StatementIterator* it = (StatementIterator*)rtm->user_data;

    int r = 0;

    if ((parts & RASQAL_TRIPLE_SUBJECT) != 0) {
        rasqal_variable_set_value(bindings[0], rasqal::AsLiteral(rtm->world, (*it)->getSubject()));
        r |= RASQAL_TRIPLE_SUBJECT;
    }
    if ((parts & RASQAL_TRIPLE_PREDICATE) != 0) {
        rasqal_variable_set_value(bindings[1], rasqal::AsLiteral(rtm->world, (*it)->getPredicate()));
        r |= RASQAL_TRIPLE_PREDICATE;
    }
    if ((parts & RASQAL_TRIPLE_OBJECT) != 0) {
        rasqal_variable_set_value(bindings[2], rasqal::AsLiteral(rtm->world, (*it)->getObject()));
        r |= RASQAL_TRIPLE_OBJECT;
    }
    if ((parts & RASQAL_TRIPLE_ORIGIN) != 0) {
        rasqal_variable_set_value(bindings[0], rasqal::AsLiteral(rtm->world, (*it)->getContext()));
        r |= RASQAL_TRIPLE_ORIGIN;
    }

    return (rasqal_triple_parts)r;
}

void next_match(struct rasqal_triples_match_s* rtm, void *user_data) {
    StatementIterator* it = (StatementIterator*)rtm->user_data;
    ++(*it);
}

int is_end(struct rasqal_triples_match_s* rtm, void *user_data) {
    StatementIterator* it = (StatementIterator*)rtm->user_data;
    return it->hasNext();
}

// Delete iterator and make sure its destructors are called in the C++ way.
void finish(struct rasqal_triples_match_s* rtm, void *user_data) {
    StatementIterator* it = (StatementIterator*)rtm->user_data;
    delete it;
}


int init_triples_match(
        rasqal_triples_match* rtm, struct rasqal_triples_source_s* rts,
        void *user_data, rasqal_triple_meta *m, rasqal_triple *t) {
    SparqlService* self = (SparqlService*)user_data;

    std::unique_ptr<rdf::Resource> s = nullptr;
    std::unique_ptr<rdf::URI> p = nullptr;
    std::unique_ptr<rdf::Value> o = nullptr;
    std::unique_ptr<rdf::Resource> c = nullptr;

    if ((t->flags & RASQAL_TRIPLE_SUBJECT) != 0) {
        s.reset(new rdf::Resource(rasqal::ConvertResource(t->subject)));
    }
    if ((t->flags & RASQAL_TRIPLE_PREDICATE) != 0) {
        p.reset(new rdf::URI(rasqal::ConvertURI(t->predicate)));
    }
    if ((t->flags & RASQAL_TRIPLE_OBJECT) != 0) {
        o.reset(new rdf::Value(rasqal::ConvertValue(t->object)));
    }
    if ((t->flags & RASQAL_TRIPLE_ORIGIN) != 0) {
        c.reset(new rdf::Resource(rasqal::ConvertResource(t->origin)));
    }

    // Store C++ iterator in user_data and take ownership
    auto it = self->Source().GetStatements(s.get(), p.get(), o.get(), c.get());
    rtm->user_data = it.release();

    rtm->bind_match = bind_match;
    rtm->next_match = next_match;
    rtm->is_end = is_end;
    rtm->finish = finish;

    return 0;
}

// Check for triple presence, using the SparqlService::HasStatement method.
int triple_present(
        struct rasqal_triples_source_s* rts, void *user_data, rasqal_triple *t) {
    SparqlService* self = (SparqlService*)user_data;

    auto s = rasqal::ConvertResource(t->subject);
    auto p = rasqal::ConvertURI(t->predicate);
    auto o = rasqal::ConvertValue(t->object);

    if ((t->flags & RASQAL_TRIPLE_ORIGIN) != 0) {
        auto c = rasqal::ConvertResource(t->origin);

        return self->Source().HasStatement(&s, &p, &o, &c);
    } else {
        return self->Source().HasStatement(&s, &p, &o, nullptr);
    }
}

int init_triples_source(
        rasqal_query* query, void *factory_user_data, void *user_data,
        rasqal_triples_source* rts, rasqal_triples_error_handler handler) {

    rts->user_data = factory_user_data;
    rts->init_triples_match = init_triples_match;
    rts->triple_present = triple_present;

    return 0;
}


int init_factory(rasqal_triples_source_factory *factory) {
    factory->version = 2;
    factory->init_triples_source = init_triples_source;
    return 0;
}


SparqlService::SparqlService(std::unique_ptr<TripleSource> source)
        : source(std::move(source)) {
    // Initialise Rasqal world.
    world = rasqal_new_world();

    // Register our triple source with Rasqal, providing the relevant wrappers.
    rasqal_set_triples_source_factory(world, &init_factory, this);
}

SparqlService::~SparqlService() {
    rasqal_free_world(world);
}

}  // namespace sparql
}  // namespace marmotta

