package histogram;

import java.awt.Color;
import java.util.ArrayList;
import java.util.Iterator;

import javax.swing.BorderFactory;
import javax.swing.border.BevelBorder;

import myTypeData.GenType;


public class EventHistogram {
	private ReportHistogramPanel dp;
	@SuppressWarnings("rawtypes")
	private ArrayList data;
	private int MaxBars = 100;

	public EventHistogram() {
		dp = new ReportHistogramPanel();
		dp.setBackground(new Color(255, 255, 255));
		dp.setBorder(BorderFactory.createBevelBorder(BevelBorder.RAISED));
		data = new ArrayList<GenType>();
		dp.setMaxBarWidth(50);
		dp.setVisible(false);
	}

	public void setBarNames(boolean b){
		dp.setBarNames(b);
	}

	@SuppressWarnings("unchecked")
	public void addType(GenType e) {
		data.add(e);
		checkVisibleStatus();
	}     

	@SuppressWarnings("unchecked")
	public void addTypeSet(@SuppressWarnings("rawtypes") ArrayList al){
		Iterator<GenType> iterator = al.iterator();
		int added = 0;
		while(iterator.hasNext() && added < MaxBars){
			data.add(iterator.next());
		}
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

	@SuppressWarnings("unchecked")
	public void commit(){
		dp.setData(data);
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
		dp.clearAllBars();
	}
} 