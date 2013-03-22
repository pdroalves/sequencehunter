package gui;

import gui.toolbar.OpenReportFileFilter;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.Box;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.ListSelectionModel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import auxiliares.RemovableTabComponent;

import tables.JReportTableModel;

public class ReportDrawer implements ActionListener{
	private static JPanel reportContainer;
	private static JTabbedPane reportTab;
	private static Boolean noReports = true;

	public ReportDrawer(){
		reportContainer = new JPanel();
		drawEmptyReportContainer();
	}

	public JPanel getContainer(){
		return reportContainer;
	}

	protected static void addReport(String libDatabase,File log){
		JPanel jp = new JPanel();
		jp.setLayout(new BorderLayout());
		JTabbedPane jtp = new JTabbedPane(JTabbedPane.LEFT,JTabbedPane.SCROLL_TAB_LAYOUT);	

		// Report		
		// Central Cut
		final JReportTableModel jrtm = new JReportTableModel(libDatabase);
		final JTable jte = new JTable(jrtm); 
		jte.setAutoCreateRowSorter(false);
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
					jrtm.loadData();
				}
			}					
		});
		jte.setAutoscrolls(true);

		Box seqInfo = Box.createVerticalBox();
		seqInfo.add(new JLabel("Sequence: "));
		seqInfo.add(seqJLabel);
		seqInfo.add(new JLabel("Sequence frequency: "));
		seqInfo.add(seqFreqJLabel);

		// Log Report
		if(log != null){
			final JTextArea logJTA = new JTextArea();
			logJTA.setLineWrap(true);
			logJTA.setWrapStyleWord(true);
			try {
				Scanner scLog = new Scanner(log);
				while(scLog.hasNextLine()){
					String linha = scLog.nextLine();
					logJTA.append(linha+"\n");
				}
				scLog.close();
				JScrollPane jscrlp = new JScrollPane(logJTA);
				jtp.addTab("Hunt log", jscrlp);
			} catch (FileNotFoundException e1) {
				Drawer.writeToLog("File "+log.getAbsolutePath()+" could not be read.");
			}
		}

		jtp.addTab("Central Cut",jscp);

		JPanel insideJp = new JPanel();
		insideJp.setLayout(new BorderLayout());
		insideJp.add(seqInfo,BorderLayout.EAST);
		insideJp.add(jtp,BorderLayout.CENTER);

		jp.add(insideJp,BorderLayout.CENTER);

		if(noReports){
			reportContainer.removeAll();
			reportContainer.setLayout(new BorderLayout());
			reportTab = new JTabbedPane(JTabbedPane.TOP,JTabbedPane.SCROLL_TAB_LAYOUT);
			reportContainer.add(reportTab);
			noReports = false;
		}
		reportTab.addTab(libDatabase,jp);
		reportTab.setSelectedIndex(reportTab.getTabCount()-1);	
		initTabsComponents(reportTab);
		return;
	}

	public static void updateReportTabs(){
		reportContainer.repaint();
		initTabsComponents(reportTab);
		if(reportTab.getTabCount() == 0){
			drawEmptyReportContainer();
			noReports = true;
		}
	}
	
	private static void drawEmptyReportContainer(){
		reportContainer.removeAll();
		reportContainer.setLayout(new FlowLayout());
		JLabel emptyLabel = new JLabel("No report to show");
		reportContainer.add(emptyLabel);
		reportContainer.repaint();
		return;
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
				if(databaseMatcher.find())
					addReport(jfc.getSelectedFile().getAbsolutePath(),null);
				Drawer.moveToReportTab();
			}
			break;
		}

	}

}
