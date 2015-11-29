package org.apache.marmotta.cmarmotta.model;

import org.apache.marmotta.cmarmotta.model.proto.Model;
import org.openrdf.model.BNode;
import org.openrdf.model.Literal;
import org.openrdf.model.URI;

/**
 * An implementation of a Sesame BNode backed by a protocol buffer.
 *
 * @author Sebastian Schaffert (sschaffert@apache.org)
 */
public class ProtoBNode implements BNode {

    private Model.BNode message;

    public ProtoBNode(String id) {
        message = Model.BNode.newBuilder().setId(id).build();
    }

    public ProtoBNode(Model.BNode message) {
        this.message = message;
    }

    public Model.BNode getMessage() {
        return message;
    }

    /**
     * retrieves this blank node's identifier.
     *
     * @return A blank node identifier.
     */
    @Override
    public String getID() {
        return null;
    }

    /**
     * Returns the String-value of a <tt>Value</tt> object. This returns either
     * a {@link Literal}'s label, a {@link URI}'s URI or a {@link BNode}'s ID.
     */
    @Override
    public String stringValue() {
        return null;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        ProtoBNode that = (ProtoBNode) o;

        return message.equals(that.message);

    }

    @Override
    public int hashCode() {
        return message.hashCode();
    }
}
