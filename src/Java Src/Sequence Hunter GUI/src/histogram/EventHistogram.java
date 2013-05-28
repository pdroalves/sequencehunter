package histogram;

import hunt.Evento;

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
	private ReportHistogramPanel panel;
	@SuppressWarnings("rawtypes")
	private ArrayList data;
	private int MaxBars = 100;
	private DBManager dbm;
	@SuppressWarnings("rawtypes")
	public EventHistogram(DBManager dbm) {
		panel = new ReportHistogramPanel();
		panel.setBackground(new Color(255, 255, 255));
		panel.setBorder(BorderFactory.createBevelBorder(BevelBorder.RAISED));
		data = new ArrayList<GenType>();
		panel.setMaxBarWidth(50);
		panel.setVisible(false);
		this.enableLinearize(true);
		data = new ArrayList();
		this.dbm = dbm;
	}

	public void setBarNames(boolean b){
		panel.setBarNames(b);
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
				panel.setVisible(true);
			}else{
				panel.setVisible(false);
			}
		}else{
			panel.setVisible(false);
		}
	}

	public ReportHistogramPanel getPanel(){
		return panel;
	}

	@SuppressWarnings("unchecked")
	public void commit(){
		int[] mode = dbm.getMode();
		int max = -1;
		switch(mode[0]){
		case Evento.VALUE_PARES_ABS:
			break;
		case Evento.VALUE_PARES_REL:
			max = dbm.getTotalPares();
			break;
		case 2:
			break;
		}
		if(max > 0){
			dbm.normalizeData(max,mode[0]);
		}
		panel.setData(data);
	}

	public void enableLinearize(boolean b){
		panel.enableLinearize(b);
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
				panel.drawMark(g);
			}else{
				panel.dontDrawMark(g);
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
		panel.repaint();
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