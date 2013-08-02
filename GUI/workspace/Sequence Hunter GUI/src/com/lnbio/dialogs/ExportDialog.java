package com.lnbio.dialogs;

import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import javax.swing.Box;
import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.JSpinner;
import javax.swing.JTree;
import javax.swing.SpinnerModel;
import javax.swing.SpinnerNumberModel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import com.lnbio.auxiliares.checkbox.CheckBoxNode;
import com.lnbio.auxiliares.checkbox.CheckBoxNodeEditor;
import com.lnbio.auxiliares.checkbox.CheckBoxNodeRenderer;
import com.lnbio.auxiliares.checkbox.NamedVector;
import com.lnbio.gui.Drawer;
import com.lnbio.gui.ReportDrawer;
import com.lnbio.xml.TranslationsManager;



public class ExportDialog extends JDialog implements ActionListener, ChangeListener{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private JTree tree;
	private ArrayList<CheckBoxNode> cbnList;
	private long maxSeqsToExport = 200;
	private JProgressBar jpb;

	public ExportDialog(JFrame parent){
		super(parent, "Export", true);
		cbnList = new ArrayList<CheckBoxNode>();

		JPanel jp = new JPanel(new GridBagLayout());
		JPanel item = new JPanel(new FlowLayout());
		GridBagConstraints c = new GridBagConstraints();
		TranslationsManager tm = TranslationsManager.getInstance();

		///////////////////
		// Line - Export type
		Box horizontal = Box.createHorizontalBox();
		///////////////////
		// Export single
		JRadioButton selectExportTypeSingle = new JRadioButton();
		selectExportTypeSingle.setActionCommand("SingleExport");
		selectExportTypeSingle.setSelected(true);
		JLabel singleExportJLabel = new JLabel(tm.getText("menuFileItemExportGenericSingleReport"));
		item.add(selectExportTypeSingle);
		item.add(singleExportJLabel);
		horizontal.add(item);

		///////////////////
		// Export all
		item = new JPanel(new FlowLayout());
		JRadioButton selectExportTypeAll = new JRadioButton();
		selectExportTypeAll.setActionCommand("FullExport");
		selectExportTypeAll.setSelected(false);
		JLabel fullExportJLabel = new JLabel(tm.getText("menuFileItemExportGenericAllReports"));
		item.add(selectExportTypeAll);
		item.add(fullExportJLabel);
		horizontal.add(item);
		
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weighty = 0.10;
		c.weightx = 1;
		c.gridy = 0;
		c.gridx = 0;
		c.gridwidth = 2;
		jp.add(horizontal,c);

		ButtonGroup bgroup = new ButtonGroup();
		bgroup.add(selectExportTypeSingle);
		bgroup.add(selectExportTypeAll);

		selectExportTypeSingle.addActionListener(this);
		selectExportTypeAll.addActionListener(this);
		///////////////////

		///////////////////
		// Cria arvore
		tree = createTree();
		JScrollPane jscp = new JScrollPane(tree);


		// Adiciona arvore
		c.fill = GridBagConstraints.BOTH;
		c.weighty = 0.30;
		c.weightx = 1;
		c.gridy = 1;
		c.gridx = 0;
		c.gridwidth = 2;
		jp.add(jscp,c);

		///////////////////
		// Botoes
		SpinnerModel spinnermodel = new SpinnerNumberModel(maxSeqsToExport, 1, 1e20, 100);
		JSpinner spinner = new JSpinner(spinnermodel); 
		Dimension d = spinner.getPreferredSize();  
		d.width = 50;  
		spinner.setPreferredSize(d);  
		spinner.addChangeListener(this);
		c.fill = GridBagConstraints.NONE;
		c.weighty = 0.05;
		c.weightx = 0.5;
		c.gridy = 2;
		c.gridx = 0;
		c.gridwidth = 1;
		JLabel maxSeqs = new JLabel(TranslationsManager.getInstance().getText("MaxSeqs"));
		Box b = Box.createHorizontalBox();
		b.add(maxSeqs);
		b.add(spinner);
		jp.add(b,c);

		JButton exportButton = new JButton(tm.getText("ExportDialogExportButton"));
		exportButton.setActionCommand("Export");
		exportButton.addActionListener(this);
		JButton cancelButton = new JButton(tm.getText("ExportDialogCancelButton"));
		cancelButton.setActionCommand("Cancel");
		cancelButton.addActionListener(this);
		c.fill = GridBagConstraints.NONE;
		c.weighty = 0.05;
		c.weightx = 0.5;
		c.gridy = 3;
		c.gridx = 0;
		c.gridwidth = 2;
		Box hbox = Box.createHorizontalBox();
		hbox.add(exportButton);
		hbox.add(cancelButton);
		jp.add(hbox,c);
		

		///////////////////
		// JProgressBar
		jpb = new JProgressBar();
		jpb.setVisible(false);
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weighty = 0.05;
		c.weightx = 1;
		c.gridy = 4;
		c.gridx = 0;
		c.gridwidth = 2;
		jp.add(jpb,c);
		
		this.getContentPane().add(jp);
		setSize(450, 250);
		setLocationByPlatform(true);
		setLocationRelativeTo(parent);
	}

	@SuppressWarnings("rawtypes")
	private JTree createTree() {	    
		List<String> reportNames = ReportDrawer.getAllReportTitles();
		List<List<String>> tabNames = ReportDrawer.getAllTabNames();

		Object rootNodes[] = new Object[reportNames.size()];

		for(int i=0; i < reportNames.size();i++){
			CheckBoxNode mainReport[] = new CheckBoxNode[tabNames.get(i).size()];
			for(int j=0;j < tabNames.get(i).size();j++){
				Object obj = ReportDrawer.getReport(i, j);
				String name = ReportDrawer.getReportTitle(i);

				Pattern pattern = Pattern.compile("^.*"+System.getProperty("file.separator")+"(.*).db$");
				Matcher matcher = pattern.matcher(name);
				if(matcher.find()){
					name = matcher.group(1);
				}
				CheckBoxNode subReport = new CheckBoxNode(name+" - "+tabNames.get(i).get(j),obj,false);
				cbnList.add(subReport);
				mainReport[j] = subReport;
			}
			Vector mainReportVector = new NamedVector(reportNames.get(i),mainReport);
			rootNodes[i] = mainReportVector;
		}
		Vector rootVector = new NamedVector("Root", rootNodes);

		JTree tree = new JTree(rootVector);

		CheckBoxNodeRenderer renderer = new CheckBoxNodeRenderer();
		tree.setCellRenderer(renderer);

		tree.setCellEditor(new CheckBoxNodeEditor(tree,cbnList));
		tree.setEditable(true);

		return tree;
	}

	@Override
	public void actionPerformed(ActionEvent ae) {
		if(ae.getActionCommand().equals("SingleExport")){
			tree.setEnabled(true);
		}else if(ae.getActionCommand().equals("FullExport")){
			tree.setEnabled(false);			
		}else if(ae.getActionCommand().equals("Export")){
			// Escolhe o local para salvar o arquivo
			this.setEnabled(false);
			ExportWorker ew = new ExportWorker(this,jpb, cbnList, tree, maxSeqsToExport);
			ew.start();
		}else if(ae.getActionCommand().equals("Cancel")){
			super.dispose();
		}
	}
	
	protected void setExported(boolean b){
		if(b){
			this.setEnabled(true);
			Drawer.writeToLog(TranslationsManager.getInstance().getText("Exported"));
			super.dispose();
		}
	}

	@Override
	public void stateChanged(ChangeEvent e) {

		Object obj = e.getSource();
		if(obj instanceof JSpinner){
			JSpinner spinner = (JSpinner)obj;
			maxSeqsToExport = Math.round((Double) spinner.getValue());
			System.err.println("Maximo: "+maxSeqsToExport);
		}
	}
}
