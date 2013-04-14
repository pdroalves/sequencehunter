package dialogs;

import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.util.List;

import javax.swing.ButtonGroup;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;

import xml.TranslationsManager;

import gui.ReportDrawer;

public class ExportDialog extends JDialog{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public ExportDialog(JFrame parent){
	    super(parent, "Export", true);
	
	    JPanel jp = new JPanel(new GridBagLayout());
	    JPanel item = new JPanel(new FlowLayout());
	    GridBagConstraints c = new GridBagConstraints();
	    TranslationsManager tm = TranslationsManager.getInstance();

	    ///////////////////
	    ///////////////////
		// Line - Export type
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.10;
	    c.weightx = 0.5;
	    c.gridy = 0;
	    
	    // Export single
	    c.gridx = 0;
	    JRadioButton selectExportTypeSingle = new JRadioButton();
	    selectExportTypeSingle.setSelected(true);
	    JLabel singleExportJLabel = new JLabel(tm.getText("menuFileItemExportGenericSingleReport"));
	    item.add(selectExportTypeSingle);
	    item.add(singleExportJLabel);
		jp.add(item,c);
		
		// Export all
		c.gridx = 1;
		item = new JPanel(new FlowLayout());
		JRadioButton selectExportTypeAll = new JRadioButton();
	    selectExportTypeAll.setSelected(false);
	    JLabel fullExportJLabel = new JLabel(tm.getText("menuFileItemExportGenericAllReports"));
	    item.add(selectExportTypeAll);
	    item.add(fullExportJLabel);
	    jp.add(item);
	    

	    ButtonGroup bgroup = new ButtonGroup();
	    bgroup.add(selectExportTypeSingle);
	    bgroup.add(selectExportTypeAll);
	    ///////////////////
	    ///////////////////
	    ///////////////////
	    
	    // Arvore de reports
        DefaultMutableTreeNode top = new DefaultMutableTreeNode("The Java Series");
        createNodes(top);
	    JTree tree = new JTree(top);
	    JScrollPane jscp = new JScrollPane(tree);
	    c.fill = GridBagConstraints.BOTH;
	    c.weighty = 0.30;
	    c.weightx = 1;
	    c.gridy = 1;
		c.gridx = 0;
		c.gridwidth = 2;
	    jp.add(jscp,c);
	    
	    getContentPane().add(jp);
	    
	    setSize(450, 250);
		setLocationByPlatform(true);
		setLocationRelativeTo(parent);
	}
	
	private void createNodes(DefaultMutableTreeNode top) {	    
	    List<String> reportNames = ReportDrawer.getAllReportTitles();
	    List<List<String>> tabNames = ReportDrawer.getAllTabNames();
	    
	    for(int i=0; i < reportNames.size();i++){
	    	DefaultMutableTreeNode mainReport = new DefaultMutableTreeNode(reportNames.get(i));
	    	for(int j=0;j < tabNames.get(i).size();j++){
	    		DefaultMutableTreeNode subReport = new DefaultMutableTreeNode(tabNames.get(i).get(j));
	    		mainReport.add(subReport);
	    	}
	    	top.add(mainReport);
	    }
	}
}
