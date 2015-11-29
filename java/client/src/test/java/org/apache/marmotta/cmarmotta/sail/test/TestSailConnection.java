package org.apache.marmotta.cmarmotta.sail.test;

import info.aduna.iteration.CloseableIteration;
import org.apache.marmotta.cmarmotta.sail.CMarmottaSail;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;
import org.openrdf.model.Statement;
import org.openrdf.model.URI;
import org.openrdf.repository.Repository;
import org.openrdf.repository.RepositoryConnection;
import org.openrdf.repository.RepositoryException;
import org.openrdf.repository.RepositoryResult;
import org.openrdf.repository.sail.SailRepository;
import org.openrdf.rio.RDFFormat;
import org.openrdf.rio.RDFHandlerException;
import org.openrdf.rio.RDFWriter;
import org.openrdf.rio.Rio;

/**
 * Add file description here!
 *
 * @author Sebastian Schaffert (sschaffert@apache.org)
 */
public class TestSailConnection {

    private static Repository repository;

    @BeforeClass
    public static void setup() throws RepositoryException {
        repository = new SailRepository(new CMarmottaSail("localhost", 10000));
        repository.initialize();
    }

    @AfterClass
    public static void teardown() throws RepositoryException {
        repository.shutDown();
    }

    @Test
    public void testQuery() throws RepositoryException, RDFHandlerException {
        RDFWriter writer = Rio.createWriter(RDFFormat.TURTLE, System.out);
        URI s = repository.getValueFactory().createURI("http://umbel.org/umbel/rc/Zyban");

        RepositoryConnection con = repository.getConnection();
        try {
            con.begin();
            writer.startRDF();

            RepositoryResult<Statement> it = con.getStatements(s, null, null, true);
            while (it.hasNext()) {
                writer.handleStatement(it.next());
            }

            writer.endRDF();

            con.commit();
        } catch(RepositoryException ex) {
            con.rollback();
        } finally {
            con.close();
        }
    }
}
