package myTypeData;
import histogram.SimpleHistogramPanel;

import java.awt.Color;
import java.util.ArrayList;
import java.util.Iterator;

import javax.swing.BorderFactory;
import javax.swing.border.BevelBorder;


public class TypeHistogram {
    private SimpleHistogramPanel dp;
    @SuppressWarnings("rawtypes")
	private ArrayList data;

    public TypeHistogram() {
        dp = new SimpleHistogramPanel("My histogram");
        dp.setBackground(new Color(255, 255, 255));
        dp.setBorder(BorderFactory.createBevelBorder(BevelBorder.RAISED));
        this.setData(new ArrayList<Type>());
    }
    
    public void setBarNames(boolean b){
    	dp.setBarNames(b);
    }

    @SuppressWarnings("unchecked")
	public void addType(Type e) {
    	getData().add(e);
    	calcRelativeFreq();
    } 
    
    
    @SuppressWarnings("unchecked")
	public SimpleHistogramPanel draw(){
    	dp.setData(getData());
    	return dp;
    }
    
    public void enableLinearize(boolean b){
    	dp.enableLinearize(b);
    }
    
    @SuppressWarnings("unchecked")
	private void calcRelativeFreq(){
		Iterator<Type> iterator = getData().iterator();
		int total = 0;
		Type e;

		// Conta total de eventos
		while(iterator.hasNext()){
			e = iterator.next();
			total += e.getFreq();
		}

		iterator = getData().iterator();
		// Ajusta os eventos
		while(iterator.hasNext()){
			e = iterator.next();
			e.setRelativeFreq(e.getFreq()/(float)(total));
		}
	}

	@SuppressWarnings("rawtypes")
	public ArrayList getData() {
		return data;
	}

	@SuppressWarnings("rawtypes")
	public void setData(ArrayList data) {
		this.data = data;
	}
 
} 