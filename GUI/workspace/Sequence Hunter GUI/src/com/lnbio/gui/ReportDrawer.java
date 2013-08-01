package com.lnbio.gui;


import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.FlowLayout;
import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Observable;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.BorderFactory;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTabbedPane;
import javax.swing.border.EmptyBorder;

import com.lnbio.auxiliares.RemovableTabComponent;
import com.lnbio.database.DBManager;
import com.lnbio.dialogs.WaitDialogHandler;
import com.lnbio.gui.modules.CustomReportBuilderWorker;
import com.lnbio.gui.modules.OpenReportFileFilter;
import com.lnbio.gui.modules.ReportWorker;
import com.lnbio.hunt.Evento;
import com.lnbio.tables.report.Report;
import com.lnbio.tables.report.TabledReport;
import com.lnbio.tables.report.TextReport;
import com.lnbio.xml.TranslationsManager;

public class ReportDrawer extends Observable{
	private static JPanel reportContainer;
	private static JTabbedPane reportTab;
	private static JPanel emptyReportTab;
	private static TranslationsManager tm;
	private static List<List<Report>> data;
	private static List<List<String>> tabNames;
	private static List<String> reportName;
	private static WaitDialogHandler waitdialog;

	public ReportDrawer(){
		super();
		reportContainer = new JPanel(new BorderLayout());
		reportContainer.setBorder(BorderFactory.createLineBorder(Color.gray));
		ReportDrawer.tm = TranslationsManager.getInstance();
		reportTab = new JTabbedPane(JTabbedPane.NORTH,JTabbedPane.SCROLL_TAB_LAYOUT);
		reportTab.setBorder(new EmptyBorder(10,10,10,10));
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
		// Pega os subreports vinculados ao report main
		List<Report> reports = data.get(index);
		Iterator<Report> iterator = reports.iterator();
		while(iterator.hasNext()){
			Report report = iterator.next();
			if(report instanceof TabledReport){
				TabledReport tr = (TabledReport) report;
				tr.getDBM().destroy();
			}
		}
		reports.removeAll(reports);
		data.remove(index);
		System.out.println("Oi");
		tabNames.get(index).clear();
		tabNames.remove(index);
		reportName.remove(index);	
		this.setChanged();
		this.notifyObservers();
	}

	public JPanel getContainer(){
		return reportContainer;
	}

	public void addMainReport(String libDatabase,File log){
		// Inicia wait dialog
		Drawer.writeToLog(tm.getText("LoadingReport"));
		waitdialog = new WaitDialogHandler(Drawer.getJFrame(),this);
		waitdialog.start();
		ReportWorker worker = new ReportWorker(this,libDatabase,log,data,tabNames,reportName,reportTab);
		worker.start();
		return;
	}


	public void addFiveCutSubReport(DBManager dbm,int mainTabIndex,String centralCutSeq) throws Exception{
		// Inicia wait dialog
		//Drawer.writeToLog(tm.getText("LoadingReport"));
		//waitdialog = new WaitDialogHandler(Drawer.getJFrame(),this);
		//waitdialog.start();
		CustomReportBuilderWorker worker = new CustomReportBuilderWorker(this, dbm,centralCutSeq, mainTabIndex, data, tabNames, reportName, reportTab);
		worker.start();
		return;
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

	public void setReportAdded(){
		super.setChanged();
		super.notifyObservers();
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
	public void openLibrary(){
		System.out.println("Report!");
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
			return dbm.getCentralCutEvents();
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
