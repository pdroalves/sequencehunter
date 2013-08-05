package com.lnbio.gui.modules;


import java.awt.BorderLayout;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.JTable;

import com.lnbio.database.DBManager;
import com.lnbio.gui.Drawer;
import com.lnbio.gui.ReportDrawer;
import com.lnbio.tables.report.JPartialReportTableModel;
import com.lnbio.tables.report.JTotalReportTableModel;
import com.lnbio.tables.report.Report;
import com.lnbio.tables.report.ReportFactory;
import com.lnbio.tables.report.TabledReport;
import com.lnbio.tables.report.TextReport;
import com.lnbio.xml.TranslationsManager;


public class ReportWorker extends Thread{

	private TranslationsManager tm;
	private String libDatabase;
	private File log;
	private List<List<Report>> data;
	private List<List<String>> tabNames;
	private ReportDrawer owner;
	private List<String> reportName;
	private JTabbedPane reportTab;

	public ReportWorker(ReportDrawer owner,String libDatabase,File log,List<List<Report>> data,List<List<String>> tabNames,List<String> reportName,JTabbedPane reportTab){
		tm = TranslationsManager.getInstance();
		this.owner = owner;
		this.libDatabase = libDatabase;
		this.log = log;
		this.data = data;
		this.tabNames = tabNames;
		this.reportName = reportName;
		this.reportTab = reportTab;
	}

	public void run(){
		// Inicia wait dialog
		JPanel jp = new JPanel();
		jp.setLayout(new BorderLayout());
		JTabbedPane jtp = new JTabbedPane(JTabbedPane.LEFT,JTabbedPane.SCROLL_TAB_LAYOUT);	
		data.add(new ArrayList<Report>());
		tabNames.add(new ArrayList<String>());
		ReportFactory rf = new ReportFactory();

		// Report	
		JComponent jc;
		String tabName;
		DBManager dbm = null;
		try{
			if(libDatabase != null){
				dbm = new DBManager();
				dbm.setDBFile(libDatabase);

				// Central Cut paired
				System.err.println("Adicionando central cut paired");
				JPartialReportTableModel jprtm = new JPartialReportTableModel(dbm);
				TabledReport tabledreportPaired = rf.createTabledReport("central cut paired",dbm,jprtm);	
				JTable table = tabledreportPaired.getJTable();
				if(dbm.isCustomFiveCutSupported())
					table.addMouseListener(new CentralCutMenuMouseAdapter(owner,dbm,reportTab.getTabCount(),table));
				data.get(data.size()-1).add(tabledreportPaired);
				jc = tabledreportPaired.getComponent();
				tabName = tm.getText("reportCentralCutPairedDefaultName");
				tabNames.get(tabNames.size()-1).add(tabName);
				jtp.addTab(tabName,jc);
				// Observadores
				dbm.addObserver(jprtm);		
				dbm.addObserver(tabledreportPaired);

				// Central Cut unpaired
				System.err.println("Adicionando central cut unpaired");
				JTotalReportTableModel jtrtm = new JTotalReportTableModel(dbm);
				TabledReport tabledreportUnpaired = rf.createTabledReport("central cut unpaired",dbm,jtrtm);
				table = tabledreportUnpaired.getJTable();
				if(dbm.isCustomFiveCutSupported())
					table.addMouseListener(new CentralCutMenuMouseAdapter(owner,dbm,reportTab.getTabCount(),table));
				data.get(data.size()-1).add(tabledreportUnpaired);
				jc = tabledreportUnpaired.getComponent();
				tabName = tm.getText("reportCentralCutUnpairedDefaultName");
				tabNames.get(tabNames.size()-1).add(tabName);
				jtp.addTab(tabName,jc);
				// Observadores
				dbm.addObserver(jtrtm);
				dbm.addObserver(tabledreportUnpaired);

				if(dbm.isFiveCutSupported()){
					// Five Cut paired
					System.err.println("Adicionando five cut paired");
					jprtm = new JPartialReportTableModel(dbm,JPartialReportTableModel.FIVE_CUT);
					tabledreportPaired = rf.createTabledReport("five cut paired",dbm,jprtm);	
					data.get(data.size()-1).add(tabledreportPaired);
					jc = tabledreportPaired.getComponent();
					tabName = tm.getText("reportFiveCutPairedDefaultName");
					tabNames.get(tabNames.size()-1).add(tabName);
					jtp.addTab(tabName,jc);
					// Observadores
					dbm.addObserver(jprtm);		
					dbm.addObserver(tabledreportPaired);

					// Five Cut unpaired
					System.err.println("Adicionando five cut unpaired");
					jtrtm = new JTotalReportTableModel(dbm,JTotalReportTableModel.FIVE_CUT);
					tabledreportUnpaired = rf.createTabledReport("five cut unpaired",dbm,jtrtm);	
					data.get(data.size()-1).add(tabledreportUnpaired);
					jc = tabledreportUnpaired.getComponent();
					tabName = tm.getText("reportFiveCutUnpairedDefaultName");
					tabNames.get(tabNames.size()-1).add(tabName);
					jtp.addTab(tabName,jc);
					// Observadores
					dbm.addObserver(jtrtm);		
					dbm.addObserver(tabledreportUnpaired);
				}

			}

			// Log Report
			if(log != null){
				System.err.println("Adicionando hunt log");
				TextReport tr = rf.createTextReport(log);
				jc = tr.getReport();
				data.get(data.size()-1).add(tr);
				tabName = tm.getText("reportHuntLogDefaultName");
				tabNames.get(tabNames.size()-1).add(tabName);
				jtp.addTab(tabName, jc);
			}

			/*JPanel insideJp = new JPanel();
		insideJp.setLayout(new BorderLayout());
		insideJp.add(seqInfo,BorderLayout.EAST);
		insideJp.add(jtp,BorderLayout.CENTER);*/

			jp.add(jtp,BorderLayout.CENTER);

			String reportTitle = libDatabase;
			reportName.add(reportTitle);
			reportTab.addTab(reportTitle,jp);
			reportTab.setSelectedIndex(reportTab.getTabCount()-1);
		}catch(Exception e){
			Drawer.writeToLog(TranslationsManager.getInstance().getText("CouldntLoad")+" "+e.getMessage());
		}
		owner.setReportAdded();
		return;
	}
}
