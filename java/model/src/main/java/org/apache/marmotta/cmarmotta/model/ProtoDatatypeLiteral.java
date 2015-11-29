package org.apache.marmotta.cmarmotta.model;

import org.apache.marmotta.cmarmotta.model.proto.Model;
import org.apache.marmotta.commons.util.DateUtils;
import org.openrdf.model.BNode;
import org.openrdf.model.Literal;
import org.openrdf.model.URI;
import org.openrdf.model.datatypes.XMLDatatypeUtil;

import javax.xml.datatype.XMLGregorianCalendar;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.Date;

/**
 * Add file description here!
 *
 * @author Sebastian Schaffert (sschaffert@apache.org)
 */
public class ProtoDatatypeLiteral extends ProtoLiteralBase implements Literal {

    private Model.DatatypeLiteral message;

    public ProtoDatatypeLiteral(Model.DatatypeLiteral message) {
        this.message = message;
    }

    public ProtoDatatypeLiteral(String content) {
        this.message = Model.DatatypeLiteral.newBuilder()
                .setContent(content)
                .build();
    }

    public ProtoDatatypeLiteral(String content, URI datatype) {
        this.message = Model.DatatypeLiteral.newBuilder()
                .setContent(content)
                .setDatatype(Model.URI.newBuilder().setUri(datatype.stringValue()).build())
                .build();
    }

    public Model.DatatypeLiteral getMessage() {
        return message;
    }

    /**
     * Gets the label of this literal.
     *
     * @return The literal's label.
     */
    @Override
    public String getLabel() {
        return message.getContent();
    }

    /**
     * Gets the language tag for this literal, normalized to lower case.
     *
     * @return The language tag for this literal, or <tt>null</tt> if it
     * doesn't have one.
     */
    @Override
    public String getLanguage() {
        return null;
    }

    /**
     * Gets the datatype for this literal.
     *
     * @return The datatype for this literal, or <tt>null</tt> if it doesn't
     * have one.
     */
    @Override
    public URI getDatatype() {
        if (!message.hasDatatype()) {
            return null;
        }
        return new ProtoURI(message.getDatatype());
    }


    /**
     * Returns the String-value of a <tt>Value</tt> object. This returns either
     * a {@link Literal}'s label, a {@link URI}'s URI or a {@link BNode}'s ID.
     */
    @Override
    public String stringValue() {
        return message.getContent();
    }
}
