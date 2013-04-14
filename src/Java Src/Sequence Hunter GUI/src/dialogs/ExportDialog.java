package dialogs;

import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import javax.swing.ButtonGroup;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JRadioButton;

import xml.TranslationsManager;

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
		
		// Line - Export type
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.10;
	    c.weightx = 0.5;
	    c.gridx = 0;
	    c.gridy = 0;
	    JRadioButton selectExportTypeSingle = new JRadioButton();
	    selectExportTypeSingle.setSelected(true);
	    JLabel singleExportJLabel = new JLabel(tm.getText("menuFileItemExportGenericSingleReport"));
	    item.add(selectExportTypeSingle);
	    item.add(singleExportJLabel);
		jp.add(item,c);
		
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
	    
	    getContentPane().add(jp);
	    
	    setSize(450, 250);
		setLocationByPlatform(true);
		setLocationRelativeTo(parent);
	}
}
