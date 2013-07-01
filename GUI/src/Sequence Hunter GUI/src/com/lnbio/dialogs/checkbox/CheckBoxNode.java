package com.lnbio.dialogs.checkbox;

public class CheckBoxNode {

	private String text;
	private boolean selected;
	private Object attribute;
	private int maxSeqsToPrint = 1000;

	public CheckBoxNode(String text, boolean selected) {
		this.text = text;
		this.selected = selected;
		this.attribute = null;
	}
	public CheckBoxNode(String text, Object attribute,boolean selected) {
		this.text = text;
		this.selected = selected;
		this.attribute = attribute;
	}
	public CheckBoxNode(String text, int maxSeqsToPrint,boolean selected) {
		this.text = text;
		this.selected = selected;
		this.maxSeqsToPrint = maxSeqsToPrint;
	}
	public CheckBoxNode(String text, Object attribute,int maxSeqsToPrint,boolean selected) {
		this.text = text;
		this.selected = selected;
		this.attribute = attribute;
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

	public Object getAttribute(){
		return attribute;
	}

	public String toString() {
		return getClass().getName() + "[" + text + "/" + selected+"]";
	}
	
	public int getMaxSeqsToPrint(){
		return maxSeqsToPrint;
	}
}

