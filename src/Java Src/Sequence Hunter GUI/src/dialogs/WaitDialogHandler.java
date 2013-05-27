package dialogs;

import gui.ReportDrawer;

import javax.swing.JFrame;

public class WaitDialogHandler extends Thread {
	private JFrame parent;
	private ReportDrawer owner;
	private WaitDialog wd;
	
	public WaitDialogHandler(JFrame parent,ReportDrawer owner){
		this.parent = parent;
		this.owner = owner;
	}
	
	public void run(){
		wd = new WaitDialog(parent,owner);
		wd.setVisible(true);
	}
	
	public void dispose(){
		if(wd != null){
			wd.dispose();
		}
	}
}
