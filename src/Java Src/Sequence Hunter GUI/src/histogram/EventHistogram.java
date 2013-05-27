package histogram;

import java.awt.Color;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Observable;
import java.util.Observer;

import javax.swing.BorderFactory;
import javax.swing.border.BevelBorder;

import database.DBManager;

import myTypeData.GenType;


public class EventHistogram implements Observer{
	private ReportHistogramPanel dp;
	@SuppressWarnings("rawtypes")
	private ArrayList data;
	private int MaxBars = 100;

	public EventHistogram(DBManager dbm) {
		dp = new ReportHistogramPanel();
		dp.setBackground(new Color(255, 255, 255));
		dp.setBorder(BorderFactory.createBevelBorder(BevelBorder.RAISED));
		data = new ArrayList<GenType>();
		dp.setMaxBarWidth(50);
		dp.setVisible(false);
		this.enableLinearize(true);
		if(dbm.isReady()){
			this.addTypeSet(dbm.getEvents());
			this.commit();
		}
	}

	public void setBarNames(boolean b){
		dp.setBarNames(b);
	}

	@SuppressWarnings("unchecked")
	public void addType(GenType e) {
		if(data.size() < MaxBars)
			data.add(e);
		checkVisibleStatus();
	}     

	@SuppressWarnings("unchecked")
	public void addTypeSet(@SuppressWarnings("rawtypes") ArrayList al){
		data.addAll(al);
		for(int i = MaxBars;i < data.size();i++)
			data.remove(i);
		checkVisibleStatus();
	}

	private void checkVisibleStatus(){
		if(data != null){
			if(data.size() > 0){
				dp.setVisible(true);
			}else{
				dp.setVisible(false);
			}
		}else{
			dp.setVisible(false);
		}
	}

	public ReportHistogramPanel getPanel(){
		return dp;
	}

	public void commit(){
		this.addTypeSet(data);
	}

	public void enableLinearize(boolean b){
		dp.enableLinearize(b);
	} 

	@SuppressWarnings("rawtypes")
	public ArrayList getData() {
		return data;
	}
	
	public void enableBarHighlight(String tag,boolean b){
		// Ativa ou desativa highlight em barra vinculada ao GenType com essa tag
		@SuppressWarnings("unchecked")
		Iterator<GenType> iterator = data.iterator();
		while(iterator.hasNext()){
			GenType g = iterator.next();
			if(g.getTag() == tag){
				dp.drawMark(g);
			}else{
				dp.dontDrawMark(g);
			}
		}
	}

	public void clearAllBars(){
		if(data.size() > 0){
			data.clear();
			commit();
			System.err.println("Barras removidas");
		}
	}
	
	public void repaint(){
		dp.repaint();
	}

	@Override
	public void update(Observable o, Object arg) {
		System.out.println("EH updated!");
		this.clearAllBars();
		this.addTypeSet(((DBManager) arg).getEvents());
		this.commit();
		this.checkVisibleStatus();
	}
} 