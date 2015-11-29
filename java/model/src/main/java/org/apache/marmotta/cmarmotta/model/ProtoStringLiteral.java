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
 * An implementation of a Sesame Literal backed by a StringLiteral protocol buffer.
 *
 * @author Sebastian Schaffert (sschaffert@apache.org)
 */
public class ProtoStringLiteral extends ProtoLiteralBase implements Literal {

    private Model.StringLiteral message;

    public ProtoStringLiteral(Model.StringLiteral message) {
        this.message = message;
    }

    public ProtoStringLiteral(String content) {
        this.message = Model.StringLiteral.newBuilder()
                .setContent(content)
                .build();
    }

    public ProtoStringLiteral(String content, String language) {
        this.message = Model.StringLiteral.newBuilder()
                .setContent(content)
                .setLanguage(language)
                .build();
    }

    public Model.StringLiteral getMessage() {
        return message;
    }

    /**
     * Gets the language tag for this literal, normalized to lower case.
     *
     * @return The language tag for this literal, or <tt>null</tt> if it
     * doesn't have one.
     */
    @Override
    public String getLanguage() {
        if ("".equals(message.getLanguage()) || message.getLanguage() == null) {
            return null;
        }
        return message.getLanguage();
    }

    /**
     * Gets the datatype for this literal.
     *
     * @return The datatype for this literal, or <tt>null</tt> if it doesn't
     * have one.
     */
    @Override
    public URI getDatatype() {
        return null;
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
     * Returns the String-value of a <tt>Value</tt> object. This returns either
     * a {@link Literal}'s label, a {@link URI}'s URI or a {@link BNode}'s ID.
     */
    @Override
    public String stringValue() {
        return message.getContent();
    }

}
