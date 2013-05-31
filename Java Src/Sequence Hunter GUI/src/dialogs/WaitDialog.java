package dialogs;


import java.awt.BorderLayout;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JProgressBar;

import xml.TranslationsManager;

@SuppressWarnings("serial")
public class WaitDialog extends JDialog implements Observer{
	
	private TranslationsManager tm = TranslationsManager.getInstance();
	
	public WaitDialog(JFrame parent,Observable owner){
		super(parent,"Please wait...",true);
		this.setUndecorated(true);
		owner.addObserver(this);
		JLabel label = new JLabel(tm.getText("PleaseWaitDialog"));
		this.getContentPane().setLayout(new BorderLayout());
		this.getContentPane().add(label,BorderLayout.CENTER);
		JProgressBar jprog = new JProgressBar();
		jprog.setIndeterminate(true);
		this.getContentPane().add(jprog,BorderLayout.SOUTH);
		setSize(100, 50);
		setLocationByPlatform(true);
		setLocationRelativeTo(parent);
	}

	@Override
	public void update(Observable o, Object arg) {
		this.dispose();		
	}

}
