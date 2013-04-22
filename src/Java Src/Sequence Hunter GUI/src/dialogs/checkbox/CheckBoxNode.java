package dialogs.checkbox;

import database.DBManager;


public class CheckBoxNode {

	private String text;
	private boolean selected;
	private DBManager dbm;
	private int maxSeqsToPrint = 1000;

	public CheckBoxNode(String text, boolean selected) {
		this.text = text;
		this.selected = selected;
		this.dbm = null;
	}
	public CheckBoxNode(String text, DBManager dbm,boolean selected) {
		this.text = text;
		this.selected = selected;
		this.dbm = dbm;
	}
	public CheckBoxNode(String text, int maxSeqsToPrint,boolean selected) {
		this.text = text;
		this.selected = selected;
		this.maxSeqsToPrint = maxSeqsToPrint;
	}
	public CheckBoxNode(String text, DBManager dbm,int maxSeqsToPrint,boolean selected) {
		this.text = text;
		this.selected = selected;
		this.dbm = dbm;
		this.maxSeqsToPrint = maxSeqsToPrint;
	}

	public boolean isSelected() {
		return selected;
	}

	public void setSelected(boolean newValue) {
		selected = newValue;
	}

	public String getText() {
		return text;
	}

	public void setText(String newValue) {
		text = newValue;
	}

	public DBManager getDBManager(){
		return dbm;
	}

	public String toString() {
		return getClass().getName() + "[" + text + "/" + selected+"]";
	}
	
	public int getMaxSeqsToPrint(){
		return maxSeqsToPrint;
	}
}

