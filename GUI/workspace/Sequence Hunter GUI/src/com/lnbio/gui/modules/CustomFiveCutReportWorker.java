package com.lnbio.gui.modules;


import java.util.List;

import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;

import com.lnbio.database.DBManager;
import com.lnbio.gui.Drawer;
import com.lnbio.gui.ReportDrawer;
import com.lnbio.tables.report.JFiveCutQueryReportTableModel;
import com.lnbio.tables.report.Report;
import com.lnbio.tables.report.ReportFactory;
import com.lnbio.tables.report.TabledReport;
import com.lnbio.xml.TranslationsManager;


public class CustomFiveCutReportWorker extends Thread{

	private TranslationsManager tm;
	private List<List<Report>> data;
	private List<List<String>> tabNames;
	private JTabbedPane reportTab;
	private String centralCutSeq;
	private int mainTabIndex;
	private ReportDrawer owner;
	private long dist;
	private long tam;
	private DBManager dbm;
	
	public CustomFiveCutReportWorker(ReportDrawer owner,DBManager dbm,String centralCutSeq,long dist,long tam,int mainTabIndex,List<List<Report>> data,List<List<String>> tabNames,List<String> reportName,JTabbedPane reportTab){
		tm = TranslationsManager.getInstance();
		this.owner = owner;
		this.dbm = dbm;
		this.data = data;
		this.tabNames = tabNames;
		this.reportTab = reportTab;
		this.centralCutSeq = centralCutSeq;
		this.mainTabIndex = mainTabIndex;
		this.dist = dist;
		this.tam = tam;
	}

	public void run(){
		try {
			// Cria tabela com dados de five cut sequences referentes a uma central cut sequence arbitraria
			System.err.println("Adicionando five cut unpaired para "+centralCutSeq);
			ReportFactory rf = new ReportFactory();

			JFiveCutQueryReportTableModel jfcrtm;
			jfcrtm = new JFiveCutQueryReportTableModel(dbm,centralCutSeq,dist,tam);
			TabledReport tabledreportUnpaired = rf.createTabledReport("custom five cut unpaired",dbm,jfcrtm);	

			data.get(data.size()-1).add(tabledreportUnpaired);
			JComponent jc = tabledreportUnpaired.getComponent();
			
			String tabName = tm.getText("reportCustomFiveCutName")+" - "+centralCutSeq;
			tabNames.get(tabNames.size()-1).add(tabName);
			JPanel mainJPanel = (JPanel) reportTab.getComponentAt(mainTabIndex);
			JTabbedPane mainTab = (JTabbedPane)mainJPanel.getComponent(0);
			
			mainTab.addTab(tabName,jc);
			mainTab.setSelectedIndex(mainTab.getTabCount()-1);

			// Observadores
			dbm.addObserver(jfcrtm);		
			dbm.addObserver(tabledreportUnpaired);
		} catch (Exception e) {
			String error = tm.getText("statusErrorProcessingMsg")+" "+e.getMessage();
			System.err.println(error);
			Drawer.writeToLog(error);
		}
		owner.setReportAdded();
		return;
	}
}
