package com.lnbio.dialogs;


import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.net.URL;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.border.EmptyBorder;

import com.lnbio.hunt.Hunter;
import com.lnbio.xml.TranslationsManager;


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
		c.anchor = GridBagConstraints.CENTER;
		JPanel logoPanel = new JPanel(new BorderLayout());
		try {
			URL marca = getClass().getResource("/com/lnbio/resources/logotipoSH.png");
			BufferedImage logotipo = ImageIO.read(marca);

			JLabel image = new JLabel(new ImageIcon(logotipo.getScaledInstance(150,-1, BufferedImage.SCALE_SMOOTH)));
			logoPanel.add(image,BorderLayout.CENTER);
		} catch (IOException e) {
			System.err.println("Couldn't load logo");
		}
		logoPanel.setAlignmentX(CENTER_ALIGNMENT);
		logoPanel.add(new JLabel(Hunter.getAppName()),BorderLayout.PAGE_END);
		jp.add(logoPanel,c);

		c.fill = GridBagConstraints.BOTH;
		c.weighty = 0.6;
		c.weightx = 0.5;
		c.gridx = 0;
		c.gridy = 1;
		c.gridwidth=1;
		c.anchor = GridBagConstraints.WEST;
		jp.add(new JLabel("<html><body>"+tm.getText("aboutDevLabel") + ":<br>    "+tm.getText("aboutDevs")+"</body></html>"),c);

		c.fill = GridBagConstraints.BOTH;
		c.weighty = 0.6;
		c.weightx = 0.5;
		c.gridx = 1;
		c.gridy = 1;
		c.anchor = GridBagConstraints.EAST;
		JTextArea jta = new JTextArea();
		jta.setLineWrap(true);
		jta.setWrapStyleWord(true); 
		jta.setEditable(false);
		jta.setText(tm.getText("UseTerms"));
		JScrollPane jscrlp = new JScrollPane(jta);
		jp.add(jscrlp,c);

		c.fill = GridBagConstraints.BOTH;
		c.weighty = 0.2;
		c.weightx = 1;
		c.gridx = 0;
		c.gridy = 3;
		c.gridwidth = 2;
		c.anchor = GridBagConstraints.CENTER;
		JPanel companyLogoPanel = new JPanel(new FlowLayout(FlowLayout.CENTER));
		try {			
			URL marcacnpem = getClass().getResource("/com/lnbio/resources/cnpem.jpg");
			URL marcalnbio = getClass().getResource("/com/lnbio/resources/lnbio.jpg");

			BufferedImage logocnpem = ImageIO.read(marcacnpem);
			BufferedImage logolnbio = ImageIO.read(marcalnbio);

			JLabel imagecnpem = new JLabel(new ImageIcon(logocnpem.getScaledInstance(60,-1, BufferedImage.SCALE_SMOOTH)));			
			JLabel imagelnbio = new JLabel(new ImageIcon(logolnbio.getScaledInstance(60,-1, BufferedImage.SCALE_SMOOTH)));


			companyLogoPanel.add(imagecnpem);
			companyLogoPanel.add(imagelnbio);
		} catch (IOException e) {
			System.err.println("Couldn't load logo");
		}
		jp.add(companyLogoPanel,c);

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

		this.setSize(650, 500);
		this.setLocationByPlatform(true);
		this.setLocationRelativeTo(parent);
	}

}
