package org.apache.marmotta.cmarmotta.sail.test;

import org.apache.marmotta.cmarmotta.sail.CMarmottaSail;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.openrdf.repository.Repository;
import org.openrdf.repository.RepositoryException;
import org.openrdf.repository.sail.SailRepository;
import org.openrdf.sail.RDFStoreTest;
import org.openrdf.sail.Sail;
import org.openrdf.sail.SailConnection;
import org.openrdf.sail.SailException;

/**
 * Add file description here!
 *
 * @author Sebastian Schaffert (sschaffert@apache.org)
 */
public class CMarmottaSailTest extends RDFStoreTest {

    private static Sail repository;

    @BeforeClass
    public static void setupClass() throws SailException {
        repository = new CMarmottaSail("localhost", 10000) {
            @Override
            public void shutDown() throws SailException {
                // Clear repository on shutdown, but otherwise reuse it.
                SailConnection con = getConnection();
                con.begin();
                try {
                    con.clear();
                    con.clearNamespaces();
                } finally {
                    con.commit();
                    con.close();
                }
            }
        };
        repository.initialize();
    }

    @AfterClass
    public static void teardownClass() throws SailException {
        repository.shutDown();
    }


    /**
     * Gets an instance of the Sail that should be tested. The returned
     * repository should already have been initialized.
     *
     * @return an initialized Sail.
     * @throws SailException If the initialization of the repository failed.
     */
    @Override
    protected Sail createSail() throws SailException {
        return repository;
    }
}
