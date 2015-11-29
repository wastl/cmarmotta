package org.apache.marmotta.cmarmotta.model;

import org.apache.marmotta.cmarmotta.model.proto.Model;
import org.openrdf.model.*;

/**
 * Add file description here!
 *
 * @author Sebastian Schaffert (sschaffert@apache.org)
 */
public class ProtoStatement implements Statement {

    private Model.Statement message;

    public ProtoStatement(Model.Statement message) {
        this.message = message;
    }

    /**
     * Build a statement backed by a proto message. The constructor can be used with any Sesame values, but
     * using ProtoValues provides slighty better performance.
     *
     * @param subject
     * @param predicate
     * @param object
     * @param context
     */
    public ProtoStatement(Resource subject, URI predicate, Value object, Resource context) {
        // Build statement, mapping the Java inheritance structure to the Proto oneof structure.
        Model.Statement.Builder builder = Model.Statement.newBuilder();
        if (subject instanceof ProtoURI) {
            builder.getSubjectBuilder().setUri(((ProtoURI) subject).getMessage()).build();
        } else if (subject instanceof ProtoBNode) {
            builder.getSubjectBuilder().setBnode(((ProtoBNode) subject).getMessage()).build();
        } else if (subject instanceof URI) {
            builder.getSubjectBuilder().getUriBuilder().setUri(subject.stringValue()).build();
        } else if (subject instanceof BNode) {
            builder.getSubjectBuilder().getBnodeBuilder().setId(subject.stringValue()).build();
        }

        if (predicate instanceof ProtoURI) {
            builder.setPredicate(((ProtoURI) predicate).getMessage()).build();
        } else {
            builder.getPredicateBuilder().setUri(predicate.stringValue()).build();
        }

        if (object instanceof ProtoStringLiteral) {
            builder.getObjectBuilder().getLiteralBuilder().setStringliteral(
                    ((ProtoStringLiteral) object).getMessage()).build();
        } else if (object instanceof ProtoDatatypeLiteral) {
            builder.getObjectBuilder().getLiteralBuilder().setDataliteral(
                    ((ProtoDatatypeLiteral) object).getMessage()).build();
        } else if (object instanceof Literal) {
            Literal l = (Literal)object;
            if (l.getDatatype() != null) {
                builder.getObjectBuilder().getLiteralBuilder().getDataliteralBuilder()
                        .setContent(l.stringValue())
                        .getDatatypeBuilder().setUri(l.getDatatype().stringValue())
                        .build();
            } else {
                builder.getObjectBuilder().getLiteralBuilder().getStringliteralBuilder()
                        .setContent(l.stringValue())
                        .setLanguage(l.getLanguage())
                        .build();
            }
        } else if (object instanceof ProtoURI) {
            builder.getObjectBuilder().getResourceBuilder().setUri(
                    ((ProtoURI) object).getMessage()).build();
        } else if (object instanceof ProtoBNode) {
            builder.getObjectBuilder().getResourceBuilder().setBnode(
                    ((ProtoBNode) object).getMessage()).build();
        } else if (object instanceof URI) {
            builder.getObjectBuilder().getResourceBuilder().getUriBuilder().setUri(
                    object.stringValue()).build();
        } else if (object instanceof BNode) {
            builder.getObjectBuilder().getResourceBuilder().getBnodeBuilder().setId(
                    object.stringValue()).build();
        }

        if (context instanceof ProtoURI) {
            builder.getContextBuilder().setUri(((ProtoURI) context).getMessage()).build();
        } else if (context instanceof ProtoBNode) {
            builder.getContextBuilder().setBnode(((ProtoBNode) context).getMessage()).build();
        } else if (context instanceof URI) {
            builder.getContextBuilder().getUriBuilder().setUri(context.stringValue()).build();
        } else if (context instanceof BNode) {
            builder.getContextBuilder().getBnodeBuilder().setId(context.stringValue()).build();
        }

        message = builder.build();
    }

    public Model.Statement getMessage() {
        return message;
    }

    /**
     * Gets the context of this statement.
     *
     * @return The statement's context, or <tt>null</tt> in case of the null
     * context or if not applicable.
     */
    @Override
    public Resource getContext() {
        if (!message.hasContext()) {
            return null;
        }
        switch(message.getContext().getResourcesCase()) {
            case URI:
                return new ProtoURI(message.getContext().getUri());
            case BNODE:
                return new ProtoBNode(message.getContext().getBnode());
        }
        return null;
    }

    /**
     * Gets the subject of this statement.
     *
     * @return The statement's subject.
     */
    @Override
    public Resource getSubject() {
        if (!message.hasSubject()) {
            return null;
        }
        switch(message.getSubject().getResourcesCase()) {
            case URI:
                return new ProtoURI(message.getSubject().getUri());
            case BNODE:
                return new ProtoBNode(message.getSubject().getBnode());
        }
        return null;
    }

    /**
     * Gets the predicate of this statement.
     *
     * @return The statement's predicate.
     */
    @Override
    public URI getPredicate() {
        if (!message.hasPredicate()) {
            return null;
        }

        return new ProtoURI(message.getPredicate());
    }

    /**
     * Gets the object of this statement.
     *
     * @return The statement's object.
     */
    @Override
    public Value getObject() {
        if (!message.hasObject()) {
            return null;
        }

        // Convert back from proto oneof to Java inheritance. It's ugly.
        switch (message.getObject().getValuesCase()) {
            case RESOURCE:
                switch(message.getObject().getResource().getResourcesCase()) {
                    case URI:
                        return new ProtoURI(message.getObject().getResource().getUri());
                    case BNODE:
                        return new ProtoBNode(message.getObject().getResource().getBnode());
                }
            case LITERAL:
                switch(message.getObject().getLiteral().getLiteralsCase()) {
                    case STRINGLITERAL:
                        return new ProtoStringLiteral(message.getObject().getLiteral().getStringliteral());
                    case DATALITERAL:
                        return new ProtoDatatypeLiteral(message.getObject().getLiteral().getDataliteral());
                }
        }

        return null;
    }


    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        ProtoStatement that = (ProtoStatement) o;

        return message.equals(that.message);

    }

    @Override
    public int hashCode() {
        return message.hashCode();
    }
}
