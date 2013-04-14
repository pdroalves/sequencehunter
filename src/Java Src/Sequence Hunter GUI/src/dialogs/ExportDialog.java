package dialogs;

import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.List;
import java.util.Vector;

import javax.swing.ButtonGroup;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JScrollPane;
import javax.swing.JTree;

import dialogs.checkbox.CheckBoxNode;
import dialogs.checkbox.CheckBoxNodeEditor;
import dialogs.checkbox.CheckBoxNodeRenderer;
import dialogs.checkbox.NamedVector;

import xml.TranslationsManager;

import gui.ReportDrawer;

public class ExportDialog extends JDialog implements ActionListener{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private JTree tree;
	
	public ExportDialog(JFrame parent){
	    super(parent, "Export", true);
	
	    JPanel jp = new JPanel(new GridBagLayout());
	    JPanel item = new JPanel(new FlowLayout());
	    GridBagConstraints c = new GridBagConstraints();
	    TranslationsManager tm = TranslationsManager.getInstance();

	    ///////////////////
	    // Cria arvore de reports
        tree = createTree();
	    JScrollPane jscp = new JScrollPane(tree);
	    
	    ///////////////////
		// Line - Export type
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.10;
	    c.weightx = 0.5;
	    c.gridy = 0;
	    
	    // Export single
	    c.gridx = 0;
	    JRadioButton selectExportTypeSingle = new JRadioButton();
	    selectExportTypeSingle.setActionCommand("SingleExport");
	    selectExportTypeSingle.setSelected(true);
	    JLabel singleExportJLabel = new JLabel(tm.getText("menuFileItemExportGenericSingleReport"));
	    item.add(selectExportTypeSingle);
	    item.add(singleExportJLabel);
		jp.add(item,c);
		
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

	    ///////////////////
	    // Adiciona arvore
	    c.fill = GridBagConstraints.BOTH;
	    c.weighty = 0.30;
	    c.weightx = 1;
	    c.gridy = 1;
		c.gridx = 0;
		c.gridwidth = 2;
	    jp.add(jscp,c);
	    ///////////////////
	    ///////////////////
	    
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
	    		CheckBoxNode subReport = new CheckBoxNode(tabNames.get(i).get(j),false);
	    		mainReport[j] = subReport;
	    	}
	    	Vector mainReportVector = new NamedVector(reportNames.get(i),mainReport);
	    	rootNodes[i] = mainReportVector;
	    }
	    Vector rootVector = new NamedVector("Root", rootNodes);

	    JTree tree = new JTree(rootVector);
	    
	    CheckBoxNodeRenderer renderer = new CheckBoxNodeRenderer();
	    tree.setCellRenderer(renderer);

	    tree.setCellEditor(new CheckBoxNodeEditor(tree));
	    tree.setEditable(true);
	    
	    return tree;
	}

	@Override
	public void actionPerformed(ActionEvent ae) {
		if(ae.getActionCommand().equals("SingleExport")){
			tree.setEnabled(true);
		}else if(ae.getActionCommand().equals("FullExport")){
			tree.setEnabled(false);			
		}
	}
}
