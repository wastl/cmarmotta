package org.apache.marmotta.cmarmotta.sail;

import org.openrdf.model.ValueFactory;
import org.openrdf.sail.NotifyingSailConnection;
import org.openrdf.sail.Sail;
import org.openrdf.sail.SailConnection;
import org.openrdf.sail.SailException;
import org.openrdf.sail.helpers.NotifyingSailBase;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.io.File;

/**
 * Add file description here!
 *
 * @author Sebastian Schaffert (sschaffert@apache.org)
 */
public class CMarmottaSail extends NotifyingSailBase implements Sail {
    private static Logger log = LoggerFactory.getLogger(CMarmottaSail.class);

    private CMarmottaValueFactory valueFactory = new CMarmottaValueFactory();

    private String host;
    private int port;

    public CMarmottaSail(String host, int port) {
        this.host = host;
        this.port = port;
    }

    /**
     * Do store-specific operations to initialize the store. The default
     * implementation of this method does nothing.
     */
    @Override
    protected void initializeInternal() throws SailException {
        log.info("Initialising CMarmotta Sail (host={}, port={})", host, port);
    }

    @Override
    protected NotifyingSailConnection getConnectionInternal() throws SailException {
        return new CMarmottaSailConnection(this, host, port);
    }

    /**
     * Do store-specific operations to ensure proper shutdown of the store.
     */
    @Override
    protected void shutDownInternal() throws SailException {

    }

    /**
     * Checks whether this Sail object is writable, i.e. if the data contained in
     * this Sail object can be changed.
     */
    @Override
    public boolean isWritable() throws SailException {
        return true;
    }

    /**
     * Gets a ValueFactory object that can be used to create URI-, blank node-,
     * literal- and statement objects.
     *
     * @return a ValueFactory object for this Sail object.
     */
    @Override
    public ValueFactory getValueFactory() {
        return valueFactory;
    }
}
