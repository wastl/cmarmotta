package org.apache.marmotta.cmarmotta.model.test;

import org.apache.marmotta.cmarmotta.model.ProtoURI;
import org.apache.marmotta.cmarmotta.model.proto.Model;
import org.junit.Assert;
import org.junit.Test;
import org.openrdf.model.URI;

/**
 * Test constructing URIs.
 *
 * @author Sebastian Schaffert (sschaffert@apache.org)
 */
public class URITest {

    @Test
    public void testCreateFromString() {
        URI uri = new ProtoURI("http://apache.org/example");

        Assert.assertEquals(uri.stringValue(), "http://apache.org/example");
    }

    @Test
    public void testCreateFromMessage() {
        Model.URI msg = Model.URI.newBuilder().setUri("http://apache.org/example").build();
        URI uri = new ProtoURI(msg);

        Assert.assertEquals(uri.stringValue(), "http://apache.org/example");
    }

    @Test
    public void testEquals() {
        URI uri1 = new ProtoURI("http://apache.org/example");
        URI uri2 = new ProtoURI("http://apache.org/example");

        Assert.assertEquals(uri1, uri2);

    }

}
