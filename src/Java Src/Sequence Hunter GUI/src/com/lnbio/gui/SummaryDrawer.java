package com.lnbio.gui;


import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.util.ArrayList;
import java.util.Observable;
import java.util.Observer;

import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;

import com.lnbio.hunt.Hunter;
import com.lnbio.xml.TranslationsManager;


public class SummaryDrawer implements ActionListener,Observer {
	private static String searchSeq;
	private static JLabel searchSeqJLabel;
	private static JTextField outputfolderTF;
	private static Hunter h;
	private static JButton startButton;
	private static JButton abortButton;
	private JPanel summaryContainer;
	private static Box libSummaryVBox;
	private Box outputFolderSummaryVBox;
	private JButton browseOutputButton;
	private static Drawer drawer;
	private static ArrayList<String> libs;
	private static TranslationsManager tm;
	private static JLabel huntStrategy;

	public SummaryDrawer(Drawer d,Hunter hunterInstance){
		drawer = d;
		h = hunterInstance;
		SummaryDrawer.tm = TranslationsManager.getInstance();
		libs = new ArrayList<String>();

		// Start cancel buttons
		startButton = new JButton(tm.getText("summaryStartButton"));
		startButton.addActionListener(this);
		abortButton = new JButton(tm.getText("summaryAbortButton"));
		abortButton.setEnabled(false);
		abortButton.addActionListener(this);

		// Instancia elementos do Panel
		searchSeqJLabel = new JLabel("");
		libSummaryVBox = Box.createVerticalBox();

		// Instancia output options
		outputFolderSummaryVBox = Box.createHorizontalBox();

		browseOutputButton = new JButton(tm.getText("summaryBrowseButton"));
		browseOutputButton.addActionListener(this);
		browseOutputButton.setActionCommand("Browse");

		outputfolderTF = new JTextField(50);
		outputfolderTF.setEditable(false);

		outputfolderTF.setText(h.getOutput());
		outputFolderSummaryVBox.add(outputfolderTF);
		outputFolderSummaryVBox.add(browseOutputButton);

		huntStrategy = new JLabel();
		switch(h.getMode()){
		case Hunter.FORCE_CUDA_MODE:
			huntStrategy.setText(tm.getText("SummaryForceCudaMode"));
			break;
		case Hunter.FORCE_NONCUDA_MODE:
			huntStrategy.setText(tm.getText("SummaryForceNonCudaMode"));
			break;
		case Hunter.NON_FORCE_MODE:
			huntStrategy.setText(tm.getText("SummaryNonForceMode"));
			break;
		}

		// Initial draw
		summaryContainer = new JPanel(new BorderLayout());
		drawSummaryContainer();
	}

	public JPanel getContainer(){
		return summaryContainer;
	}

	public static void setTargetSeq(String targetSeq){
		searchSeq = targetSeq;
		searchSeqJLabel.setText(targetSeq);
	}

	public static void addLoadedLib(String libpath){
		// Adiciona lib
		JLabel lib = new JLabel(libpath);
		lib.setAlignmentY(Component.TOP_ALIGNMENT);
		libSummaryVBox.add(lib);
		libs.add(libpath);
		Drawer.setProgressBar((new File(libpath)).length());
	}

	public static void removeLoadedLib(String libpath){
		// Remove lib
		for(int i=0;i < libSummaryVBox.getComponentCount();i++){
			JLabel lib =(JLabel)libSummaryVBox.getComponent(i);
			if(lib.getText().equals(libpath)){
				libSummaryVBox.remove(i);
			}
		}
		libs.remove(libpath);
	}

	private void drawSummaryContainer(){
		JPanel jp = new JPanel();
		jp.setLayout(new GridBagLayout());
		GridBagConstraints c = new GridBagConstraints();

		// Line - Target Sequence
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weighty = 0.15;
		c.weightx = 0.3;
		c.gridx = 0;
		c.gridy = 0;
		jp.add(new JLabel(tm.getText("targetSequenceLabel")),c);

		// Line - Target Sequence
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weighty = 0.1;
		c.weightx = 0.3;
		c.gridx = 1;
		c.gridy = 0;
		jp.add(searchSeqJLabel,c);

		// Line - Loaded libraries
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weightx = 0.3;
		c.gridx = 0;
		c.gridy = 1;
		jp.add(new JLabel(tm.getText("librariesLoadedLabel")),c);

		// Line - Loaded libraries
		c.fill = GridBagConstraints.BOTH;
		c.weighty = 0.65;
		c.weightx = 0.7;
		c.gridx = 1;
		c.gridy = 1;
		JScrollPane jscrp = new JScrollPane(libSummaryVBox);
		jp.add(jscrp,c);

		// Line - Output Folder
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weighty = 0.05;
		c.weightx = 0.3;
		c.gridx = 0;
		c.gridy = 2;
		jp.add(new JLabel(tm.getText("summaryOutputFolder")),c);

		// Line - Output Folder
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weighty = 0.05;
		c.weightx = 0.4;
		c.gridx = 1;
		c.gridy = 2;
		jp.add(outputFolderSummaryVBox,c);
		
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weighty = 0.05;
		c.weightx = 1;
		c.gridx = 1;
		c.gridy = 3;
		jp.add(huntStrategy,c);

		// Line - Start/Abort buttons
		Box hbox = Box.createHorizontalBox();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weighty = 0.1;
		c.weightx = 0.3;
		c.gridx = 1;
		c.gridy = 4;		
		hbox.add(startButton);
		hbox.add(abortButton);
		jp.add(hbox,c);
		summaryContainer.add(jp,BorderLayout.CENTER);
		return;
	}

	static public void huntDone(String libDatabase,File logFile){
		if(libDatabase != null){
			abortButton.setEnabled(false);
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				Drawer.writeToLog(e.getMessage());
			}
			Drawer.getReportDrawer().addMainReport(libDatabase,logFile);
			Drawer.getReportDrawer().updateReportsView();
			Drawer.moveToReportTab();
			Drawer.writeToLog(tm.getText("statusHuntDoneMsg"));
			Drawer.writeToLog(tm.getText("statusCheckReportTabMsg"));
		}else{
			Drawer.writeToLog(tm.getText("statusErrorProcessingMsg"));
		}

		startButton.setEnabled(true);
		abortButton.setEnabled(false);
		Drawer.enableStatusJLabels(false);
	}

	static public void huntAbort(){
		h.stop();
		startButton.setEnabled(true);
		abortButton.setEnabled(false);
		Drawer.enableStatusJLabels(false);
	}

	@Override
	public void actionPerformed(ActionEvent ae) {
		switch(ae.getActionCommand()){
		case "Start":
			// Monta reportContainer
			Drawer.writeToLog(tm.getText("statusStartHuntMsg"));
			startButton.setEnabled(false);
			abortButton.setEnabled(true);
			Drawer.setProgressBar(drawer.getTotalLibSize(libs));
			h.Set(searchSeq,libs);
			h.start();				
			break;
		case "Abort":
			huntAbort();
			break;
		case "Browse":
			JFileChooser jfcBrowse = new JFileChooser();
			jfcBrowse.setCurrentDirectory(new File(h.getOutput()));
			jfcBrowse.setDialogTitle("Select output folder");
			jfcBrowse.setMultiSelectionEnabled(false);
			jfcBrowse.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
			jfcBrowse.setAcceptAllFileFilterUsed(false);
			if(jfcBrowse.showOpenDialog(null) == JFileChooser.APPROVE_OPTION){
				h.setOutput(jfcBrowse.getSelectedFile().getAbsolutePath());
				outputfolderTF.setText(h.getOutput());
				Drawer.writeToLog(tm.getText("OutputFolderSet")+h.getOutput());
			}
			break;
		}
	}

	@Override
	public void update(Observable o, Object arg) {
		switch(h.getMode()){
		case Hunter.FORCE_CUDA_MODE:
			huntStrategy.setText(tm.getText("SummaryForceCudaMode"));
			break;
		case Hunter.FORCE_NONCUDA_MODE:
			huntStrategy.setText(tm.getText("SummaryForceNonCudaMode"));
			break;
		case Hunter.NON_FORCE_MODE:
			huntStrategy.setText(tm.getText("SummaryNonForceMode"));
			break;
		}

	}
}
