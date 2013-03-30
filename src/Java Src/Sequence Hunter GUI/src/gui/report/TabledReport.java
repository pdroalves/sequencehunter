package gui.report;

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
import javax.swing.JTable;

import database.DBManager;

import auxiliares.WaitLayerUI;

import tables.report.JReportTableModel;

public class TabledReport implements Observer{
	private WaitLayerUI layerUI;
	private DBManager dbm;
	private JPanel panel;
	private JTable jte;
	private JReportTableModel jrtm;
	
	public TabledReport(DBManager dbm,JReportTableModel jrtm){
		panel = new JPanel(new BorderLayout());
		layerUI = new WaitLayerUI();
		this.dbm = dbm;
		this.jrtm = jrtm;
	}
	
	public JComponent createTabledReport(){
		// Cria e configura tabela
		jte = new JTable(jrtm); 
		jte.setAutoCreateRowSorter(false);
		
		/*
		ListSelectionModel cellSelectionModel = jte.getSelectionModel();
		final JLabel seqJLabel = new JLabel("");
		final JLabel seqFreqJLabel = new JLabel("");
		cellSelectionModel.addListSelectionListener(new ListSelectionListener() {
			@Override
			public void valueChanged(ListSelectionEvent e) { 
				String sequence=null;
				int sequenceFreq=0;

				int[] selectedRow = jte.getSelectedRows();

				for (int i = 0; i < selectedRow.length; i++) {
					sequence = (String) jte.getValueAt(selectedRow[i], 1);
					sequenceFreq = (int) jte.getValueAt(selectedRow[i], 2);		          
				}
				seqJLabel.setText(sequence);
				seqFreqJLabel.setText(Integer.toString(sequenceFreq));
			}
		});*/
		
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
		panel.add(jscp,BorderLayout.CENTER);
		JLayer<JPanel> jlayer = new JLayer<JPanel>(panel, layerUI);
		if(!dbm.isReady()){
			layerUI.start();
		}
		return jlayer;
	}

	@Override
	public void update(Observable o, Object arg) {
		if(dbm.isReady()){
			jte.repaint();
			panel.repaint();
			jrtm.fireTableDataChanged();
			layerUI.stop();
		}
	}
	
}
