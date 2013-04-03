package tables.report;

import histogram.EventHistogram;
import histogram.ReportHistogramPanel;
import histogram.SimpleHistogramPanel;

import java.awt.BorderLayout;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JComponent;
import javax.swing.JLayer;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.border.EmptyBorder;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import database.DBManager;

import auxiliares.WaitLayerUI;


public class TabledReport implements Observer{
	private WaitLayerUI layerUI;
	private DBManager dbm;
	private JPanel panel;
	private JTable jte;
	private JReportTableModel jrtm;
	private EventHistogram eh;
	
	public TabledReport(DBManager dbm,JReportTableModel jrtm){
		panel = new JPanel(new BorderLayout());
		layerUI = new WaitLayerUI();
		this.dbm = dbm;
		this.jrtm = jrtm;
		eh = new EventHistogram();
	}
	
	public JComponent createTabledReport(){
		// Cria e configura tabela
		jte = new JTable(jrtm); 
		jte.setAutoCreateRowSorter(false);
		
		// Table selection listener
		ListSelectionModel cellSelectionModel = jte.getSelectionModel();
		cellSelectionModel.addListSelectionListener(new ListSelectionListener() {
			@Override
			public void valueChanged(ListSelectionEvent e) { 
				// Ativa highlight de barra e linha
				String sequence=null;

				int[] selectedRow = jte.getSelectedRows();

				for (int i = 0; i < selectedRow.length; i++) {
					sequence = (String) jte.getValueAt(selectedRow[i], 1);	        
					eh.enableBarHighlight(sequence, true);
				}
			}
		});
		JScrollPane jscp = new JScrollPane(jte);
		JScrollBar jsb = jscp.getVerticalScrollBar();
		jsb.addAdjustmentListener(new AdjustmentListener(){
			@Override
			public void adjustmentValueChanged(AdjustmentEvent e) {
				JScrollBar jsb = (JScrollBar) e.getSource();
				int jsbMax = jsb.getMaximum();
				int jsbPos = jsb.getValue();
				System.out.println(jsbPos+"/"+jsbMax+" - "+(float)(jsbPos)*100/jsbMax+"%");
				if(jsbMax*0.6 <= jsbPos){
					System.out.println("Loading...");
					jrtm.load();
				}
			}					
		});
		jte.setAutoscrolls(true);

		/*Box seqInfo = Box.createVerticalBox();
		seqInfo.add(new JLabel(tm.getText("reportSequenceInfoLabel")));
		seqInfo.add(seqJLabel);
		seqInfo.add(new JLabel(tm.getText("reportSequenceFrequencyInfoLabel")));
		seqInfo.add(seqFreqJLabel);*/
		
		// Cria histograma
		eh.enableLinearize(true);
		ReportHistogramPanel rhp = eh.getPanel();
		rhp.setBorder(new EmptyBorder(25,15,45,10));
		// Table selection
		rhp.setJTableToListen(jte);
		JSplitPane jsp = new JSplitPane(JSplitPane.VERTICAL_SPLIT,true,rhp,jscp);
		
		panel.add(jsp,BorderLayout.CENTER);
		JLayer<JPanel> jlayer = new JLayer<JPanel>(panel, layerUI);
		if(!dbm.isReady()){
			layerUI.start();
		}else{
			eh.addTypeSet(dbm.getEvents());
			eh.commit();
		}
		return jlayer;
	}

	@Override
	public void update(Observable o, Object arg) {
		if(dbm.isReady()){
			eh.addTypeSet(dbm.getEvents());
			eh.commit();
			jte.repaint();
			panel.repaint();
			jrtm.fireTableDataChanged();
			layerUI.stop();
		}
	}	
}
