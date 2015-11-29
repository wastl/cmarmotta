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
public class ProtoDatatypeLiteral implements Literal {

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
     * Returns the <tt>boolean</tt> value of this literal.
     *
     * @return The <tt>long</tt> value of the literal.
     * @throws IllegalArgumentException If the literal's label cannot be represented by a <tt>boolean</tt>.
     */
    @Override
    public boolean booleanValue() {
        return Boolean.parseBoolean(message.getContent());
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
     * Returns the <tt>byte</tt> value of this literal.
     *
     * @return The <tt>byte value of the literal.
     * @throws NumberFormatException If the literal cannot be represented by a <tt>byte</tt>.
     */
    @Override
    public byte byteValue() {
        return Byte.parseByte(message.getContent());
    }

    /**
     * Returns the <tt>short</tt> value of this literal.
     *
     * @return The <tt>short</tt> value of the literal.
     * @throws NumberFormatException If the literal's label cannot be represented by a <tt>short</tt>.
     */
    @Override
    public short shortValue() {
        return Short.parseShort(message.getContent());
    }

    /**
     * Returns the <tt>int</tt> value of this literal.
     *
     * @return The <tt>int</tt> value of the literal.
     * @throws NumberFormatException If the literal's label cannot be represented by a <tt>int</tt>.
     */
    @Override
    public int intValue() {
        return Integer.parseInt(message.getContent());
    }

    /**
     * Returns the <tt>long</tt> value of this literal.
     *
     * @return The <tt>long</tt> value of the literal.
     * @throws NumberFormatException If the literal's label cannot be represented by to a <tt>long</tt>.
     */
    @Override
    public long longValue() {
        return Long.parseLong(message.getContent());
    }

    /**
     * Returns the integer value of this literal.
     *
     * @return The integer value of the literal.
     * @throws NumberFormatException If the literal's label is not a valid integer.
     */
    @Override
    public BigInteger integerValue() {
        return new BigInteger(message.getContent());
    }

    /**
     * Returns the decimal value of this literal.
     *
     * @return The decimal value of the literal.
     * @throws NumberFormatException If the literal's label is not a valid decimal.
     */
    @Override
    public BigDecimal decimalValue() {
        return new BigDecimal(message.getContent());
    }

    /**
     * Returns the <tt>float</tt> value of this literal.
     *
     * @return The <tt>float</tt> value of the literal.
     * @throws NumberFormatException If the literal's label cannot be represented by a <tt>float</tt>.
     */
    @Override
    public float floatValue() {
        return Float.parseFloat(message.getContent());
    }

    /**
     * Returns the <tt>double</tt> value of this literal.
     *
     * @return The <tt>double</tt> value of the literal.
     * @throws NumberFormatException If the literal's label cannot be represented by a <tt>double</tt>.
     */
    @Override
    public double doubleValue() {
        return Double.parseDouble(message.getContent());
    }

    /**
     * Returns the {@link XMLGregorianCalendar} value of this literal. A calendar
     * representation can be given for literals whose label conforms to the
     * syntax of the following <a href="http://www.w3.org/TR/xmlschema-2/">XML
     * Schema datatypes</a>: <tt>dateTime</tt>, <tt>time</tt>,
     * <tt>date</tt>, <tt>gYearMonth</tt>, <tt>gMonthDay</tt>,
     * <tt>gYear</tt>, <tt>gMonth</tt> or <tt>gDay</tt>.
     *
     * @return The calendar value of the literal.
     * @throws IllegalArgumentException If the literal cannot be represented by a
     *                                  {@link XMLGregorianCalendar}.
     */
    @Override
    public XMLGregorianCalendar calendarValue() {
        try {
            return XMLDatatypeUtil.parseCalendar(getLabel());
        } catch(IllegalArgumentException ex) {
            // try harder to parse the label, sometimes they have stupid formats ...
            Date cv = DateUtils.parseDate(getLabel());
            return DateUtils.getXMLCalendar(cv);
        }
    }

    /**
     * Returns the String-value of a <tt>Value</tt> object. This returns either
     * a {@link Literal}'s label, a {@link URI}'s URI or a {@link BNode}'s ID.
     */
    @Override
    public String stringValue() {
        return message.getContent();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;

        ProtoDatatypeLiteral that = (ProtoDatatypeLiteral) o;

        return message.equals(that.message);

    }

    @Override
    public int hashCode() {
        return message.hashCode();
    }
}
