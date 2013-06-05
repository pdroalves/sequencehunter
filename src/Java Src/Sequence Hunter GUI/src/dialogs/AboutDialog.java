package dialogs;

import hunt.Hunter;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

import javax.imageio.ImageIO;
import javax.swing.Box;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextArea;
import javax.swing.border.EmptyBorder;

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
		    c.weighty = 0.2;
		    c.weightx = 1;
		    c.gridx = 0;
		    c.gridy = 0;
		    c.gridwidth = 2;
		    Box logo = Box.createVerticalBox();
		    try {
				BufferedImage logotipo = ImageIO.read(new File("resources/logotipo.png"));
				JLabel image = new JLabel(new ImageIcon(logotipo));
				logo.add(image);
			} catch (IOException e) {
				System.err.println("Couldn't load logo");
			}
		    logo.add(new JLabel(Hunter.getAppName()));
			jp.add(logo,c);

		    c.fill = GridBagConstraints.VERTICAL;
		    c.weighty = 0.6;
		    c.weightx = 0.5;
		    c.gridx = 0;
		    c.gridy = 1;
		    c.anchor = GridBagConstraints.WEST;
			jp.add(new JLabel("<html><body>"+tm.getText("aboutDevLabel") + ":<br>    "+tm.getText("aboutDevs")+"</body></html>"),c);
			
		    c.fill = GridBagConstraints.VERTICAL;
		    c.weighty = 0.6;
		    c.weightx = 0.5;
		    c.gridx = 1;
		    c.gridy = 1;
		    c.anchor = GridBagConstraints.EAST;
		    JTextArea jta = new JTextArea();
		    jta.setEditable(false);
		    jta.setText(tm.getText("UseTerms"));
			jp.add(jta,c);
			
			String patrocinio = tm.getText("aboutCompanyLabel");
		    c.fill = GridBagConstraints.HORIZONTAL;
		    c.weighty = 0.2;
		    c.weightx = 1;
		    c.gridx = 0;
		    c.gridy = 2;
		    c.gridwidth = 2;
		    jp.add(new JLabel(patrocinio),c);
			
		    /*Box b = Box.createVerticalBox();
		    b.add(Box.createGlue());
		    b.add(new JLabel(tm.getText("appName")+" - "));
		    b.add(new JLabel(tm.getText("aboutDevLabel")));
		    b.add(new JLabel(tm.getText("aboutDevs")));
		    b.add(new JLabel(tm.getText("aboutCompanyLabel")));
		    b.add(Box.createGlue());*/
		    jp.setBorder(new EmptyBorder(10, 10, 10, 10));
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
