package Dialogs;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.Box;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;

public class AboutDialog extends JDialog {
	  public AboutDialog(JFrame parent) {
		    super(parent, "About", true);
		    
		    Box b = Box.createVerticalBox();
		    b.add(Box.createGlue());
		    b.add(new JLabel("Sequence Hunter - "));
		    b.add(new JLabel("Developers:"));
		    b.add(new JLabel("Pedro Alves e Marcio Chain Bjalman"));
		    b.add(new JLabel("Laborat�rio Nacional de Bioci�ncias, Campinas - SP, Brazil"));
		    b.add(Box.createGlue());
		    getContentPane().add(b, "Center");


		    JPanel p2 = new JPanel();
		    JButton ok = new JButton("Ok");
		    p2.add(ok);
		    getContentPane().add(p2, "South");

		    ok.addActionListener(new ActionListener() {
		      public void actionPerformed(ActionEvent evt) {
		        setVisible(false);
		      }
		    });

		    setSize(450, 250);
			setLocationByPlatform(true);
			setLocationRelativeTo(parent);
		  }

}
