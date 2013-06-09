package com.lnbio.tables.report;

import java.util.List;

import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableModel;
import javax.swing.table.TableRowSorter;

import com.lnbio.database.DBManager;


public class TableSorter<M extends AbstractTableModel> extends TableRowSorter<M> {
	private DBManager dbm;
	private int column;
	private int ordem;
	
	public TableSorter(AbstractTableModel model,DBManager dbm){
		super((M)model);
		this.dbm = dbm;
		column = 2;
		ordem = DBManager.DESC;
	}

	@Override
	public void toggleSortOrder(int column) {
		if(this.column != column){
			ordem = DBManager.DESC;
		}else{
			ordem = (ordem+1) % 2;
		}
		this.column = column;
		dbm.sort(column,ordem);
	}

}
