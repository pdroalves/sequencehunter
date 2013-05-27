 package gui;

import gui.toolbar.OpenReportFileFilter;
import gui.workers.ReportAddWorker;
import hunt.Evento;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Observable;
import java.util.Observer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;

import database.DBManager;
import dialogs.WaitDialogHandler;

import auxiliares.RemovableTabComponent;

import tables.report.Report;
import tables.report.TabledReport;
import tables.report.TextReport;
import xml.TranslationsManager;

public class ReportDrawer extends Observable implements ActionListener{
	private static JPanel reportContainer;
	private static JTabbedPane reportTab;
	private static JPanel emptyReportTab;
	private static TranslationsManager tm;
	private TabledReport tabledreport;
	private static List<List<Report>> data;
	private static List<List<String>> tabNames;
	private static List<String> reportName;
	private static WaitDialogHandler waitdialog;

	public ReportDrawer(){
		super();
		reportContainer = new JPanel(new BorderLayout());
		ReportDrawer.tm = TranslationsManager.getInstance();
		reportTab = new JTabbedPane(JTabbedPane.NORTH,JTabbedPane.SCROLL_TAB_LAYOUT);
		emptyReportTab = getEmptyJPanel();
		reportContainer.add(reportTab,BorderLayout.CENTER);
		reportContainer.add(emptyReportTab,BorderLayout.NORTH);
		updateReportsView();
		data = new ArrayList<List<Report>>();
		tabNames = new ArrayList<List<String>>();
		reportName = new ArrayList<String>();
	}
	
	public void removeReport(int index){
		reportTab.remove(index);
		List<Report> reports = data.get(index);
		Iterator<Report> iterator = reports.iterator();
		while(iterator.hasNext()){
			Report report = iterator.next();
			if(report instanceof TabledReport){
				TabledReport tr = (TabledReport) report;
				tr.getDBM().destroy();
			}
			data.remove(report);
		}
		data.remove(index);
		tabNames.get(index).clear();
		tabNames.remove(index);
		reportName.remove(index);		
	}

	public JPanel getContainer(){
		return reportContainer;
	}

	protected void addMainReport(String libDatabase,File log){
		// Inicia wait dialog
		Drawer.writeToLog(tm.getText("LoadingReport"));
		
		waitdialog = new WaitDialogHandler(Drawer.getJFrame(),this);
		waitdialog.start();
		ReportAddWorker worker = new ReportAddWorker(this,libDatabase,log,data,tabNames,reportName,reportTab);
		worker.start();
		return;
	}

	protected static void addSubReport(String query,String mainLibDatabase){
		// Cria tabledReport com dados relativos a uma query em cima de determinada db
		// To-do
	}

	public  void updateReportsView(){
		if(reportTab.getTabCount() == 0){
			reportTab.setVisible(false);
			emptyReportTab.setVisible(true);
		}else{
			initTabsComponents(reportTab);
			reportTab.setVisible(true);
			emptyReportTab.setVisible(false);
		}
		
		reportContainer.repaint();
		Drawer.repaint();
	}
	
	public void setReportAdded(DBManager dbm){
		super.setChanged();
		super.notifyObservers(dbm);
		updateReportsView();
		Drawer.moveToReportTab();
		Drawer.enableProgressBar(false);
		if(waitdialog != null){
			waitdialog.dispose();
		}
	}

	private static JPanel getEmptyJPanel(){
		JPanel panel = new JPanel(new FlowLayout());
		JLabel emptyLabel = new JLabel(tm.getText("reportNothingToShow"));
		panel.add(emptyLabel);
		return panel;
	}

	private  void initTabsComponents(JTabbedPane pane){
		if(pane != null){
			for(int i=0;i < pane.getTabCount();i++){
				pane.setTabComponentAt(i, new RemovableTabComponent(this,pane,i));
			}
		}
	}
	@Override
	public void actionPerformed(ActionEvent ae) {
		switch(ae.getActionCommand()){
		case "Open":
			// Abre arquivo
			JFileChooser jfc = new JFileChooser();
			jfc.setFileFilter(new OpenReportFileFilter());
			jfc.setMultiSelectionEnabled(false);
			if(jfc.showOpenDialog(null) == JFileChooser.APPROVE_OPTION){
				// O arquivo selecionado pode ser do tipo .db
				Pattern databasePattern = Pattern.compile(".*[.][d][b]");
				Matcher databaseMatcher = databasePattern.matcher(jfc.getSelectedFile().getName());
				if(databaseMatcher.find()){
					Drawer.setProgressBar(5);
					Drawer.enableProgressBar(true);
					Drawer.updateProgressBar(1);
					String filepath = jfc.getSelectedFile().getAbsolutePath();
					addMainReport(filepath,null);
					updateReportsView();
				}
			}
			break;
		}

	}
	public static int getReportsLoaded(){
		return reportTab.getTabCount();
	}

	public static int getSelectedReportTabIndex(){
		return reportTab.getSelectedIndex();
	}

	public static int getSelectedSubReportTabIndex(){		
		JPanel jp = (JPanel) reportTab.getComponentAt(getSelectedReportTabIndex());
		JTabbedPane jtp = (JTabbedPane) jp.getComponent(0);
		return jtp.getSelectedIndex();
	}

	public static ArrayList<Evento> getData(int mainIndex,int subIndex){
		Object obj = data.get(mainIndex).get(subIndex);
		if(obj instanceof DBManager){
			DBManager dbm = (DBManager) obj;
			return dbm.getEvents();
		}else{
			return null;
		}
	}
	
	public static File getLog(int mainIndex,int subIndex){
		Object obj = data.get(mainIndex).get(subIndex);
		if(obj instanceof File){
			return (File)obj;
		}else{
			return null;
		}
	}
	
	public static String getTabName(int mainIndex,int subIndex){
		// Retorna o nome de uma subtab
		return tabNames.get(mainIndex).get(subIndex);
	}
	
	public static List<List<String>> getAllTabNames(){
		// Retorna o nome de subtabs
		return tabNames;
	}
	
	public static String getReportTitle(int mainIndex){
		return reportName.get(mainIndex);
	}
	
	public static List<String> getAllReportTitles(){
		return reportName;
	}
	
	public static Object getReport(int mainReport,int subReport){
		Object obj = data.get(mainReport).get(subReport);
		if(obj instanceof TabledReport){
			return ((TabledReport) obj).getDBM();
		}else{
			return ((TextReport) obj).getFile();
		}
	}
}
