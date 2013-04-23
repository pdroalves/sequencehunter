package dialogs;

import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.zip.*;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;

import javax.swing.ButtonGroup;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.JTree;

import database.DBManager;
import dialogs.checkbox.CheckBoxNode;
import dialogs.checkbox.CheckBoxNodeEditor;
import dialogs.checkbox.CheckBoxNodeRenderer;
import dialogs.checkbox.NamedVector;

import xml.TranslationsManager;

import gui.ReportDrawer;
import hunt.Evento;

public class ExportDialog extends JDialog implements ActionListener{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private JTree tree;
	private ArrayList<CheckBoxNode> cbnList;

	public ExportDialog(JFrame parent){
		super(parent, "Export", true);
		cbnList = new ArrayList<CheckBoxNode>();

		JPanel jp = new JPanel(new GridBagLayout());
		JPanel item = new JPanel(new FlowLayout());
		GridBagConstraints c = new GridBagConstraints();
		TranslationsManager tm = TranslationsManager.getInstance();

		///////////////////
		// Line - Export type
		c.fill = GridBagConstraints.HORIZONTAL;
		c.weighty = 0.10;
		c.weightx = 0.5;
		c.gridy = 0;

		///////////////////
		// Export single
		c.gridx = 0;
		JRadioButton selectExportTypeSingle = new JRadioButton();
		selectExportTypeSingle.setActionCommand("SingleExport");
		selectExportTypeSingle.setSelected(true);
		JLabel singleExportJLabel = new JLabel(tm.getText("menuFileItemExportGenericSingleReport"));
		item.add(selectExportTypeSingle);
		item.add(singleExportJLabel);
		jp.add(item,c);

		///////////////////
		// Export all
		c.gridx = 1;
		item = new JPanel(new FlowLayout());
		JRadioButton selectExportTypeAll = new JRadioButton();
		selectExportTypeAll.setActionCommand("FullExport");
		selectExportTypeAll.setSelected(false);
		JLabel fullExportJLabel = new JLabel(tm.getText("menuFileItemExportGenericAllReports"));
		item.add(selectExportTypeAll);
		item.add(fullExportJLabel);
		jp.add(item);

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
		JButton exportButton = new JButton(tm.getText("ExportDialogExportButton"));
		exportButton.setActionCommand("Export");
		exportButton.addActionListener(this);
		JButton cancelButton = new JButton(tm.getText("ExportDialogCancelButton"));
		cancelButton.setActionCommand("Cancel");
		cancelButton.addActionListener(this);
		c.fill = GridBagConstraints.NONE;
		c.weighty = 0.05;
		c.weightx = 1;
		c.gridy = 2;
		c.gridx = 0;
		c.gridwidth = 1;
		jp.add(exportButton,c);
		c.gridx=1;
		jp.add(cancelButton,c);

		getContentPane().add(jp);

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
				name = name.substring(0, name.length()-3);
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
			JFileChooser jfc = new JFileChooser(System.getProperty("user.dir"));
			jfc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
			if(jfc.showSaveDialog(null) == JFileChooser.APPROVE_OPTION){
				Iterator<CheckBoxNode> iterator = cbnList.iterator();
				try {
					Calendar calendar = Calendar.getInstance();
					ZipOutputStream out = new ZipOutputStream(new FileOutputStream(new File(jfc.getSelectedFile()+"/"+"Export-"+calendar.getTime()+".zip")));
					while(iterator.hasNext()){
						CheckBoxNode cbn = iterator.next();
						if(cbn.isSelected() || !tree.isEnabled()){
							Object attribute = cbn.getAttribute();

							if(attribute instanceof DBManager){
								DBManager dbm = (DBManager) attribute;
								List<Evento> eventos = dbm.getEvents();

								System.out.println("Vou salvar : "+eventos.size()+" em "+jfc.getSelectedFile());

								// name the file inside the zip  file 
								out.putNextEntry(new ZipEntry(cbn.getText()+".txt")); 
								Iterator<Evento> eventoIterator = eventos.iterator();
								while(eventoIterator.hasNext()){
									Evento e = eventoIterator.next();
									String str;
									if(cbn.getText().contains("unpaired")){
										str = e.getSeq()+"-"+e.getPares()+"-"+e.getSensos()+"-"+e.getAntisensos()+"\n"; 
									}else{
										str = e.getSeq()+"-"+e.getPares()+"\n"; 
									}
									out.write(str.getBytes("UTF-8"), 0, str.length());
								}

							}else if(attribute instanceof File){
								File f = (File) attribute;
								FileInputStream in = new FileInputStream(f);
								// out put file 

								out.putNextEntry(new ZipEntry(f.getName())); 

								byte[] b = new byte[1024];

								int count;

								while ((count = in.read(b)) > 0) {
									System.out.println();

									out.write(b, 0, count);
								}
								in.close();
							}
						}
					}
					out.close();
				}catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
			}
			super.dispose();
		}else if(ae.getActionCommand().equals("Cancel")){
			super.dispose();
		}
	}
}
