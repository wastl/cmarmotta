package org.apache.marmotta.cmarmotta.model;

import org.apache.marmotta.cmarmotta.model.proto.Model;
import org.openrdf.model.Namespace;

/**
 * An implementation of a Sesame Namespace backed by a protocol buffer.
 *
 * @author Sebastian Schaffert (sschaffert@apache.org)
 */
public class ProtoNamespace implements Namespace {
    private Model.Namespace message;

    public ProtoNamespace(Model.Namespace message) {
        this.message = message;
    }

    public ProtoNamespace(String prefix, String uri) {
        message = Model.Namespace.newBuilder()
                .setUri(uri)
                .setPrefix(prefix).build();
    }

    public Model.Namespace getMessage() {
        return message;
    }

    /**
     * Gets the name of the current namespace (i.e. it's URI).
     *
     * @return name of namespace
     */
    @Override
    public String getName() {
        return message.getUri();
    }

    /**
     * Gets the prefix of the current namespace. The default namespace is
     * represented by an empty prefix string.
     *
     * @return prefix of namespace, or an empty string in case of the default
     * namespace.
     */
    @Override
    public String getPrefix() {
        return message.getPrefix();
    }

    @Override
    public int compareTo(Namespace namespace) {
        return getPrefix().compareTo(namespace.getPrefix());
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || !(o instanceof Namespace)) return false;

        Namespace that = (Namespace) o;

        return getPrefix().equals(that.getPrefix());
    }

    @Override
    public int hashCode() {
        return getPrefix().hashCode();
    }
}
