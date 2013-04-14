package gui;

import gui.toolbar.OpenReportFileFilter;
import hunt.Evento;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;
import java.util.Observable;
import java.util.Observer;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTextArea;

import database.DBManager;

import auxiliares.RemovableTabComponent;

import tables.report.JPartialReportTableModel;
import tables.report.JTotalReportTableModel;
import tables.report.TabledReport;
import xml.TranslationsManager;

public class ReportDrawer implements ActionListener, Observer{
	private static JPanel reportContainer;
	private static JTabbedPane reportTab;
	private static JPanel emptyReportTab;
	private static TranslationsManager tm;
	private TabledReport tabledreport;
	@SuppressWarnings("rawtypes")
	private static List<List> data;
	private static List<List<String>> tabNames;

	@SuppressWarnings("rawtypes")
	public ReportDrawer(){
		reportContainer = new JPanel(new BorderLayout());
		ReportDrawer.tm = TranslationsManager.getInstance();
		reportTab = new JTabbedPane(JTabbedPane.NORTH,JTabbedPane.SCROLL_TAB_LAYOUT);
		emptyReportTab = getEmptyJPanel();
		reportContainer.add(reportTab,BorderLayout.CENTER);
		reportContainer.add(emptyReportTab,BorderLayout.NORTH);
		updateReportsView();
		data = new ArrayList<List>();
		tabNames = new ArrayList<List<String>>();
	}

	public JPanel getContainer(){
		return reportContainer;
	}

	@SuppressWarnings("unchecked")
	protected void addMainReport(String libDatabase,File log){
		// Inicia wait dialog
		JPanel jp = new JPanel();
		jp.setLayout(new BorderLayout());
		JTabbedPane jtp = new JTabbedPane(JTabbedPane.LEFT,JTabbedPane.SCROLL_TAB_LAYOUT);	
		data.add(new ArrayList<DBManager>());
		tabNames.add(new ArrayList<String>());

		// Report	
		JComponent jc;
		String tabName;
		if(libDatabase != null){
			DBManager dbm = new DBManager(libDatabase);

			data.get(data.size()-1).add(dbm);
			dbm.addObserver(this);

			// Central Cut paired
			JPartialReportTableModel jprtm = new JPartialReportTableModel(dbm);
			dbm.addObserver(jprtm);
			tabledreport = new TabledReport(dbm,jprtm);
			dbm.addObserver(tabledreport);
			jc = tabledreport.createTabledReport();
			tabName = tm.getText("reportCentralCutPairedDefaultName");
			tabNames.get(tabNames.size()-1).add(tabName);
			jtp.addTab(tabName,jc);

			// Central Cut unpaired
			data.get(data.size()-1).add(dbm);
			JTotalReportTableModel jtrtm = new JTotalReportTableModel(dbm);
			dbm.addObserver(jtrtm);
			tabledreport = new TabledReport(dbm,jtrtm);
			dbm.addObserver(tabledreport);
			jc = tabledreport.createTabledReport();
			tabName = tm.getText("reportCentralCutUnpairedDefaultName");
			tabNames.get(tabNames.size()-1).add(tabName);
			jtp.addTab(tabName,jc);
		}

		// Log Report
		if(log != null){
			jc = createTextReport(log);
			data.get(data.size()-1).add(log);
			tabName = tm.getText("reportHuntLogDefaultName");
			tabNames.get(tabNames.size()-1).add(tabName);
			jtp.addTab(tabName, jc);
		}

		/*JPanel insideJp = new JPanel();
		insideJp.setLayout(new BorderLayout());
		insideJp.add(seqInfo,BorderLayout.EAST);
		insideJp.add(jtp,BorderLayout.CENTER);*/

		jp.add(jtp,BorderLayout.CENTER);

		reportTab.addTab(libDatabase,jp);
		reportTab.setSelectedIndex(reportTab.getTabCount()-1);	
		return;
	}

	protected static void addSubReport(String query,String mainLibDatabase){
		// Cria tabledReport com dados relativos a uma query em cima de determinada db
		// To-do
	}


	private static JComponent createTextReport(File txtFile){
		final JTextArea logJTA = new JTextArea();
		logJTA.setLineWrap(true);
		logJTA.setWrapStyleWord(true);
		try {
			Scanner scLog = new Scanner(txtFile);
			while(scLog.hasNextLine()){
				String linha = scLog.nextLine();
				logJTA.append(linha+"\n");
			}
			scLog.close();
			JScrollPane jscrlp = new JScrollPane(logJTA);
			return jscrlp;
		} catch (FileNotFoundException e1) {
			Drawer.writeToLog(txtFile.getAbsolutePath()+tm.getText("fileNotFound"));
			return null;
		}
	}

	public static void updateReportsView(){
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

	private static JPanel getEmptyJPanel(){
		JPanel panel = new JPanel(new FlowLayout());
		JLabel emptyLabel = new JLabel(tm.getText("reportNothingToShow"));
		panel.add(emptyLabel);
		return panel;
	}

	private static void initTabsComponents(JTabbedPane pane){
		if(pane != null){
			for(int i=0;i < pane.getTabCount();i++){
				pane.setTabComponentAt(i, new RemovableTabComponent(pane,i));
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
				Pattern databasePattern = Pattern.compile(".db");
				Matcher databaseMatcher = databasePattern.matcher(jfc.getSelectedFile().getName());
				if(databaseMatcher.find()){
					Drawer.setProgressBar(5);
					Drawer.enableProgressBar(true);
					Drawer.updateProgressBar(1);
					addMainReport(jfc.getSelectedFile().getAbsolutePath(),null);
					updateReportsView();
					Drawer.updateProgressBar(5);
					Drawer.moveToReportTab();
					Drawer.enableProgressBar(false);
				}
			}
			break;
		}

	}

	@Override
	public void update(Observable arg0, Object arg1) {
		updateReportsView();
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
		return tabNames.get(mainIndex).get(subIndex);
	}

}
