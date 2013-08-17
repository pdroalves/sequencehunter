package com.lnbio.gui.modules;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.ArrayList;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JSpinner;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.SpinnerModel;
import javax.swing.SpinnerNumberModel;
import javax.swing.border.EmptyBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import com.lnbio.database.DBManager;
import com.lnbio.gui.Drawer;
import com.lnbio.gui.ReportDrawer;
import com.lnbio.tables.report.JFullSeqReportTableModel;
import com.lnbio.tables.report.Report;

public class CustomReportBuilderWorker extends Thread implements ActionListener {

	private int dist;
	private int tam;
	private int mainTabIndex;
	private String centralCutSeq;
	private JTabbedPane reportTab;
	private List<List<Report>> data;
	private List<String> reportName;
	private List<List<String>> tabNames;
	private ReportDrawer owner;
	private ArrayList<String> fullSeqs;
	private JLabel DistPosLabel;
	private JTable table;
	private JTabbedPane mainTab;
	private DBManager dbm;
	private JButton searchButton;
	
	public CustomReportBuilderWorker(
			ReportDrawer owner,
			DBManager dbm,
			String centralCutSeq, 
			int mainTabIndex,
			List<List<Report>> data,
			List<List<String>> tabNames,
			List<String> reportName,
			JTabbedPane reportTab) throws Exception{
		this.owner = owner;
		this.mainTabIndex = mainTabIndex;
		this.setCentralCutSeq(centralCutSeq);
		this.data = data;
		this.tabNames = tabNames;
		this.reportName = reportName;
		this.reportTab = reportTab;
		this.dbm = dbm;
		searchButton = new JButton("Search");
		searchButton.setEnabled(false);
	}
	
	public void run(){
		
		// Monta tabela com fullSeqs e da opções para definição de dist,tam e centralseq
		JPanel externalJPanel = new JPanel(new BorderLayout());
		externalJPanel.setBorder(BorderFactory.createLoweredBevelBorder());
		JPanel jp = new JPanel(new GridBagLayout());
		jp.setBorder(new EmptyBorder(10,10,10,10));
		externalJPanel.add(jp,BorderLayout.CENTER);

		JLabel titleLabel = new JLabel("Custom Hunt");
		titleLabel.setFont(titleLabel.getFont().deriveFont(titleLabel.getFont().getStyle() | Font.BOLD));
		GridBagConstraints c = new GridBagConstraints();
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.10;
	    c.weightx = 1;
	    c.gridx = 1;
	    c.gridy = 0;
	    c.gridwidth = 2;
	    c.anchor = GridBagConstraints.EAST;
		jp.add(titleLabel,c);
	    
		c = new GridBagConstraints();
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.10;
	    c.weightx = 0.1;
	    c.gridx = 0;
	    c.gridy = 1;
	    c.gridwidth = 1;
		jp.add(new JLabel("Base sequence: "),c);
		
		JTextField baseTextField = new JTextField();
		baseTextField.setText(centralCutSeq);
		baseTextField.addActionListener(new ActionListener() {
			
			@Override
			public void actionPerformed(ActionEvent e) {
				setCentralCutSeq(((JTextField)e.getSource()).getText());
				((JFullSeqReportTableModel) table.getModel()).update(getCentralCutSeq(),dist,tam);
				if(!((JFullSeqReportTableModel) table.getModel()).isReady()){
					searchButton.setEnabled(false);
				}else{
					searchButton.setEnabled(true);
				}
			}
		});
		c = new GridBagConstraints();
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.10;
	    c.weightx = 0.9;
	    c.gridx = 1;
	    c.gridy = 1;
	    c.gridwidth = 1;
		jp.add(baseTextField,c);
		
		SpinnerModel spinnermodelTam = new SpinnerNumberModel(0, 0, 1000000, 1);
		SpinnerModel spinnermodelDist = new SpinnerNumberModel(0, 0, 1000000, 1);
		JSpinner spinnerTam = new JSpinner(spinnermodelTam); 
		JSpinner spinnerDist = new JSpinner(spinnermodelDist);
		Dimension d = spinnerTam.getPreferredSize();  
		d.width = 50;  
		spinnerTam.setPreferredSize(d);  
		spinnerDist.setPreferredSize(d);  
		spinnerTam.setEnabled(true);
		spinnerDist.setEnabled(true);
		spinnerTam.addChangeListener(new ChangeListener() {
			// Seta tamanho da região 5l
			@Override
			public void stateChanged(ChangeEvent arg0) {
				Object obj = arg0.getSource();
				if(obj instanceof JSpinner){
					JSpinner spinner = (JSpinner) obj;
					tam = (Integer)spinner.getValue();
					((JFullSeqReportTableModel) table.getModel()).update(getCentralCutSeq(),dist,tam);
					if(!((JFullSeqReportTableModel) table.getModel()).isReady()){
						//spinner.setValue(0);
						searchButton.setEnabled(false);
					}else{
						searchButton.setEnabled(true);
					}
				}
			}
		});
		spinnerDist.addChangeListener(new ChangeListener(){
			// Seta posição da região 5l
			@Override
			public void stateChanged(ChangeEvent arg0) {
				Object obj = arg0.getSource();
				if(obj instanceof JSpinner){
					JSpinner spinner = (JSpinner) obj;
					dist = (Integer)spinner.getValue();
					((JFullSeqReportTableModel) table.getModel()).update(getCentralCutSeq(),dist,tam);
					if(!((JFullSeqReportTableModel) table.getModel()).isReady()){
						//spinner.setValue(0);
						searchButton.setEnabled(false);
					}else{
						searchButton.setEnabled(true);
					}
				}
			}
		});

		c = new GridBagConstraints();
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.10;
	    c.weightx = 0.25;
	    c.gridx = 0;
	    c.gridy = 3;
	    c.gridwidth = 1;
		jp.add(new JLabel("Distance:"),c);
		
		c = new GridBagConstraints();
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.10;
	    c.weightx = 0.25;
	    c.gridx = 1;
	    c.gridy = 3;
	    c.gridwidth = 1;
		jp.add(spinnerDist,c);
		
		c = new GridBagConstraints();
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.10;
	    c.weightx = 0.25;
	    c.gridx = 2;
	    c.gridy = 3;
	    c.gridwidth = 1;
		jp.add(new JLabel("Length: "),c);
		
		c = new GridBagConstraints();
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.10;
	    c.weightx = 0.25;
	    c.gridx = 3;
	    c.gridy = 3;
	    c.gridwidth = 1;
		jp.add(spinnerTam,c);

		try {
			table = new JTable(new JFullSeqReportTableModel(dbm, centralCutSeq, dist, tam));
		} catch (Exception e) {
			Drawer.writeToLog(e.getMessage());
		}
		table.getColumnModel().getColumn(0).setMaxWidth(80);
		JScrollPane jscp = new JScrollPane(table);
		JScrollBar jsb = jscp.getVerticalScrollBar();
		jsb.addAdjustmentListener(new AdjustmentListener(){
			@Override
			public void adjustmentValueChanged(AdjustmentEvent e) {
				JScrollBar jsb = (JScrollBar) e.getSource();
				int jsbMax = jsb.getMaximum();
				int jsbPos = jsb.getValue();
				System.err.println(jsbPos+"/"+jsbMax+" - "+(float)(jsbPos)*100/jsbMax+"%");
				if(jsbMax*0.6 <= jsbPos){
					System.err.println("Loading...");
					((JFullSeqReportTableModel)table.getModel()).customHuntLoad();
				}
			}					
		});
		table.setAutoscrolls(true);
		c = new GridBagConstraints();
	    c.fill = GridBagConstraints.BOTH;
	    c.weighty = 0.70;
	    c.weightx = 1;
	    c.gridx = 0;
	    c.gridy = 4;
	    c.gridwidth = 4;
		jp.add(jscp,c);
		
		
		searchButton.addActionListener(this);
		c = new GridBagConstraints();
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.05;
	    c.weightx = 0.3;
	    c.gridx = 3;
	    c.gridy = 5;
	    c.gridwidth = 1;
	    jp.add(searchButton,c);
		
		
		JPanel mainJPanel = (JPanel) reportTab.getComponentAt(mainTabIndex);
		mainTab = (JTabbedPane)mainJPanel.getComponent(mainJPanel.getComponentCount() -1);
		
		mainTab.add("Custom Search",externalJPanel);
		mainTab.setSelectedIndex(mainTab.getTabCount()-1);
		
		owner.setReportAdded();
	}
	
	@Override
	public void actionPerformed(ActionEvent e) {
		CustomFiveCutReportWorker worker = new CustomFiveCutReportWorker(owner,dbm,getCentralCutSeq(),dist,tam,mainTabIndex,data,tabNames,reportName,reportTab);
		mainTab.removeTabAt(mainTab.getTabCount()-1);
		owner.startWaitDialog();
		worker.start();
	}

	private String getCentralCutSeq() {
		return centralCutSeq;
	}

	private void setCentralCutSeq(String centralCutSeq) {
		this.centralCutSeq = centralCutSeq;
	}
	

}
