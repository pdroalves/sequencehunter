package dialogs;

import java.util.Observable;


import javax.swing.JFrame;

public class WaitDialogHandler extends Thread{
	private JFrame parent;
	private Observable owner;
	private WaitDialog wd;
	private Observador observador;
	
	public WaitDialogHandler(JFrame parent,Observable owner){
		this.parent = parent;
		this.owner = owner;
	}
	public WaitDialogHandler(JFrame parent){
		this.parent = parent;
		this.owner = null;
		observador = new Observador();
	}
	
	public void run(){
		if(owner != null)
			wd = new WaitDialog(parent,owner);
		else
			wd = new WaitDialog(parent,observador);
		wd.setVisible(true);
	}
	
	public void setDone(boolean b){
		if(observador != null)
			observador.setDone(b);
	}
	
	public void dispose(){
		if(wd != null){
			wd.dispose();
		}
	}
}
