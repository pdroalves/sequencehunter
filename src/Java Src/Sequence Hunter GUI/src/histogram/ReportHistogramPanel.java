package histogram;

import javax.swing.JTable;

import histogram.HistogramBar;
import histogram.SimpleHistogramPanel;

public class ReportHistogramPanel extends SimpleHistogramPanel {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private JTable jte;

	public void setJTableToListen(JTable jte){
		this.jte = jte;
	}

	@Override
	public void barClicked(HistogramBar b) {
		System.out.println("Teste: "+b.getName());
		if(jte != null){
			for(int i = 0;i < jte.getRowCount();i++){
				String seq = (String)jte.getValueAt(i, 1);
				if(seq.equals(b.getName())){
					jte.clearSelection();
					jte.addRowSelectionInterval(i, i);
					return;
				}
			}
		}
	}
}
