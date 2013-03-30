package gui;

import auxiliares.WaitLayerUI;
import gui.toolbar.OpenReportFileFilter;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.Box;
import javax.swing.JComponent;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JLayer;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.ListSelectionModel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import database.DBManager;

import auxiliares.RemovableTabComponent;

import tables.report.JPartialReportTableModel;
import tables.report.JTotalReportTableModel;
import xml.TranslationsManager;

import gui.report.*;

public class ReportDrawer implements ActionListener, Observer{
	private static JPanel reportContainer;
	private static JTabbedPane reportTab;
	private static JPanel emptyReportTab;
	private static TranslationsManager tm;
	private TabledReport tabledreport;

	public ReportDrawer(){
		reportContainer = new JPanel(new BorderLayout());
		ReportDrawer.tm = TranslationsManager.getInstance();
		reportTab = new JTabbedPane(JTabbedPane.NORTH,JTabbedPane.SCROLL_TAB_LAYOUT);
		emptyReportTab = getEmptyJPanel();
		reportContainer.add(reportTab,BorderLayout.CENTER);
		reportContainer.add(emptyReportTab,BorderLayout.NORTH);
		updateReportsView();
	}

	public JPanel getContainer(){
		return reportContainer;
	}

	protected void addMainReport(String libDatabase,File log){
		// Inicia wait dialog
		JPanel jp = new JPanel();
		jp.setLayout(new BorderLayout());
		JTabbedPane jtp = new JTabbedPane(JTabbedPane.LEFT,JTabbedPane.SCROLL_TAB_LAYOUT);	
		
		// Report	
		JComponent jc;
		if(libDatabase != null){
			DBManager dbm = new DBManager(libDatabase);
			dbm.addObserver(this);

			// Central Cut paired
			JPartialReportTableModel jprtm = new JPartialReportTableModel(dbm);
			dbm.addObserver(jprtm);
			tabledreport = new TabledReport(dbm,jprtm);
			dbm.addObserver(tabledreport);
			jc = tabledreport.createTabledReport();
			jtp.addTab(tm.getText("reportCentralCutPairedDefaultName"),jc);

			// Central Cut unpaired
			JTotalReportTableModel jtrtm = new JTotalReportTableModel(dbm);
			dbm.addObserver(jtrtm);
			tabledreport = new TabledReport(dbm,jtrtm);
			dbm.addObserver(tabledreport);
			jc = tabledreport.createTabledReport();
			jtp.addTab(tm.getText("reportCentralCutUnpairedDefaultName"),jc);
		}
		
		// Log Report
		if(log != null){
			jc = createTextReport(log);
			jtp.addTab(tm.getText("reportHuntLogDefaultName"), jc);
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

}
