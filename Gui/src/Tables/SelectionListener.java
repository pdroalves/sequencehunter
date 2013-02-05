package Tables;

import javax.swing.JTable;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

public class SelectionListener implements ListSelectionListener {
	private JTable table;
	private long index;

	public SelectionListener(JTable table) {
		this.table = table;
	}
	@Override
	public void valueChanged(ListSelectionEvent e) {
		if (e.getSource() == table.getSelectionModel() && table.getRowSelectionAllowed()) {
			index = e.getFirstIndex(); // Indice
		} 
	}
}