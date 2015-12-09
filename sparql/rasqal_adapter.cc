//
// Created by wastl on 09.12.15.
//
#include <functional>

#include <rasqal/rasqal.h>
#include <glog/logging.h>
#include <raptor2.h>
#include "rasqal_adapter.h"

namespace marmotta {
namespace sparql {

namespace {
void log_handler(void *user_data, raptor_log_message *message) {
    LOG(ERROR) << "SPARQL Error(" << message->code << "): " << message->text;
}


rasqal_triple_parts bind_match(
        struct rasqal_triples_match_s *rtm, void *user_data,
        rasqal_variable *bindings[4], rasqal_triple_parts parts) {
    StatementIterator *it = (StatementIterator *) rtm->user_data;

    int r = 0;

    DLOG(INFO) << "Binding parts " << parts;
    if ((parts & RASQAL_TRIPLE_SUBJECT) != 0) {
        DLOG(INFO) << "binding " << bindings[0]->name << " to " << (*it)->getSubject().as_turtle();
        rasqal_variable_set_value(bindings[0], rasqal::AsLiteral(rtm->world, (*it)->getSubject()));
        r |= RASQAL_TRIPLE_SUBJECT;
    }
    if ((parts & RASQAL_TRIPLE_PREDICATE) != 0) {
        DLOG(INFO) << "binding " << bindings[1]->name << " to " << (*it)->getPredicate().as_turtle();
        rasqal_variable_set_value(bindings[1], rasqal::AsLiteral(rtm->world, (*it)->getPredicate()));
        r |= RASQAL_TRIPLE_PREDICATE;
    }
    if ((parts & RASQAL_TRIPLE_OBJECT) != 0) {
        DLOG(INFO) << "binding " << bindings[2]->name << " to " << (*it)->getObject().as_turtle();
        rasqal_variable_set_value(bindings[2], rasqal::AsLiteral(rtm->world, (*it)->getObject()));
        r |= RASQAL_TRIPLE_OBJECT;
    }
    if ((parts & RASQAL_TRIPLE_ORIGIN) != 0) {
        rasqal_variable_set_value(bindings[3], rasqal::AsLiteral(rtm->world, (*it)->getContext()));
        r |= RASQAL_TRIPLE_ORIGIN;
    }

    return (rasqal_triple_parts) r;
}

void next_match(struct rasqal_triples_match_s *rtm, void *user_data) {
    StatementIterator *it = (StatementIterator *) rtm->user_data;
    ++(*it);
}

int is_end(struct rasqal_triples_match_s *rtm, void *user_data) {
    StatementIterator *it = (StatementIterator *) rtm->user_data;
    return !it->hasNext();
}

// Delete iterator and make sure its destructors are called in the C++ way.
void finish(struct rasqal_triples_match_s *rtm, void *user_data) {
    StatementIterator *it = (StatementIterator *) rtm->user_data;
    delete it;
}


int init_triples_match(
        rasqal_triples_match *rtm, struct rasqal_triples_source_s *rts,
        void *user_data, rasqal_triple_meta *m, rasqal_triple *t) {
    DLOG(INFO) << "Get statements (exact=" << rtm->is_exact << ", finished=" << rtm->finished << ")";

    SparqlService *self = (SparqlService *) user_data;

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

    rasqal_variable* var;

    m->bindings[0]=rasqal_literal_as_variable(t->subject);
    m->bindings[1]=rasqal_literal_as_variable(t->predicate);
    m->bindings[2]=rasqal_literal_as_variable(t->object);

    if(t->origin) {
        m->bindings[3]=rasqal_literal_as_variable(t->origin);
    }

    return 0;
}

// Check for triple presence, using the SparqlService::HasStatement method.
int triple_present(
        struct rasqal_triples_source_s *rts, void *user_data, rasqal_triple *t) {
    DLOG(INFO) << "Check triple";

    auto s = rasqal::ConvertResource(t->subject);
    auto p = rasqal::ConvertURI(t->predicate);
    auto o = rasqal::ConvertValue(t->object);

    SparqlService *self = (SparqlService *) user_data;
    if ((t->flags & RASQAL_TRIPLE_ORIGIN) != 0) {
        auto c = rasqal::ConvertResource(t->origin);

        return self->Source().HasStatement(&s, &p, &o, &c);
    } else {
        return self->Source().HasStatement(&s, &p, &o, nullptr);
    }
}

void free_triples_source(void *user_data) {
    DLOG(INFO) << "Free triples source";
}

int new_triples_source(rasqal_query* query, void *factory_user_data, void *user_data, rasqal_triples_source* rts) {
    DLOG(INFO) << "Init triples source";

    rts->version = 1;
    rts->user_data = factory_user_data;
    rts->init_triples_match = init_triples_match;
    rts->triple_present = triple_present;
    rts->free_triples_source = free_triples_source;

    return 0;

}

int init_triples_source(
        rasqal_query *query, void *factory_user_data, void *user_data,
        rasqal_triples_source *rts, rasqal_triples_error_handler handler) {
    return new_triples_source(query, factory_user_data, user_data, rts);
}

int init_factory(rasqal_triples_source_factory *factory) {
    DLOG(INFO) << "Init query factory";
    factory->version = 1;
    factory->new_triples_source = new_triples_source;
    factory->init_triples_source = init_triples_source;
    return 0;
}
}  // namespace


SparqlService::SparqlService(std::unique_ptr<TripleSource> source)
        : source(std::move(source)) {
    // Initialise Rasqal world.
    world = rasqal_new_world();
    rasqal_world_open(world);

    // Redirect logging output to glog.
    rasqal_world_set_log_handler(world, nullptr, log_handler);

    // Register our triple source with Rasqal, providing the relevant wrappers.
    rasqal_set_triples_source_factory(world, &init_factory, this);
}

SparqlService::~SparqlService() {
    rasqal_free_world(world);
}

void SparqlService::TupleQuery(const std::string query, std::function<bool(const RowType&)> row_handler) {
    auto q = rasqal_new_query(world, "sparql11-query", nullptr);
    auto base = raptor_new_uri(rasqal_world_get_raptor(world), (const unsigned char*)"http://example.com");
    if (rasqal_query_prepare(q, (const unsigned char*)query.c_str(), base) != 0) {
        raptor_free_uri(base);
        rasqal_free_query(q);
        throw SparqlException("Query preparation failed", query);
    }

    bool next = true;
    auto r = rasqal_query_execute(q);
    if (r == nullptr) {
        raptor_free_uri(base);
        rasqal_free_query(q);
        throw SparqlException("Query execution failed", query);
    }

    int rowcount = 0;
    while (next && rasqal_query_results_finished(r) == 0) {
        RowType row;
        DLOG(INFO) << "Row " << rowcount << " has results!";
        for (int i=0; i<rasqal_query_results_get_bindings_count(r); i++) {
            row[(const char*)rasqal_query_results_get_binding_name(r,i)] =
                    rasqal::ConvertValue(rasqal_query_results_get_binding_value(r,i));
            DLOG(INFO)
                << "Row " << rowcount << ": " << rasqal_query_results_get_binding_name(r,i) << " = "
                << row.at((const char*)rasqal_query_results_get_binding_name(r,i)).as_turtle();
        }
        next = row_handler(row);

        rasqal_query_results_next(r);
    }

    rasqal_free_query(q);
}

}  // namespace sparql
}  // namespace marmotta

