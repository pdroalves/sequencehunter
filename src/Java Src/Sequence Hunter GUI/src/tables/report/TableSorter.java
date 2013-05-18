package tables.report;

import java.util.List;

import javax.swing.table.AbstractTableModel;
import javax.swing.table.TableModel;
import javax.swing.table.TableRowSorter;

import database.DBManager;

public class TableSorter<M extends AbstractTableModel> extends TableRowSorter<M> {
	private DBManager dbm;
	private int ordem;
	public TableSorter(AbstractTableModel model,DBManager dbm){
		super((M)model);
		this.dbm = dbm;
		ordem = 0;
	}

	@Override
	public void toggleSortOrder(int column) {
		ordem = (ordem+1) % 2;
		dbm.sort(column,ordem);
	}

}
