package com.lnbio.dialogs;

import java.util.Observable;

public class Observador extends Observable {
	public void setDone(boolean b){
		if(b){
			this.setChanged();
			this.notifyObservers();
		}
	}
}
