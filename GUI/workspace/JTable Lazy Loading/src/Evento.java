import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;
import java.util.logging.Logger;

/**
 *
 * @author Ruben Laguna <ruben.laguna at gmail.com>
 */
public class Evento {

    private static final Logger LOG = Logger.getLogger(Evento.class.getName());
    private static final String NOT_RETRIEVED_YET = "<not retrieved yet>";
    private int id = 0;
    public static final String PROP_id = "id";
    public static final String PROP_SEQ = "main_seq";
    protected String main_seq = NOT_RETRIEVED_YET;
    public static final String PROP_SENSOS = "qnt_sensos";
    protected String qnt_sensos = NOT_RETRIEVED_YET;
    public static final String PROP_ANTISENSOS = "qnt_antisensos";
    protected String qnt_antisensos = NOT_RETRIEVED_YET;
    private PropertyChangeSupport propertyChangeSupport = new PropertyChangeSupport(this);

    Evento(int j) {
        this.id = j;
    }

    public int getId() {
        return id;
    }

    public String getMainSeq() {
        return main_seq;
    }

    public void setMainSeq(String main_seq) {
        String oldMainSeq = this.main_seq;
        this.main_seq = main_seq;
        propertyChangeSupport.firePropertyChange(PROP_SEQ, oldMainSeq, main_seq);
    }

    public String getQntSensos() {
        return qnt_sensos;
    }

    public void setQntSensos(String qnt_sensos) {
        String oldQntSensos = this.qnt_sensos;
        this.qnt_sensos = qnt_sensos;
        if (propertyChangeSupport.hasListeners(PROP_SENSOS)) {
            LOG.info("notifying!!!");
        } else {
            LOG.info("there is no listeners for the property");
        }

        propertyChangeSupport.firePropertyChange(PROP_SENSOS, oldQntSensos, qnt_sensos);
    }

    public void addPropertyChangeListener(PropertyChangeListener listener) {
        LOG.info("add listener to customer " + this.id);
        propertyChangeSupport.addPropertyChangeListener(listener);
    }

    public void removePropertyChangeListener(PropertyChangeListener listener) {
        propertyChangeSupport.removePropertyChangeListener(listener);
    }

    @Override
    public String toString() {
        return "id: " + id + " firstName: " + main_seq;
    }
}