package gui.workers;

import gui.Drawer;
import gui.ReportDrawer;
import histogram.EventHistogram;

import java.awt.BorderLayout;
import java.io.File;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JComponent;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;

import tables.report.JPartialReportTableModel;
import tables.report.JTotalReportTableModel;
import tables.report.Report;
import tables.report.ReportFactory;
import tables.report.TabledReport;
import tables.report.TextReport;
import xml.TranslationsManager;
import database.DBManager;

public class ReportAddWorker extends Thread{

	private TranslationsManager tm;
	private String libDatabase;
	private File log;
	private List<List<Report>> data;
	private List<List<String>> tabNames;
	private ReportDrawer owner;
	private List<String> reportName;
	private JTabbedPane reportTab;

	public ReportAddWorker(ReportDrawer owner,String libDatabase,File log,List<List<Report>> data,List<List<String>> tabNames,List<String> reportName,JTabbedPane reportTab){
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
				dbm = new DBManager(libDatabase);

				// Central Cut paired
				System.err.println("Adicionando central cut paired");
				JPartialReportTableModel jprtm = new JPartialReportTableModel(dbm);
				TabledReport tabledreportPaired = rf.createTabledReport("central cut paired",dbm,jprtm);	
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
				data.get(data.size()-1).add(tabledreportUnpaired);
				jc = tabledreportUnpaired.getComponent();
				tabName = tm.getText("reportCentralCutUnpairedDefaultName");
				tabNames.get(tabNames.size()-1).add(tabName);
				jtp.addTab(tabName,jc);
				// Observadores
				dbm.addObserver(jtrtm);
				dbm.addObserver(tabledreportUnpaired);
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
			Drawer.writeToLog(TranslationsManager.getInstance().getText("CouldntLoad"));
		}
		owner.setReportAdded();
		return;
	}
}
