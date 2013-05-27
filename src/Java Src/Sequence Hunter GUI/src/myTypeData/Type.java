package myTypeData;
import histogram.HistogramBar;

import java.awt.Color;

public class Type extends HistogramBar{
	private String seq;
	private int freq;
	private double relativeFreq;
	
	public Type(String s){
		super();
		seq = s;
		freq = 0;
		relativeFreq = 0;
	}
	
	public Type(String s,Color c){
		super();
		seq = s;
		freq = 0;
		relativeFreq = 0;
		super.setColor(c);
	}

	public Type(String s,int f){
		super();
		seq = s;
		freq = f;
		relativeFreq = 0;
	}

	public Type(String s,int f,Color c){
		super();
		seq = s;
		freq = f;
		relativeFreq = 0;
		super.setColor(c);
	}
	
	public String getSeq(){
		return seq;
	}
	
	public void setSeq(String s){
		seq = s;
	}
	
	public int getFreq(){
		return freq;
	}
	
	public void setFreq(int n){
		freq = n;
	}
	
	public double getRelativeFreq(){
		return relativeFreq;
	}

	public void setRelativeFreq(double r){
		relativeFreq = r;
	}
	
	public void setBar(int x,int y,int width,int height){
		super.setX(x);
		super.setY(y);
		super.setWidth(width);
		super.setHeight(height);
	}
	
	public void setColor(Color c){
		super.setColor(c);
	}
	
	public Color getColor(){
		return super.getColor();
	}
	
	// Metodos override de HistogramBar
	
	@Override
	public String getName(){
		return getSeq();
	}
	
	@Override
	public void setName(String n) {
		setSeq(n);
	}

	@Override
	public void setValue(int n) {
		setFreq(n);
	}

	@Override
	public int getValue() {
		return getFreq();
	}
	
}
