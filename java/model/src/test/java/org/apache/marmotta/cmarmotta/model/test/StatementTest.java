package org.apache.marmotta.cmarmotta.model.test;

import org.apache.marmotta.cmarmotta.model.ProtoBNode;
import org.apache.marmotta.cmarmotta.model.ProtoStatement;
import org.apache.marmotta.cmarmotta.model.ProtoStringLiteral;
import org.apache.marmotta.cmarmotta.model.ProtoURI;
import org.junit.Assert;
import org.junit.Test;

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
}
