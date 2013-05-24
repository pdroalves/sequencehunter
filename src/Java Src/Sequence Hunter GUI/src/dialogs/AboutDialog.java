package dialogs;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.border.BevelBorder;

import xml.TranslationsManager;

public class AboutDialog extends JDialog {
	  /**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	
	public AboutDialog(JFrame parent) {
		
		    super(parent, "About", true);
		    TranslationsManager tm = TranslationsManager.getInstance();
		    JPanel jp = new JPanel(new GridBagLayout());
		    GridBagConstraints c = new GridBagConstraints();

			// Line - Target Sequence
		    c.fill = GridBagConstraints.BOTH;
		    c.weighty = 0.5;
		    c.weightx = 0.3;
		    c.gridx = 0;
		    c.gridy = 0;
			jp.add(new JLabel(tm.getText("appName")),c);

			Box b = Box.createVerticalBox();
			b.add(new JLabel(tm.getText("appName")));
			b.add(new JLabel(tm.getText("aboutDevLabel") + ": "+tm.getText("aboutDevs")));
		    c.fill = GridBagConstraints.BOTH;
		    c.weighty = 0.5;
		    c.weightx = 0.7;
		    c.gridx = 1;
		    c.gridy = 0;
			jp.add(b,c);
			
			String patrocinio = tm.getText("aboutCompanyLabel");
		    c.fill = GridBagConstraints.BOTH;
		    c.weighty = 0.5;
		    c.weightx = 1;
		    c.gridx = 0;
		    c.gridy = 1;
		    jp.add(new JLabel(patrocinio),c);
			
		    /*Box b = Box.createVerticalBox();
		    b.add(Box.createGlue());
		    b.add(new JLabel(tm.getText("appName")+" - "));
		    b.add(new JLabel(tm.getText("aboutDevLabel")));
		    b.add(new JLabel(tm.getText("aboutDevs")));
		    b.add(new JLabel(tm.getText("aboutCompanyLabel")));
		    b.add(Box.createGlue());*/
		    getContentPane().add(jp, "Center");


		    JPanel p2 = new JPanel();
		    JButton ok = new JButton("Ok");
		    p2.add(ok);
		    getContentPane().add(p2, "South");

		    ok.addActionListener(new ActionListener() {
		      public void actionPerformed(ActionEvent evt) {
		        setVisible(false);
		      }
		    });

		    setSize(650, 250);
			setLocationByPlatform(true);
			setLocationRelativeTo(parent);
		  }

}
