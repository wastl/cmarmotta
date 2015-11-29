package org.apache.marmotta.cmarmotta.model.test;

import org.apache.marmotta.cmarmotta.model.ProtoBNode;
import org.apache.marmotta.cmarmotta.model.ProtoStatement;
import org.apache.marmotta.cmarmotta.model.ProtoStringLiteral;
import org.apache.marmotta.cmarmotta.model.ProtoURI;
import org.junit.Assert;
import org.junit.Test;
import org.openrdf.model.BNode;
import org.openrdf.model.Literal;
import org.openrdf.model.URI;
import org.openrdf.model.impl.BNodeImpl;
import org.openrdf.model.impl.LiteralImpl;
import org.openrdf.model.impl.URIImpl;

/**
 * Add file description here!
 *
 * @author Sebastian Schaffert (sschaffert@apache.org)
 */
public class StatementTest {

    @Test
    public void testCreateFromProtoValues() {
        ProtoBNode s = new ProtoBNode("1234");
        ProtoURI p = new ProtoURI("http://apache.org/example/P1");
        ProtoStringLiteral o = new ProtoStringLiteral("Hello, World", "en");
        ProtoURI c = new ProtoURI("http://apache.org/example/C1");
        ProtoStatement stmt = new ProtoStatement(s, p, o, c);

        Assert.assertEquals(stmt.getSubject(), s);
        Assert.assertEquals(stmt.getPredicate(), p);
        Assert.assertEquals(stmt.getObject(), o);
        Assert.assertEquals(stmt.getContext(), c);
    }

    @Test
    public void testCreateFromSesameValues() {
        BNode s = new BNodeImpl("1234");
        URI p = new URIImpl("http://apache.org/example/P1");
        Literal o = new LiteralImpl("Hello, World", "en");
        URI c = new URIImpl("http://apache.org/example/C1");
        ProtoStatement stmt = new ProtoStatement(s, p, o, c);

        Assert.assertEquals(stmt.getSubject(), s);
        Assert.assertEquals(stmt.getPredicate(), p);
        Assert.assertEquals(stmt.getObject(), o);
        Assert.assertEquals(stmt.getContext(), c);
    }

}
