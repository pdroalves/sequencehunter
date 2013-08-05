package com.lnbio.gui.modules;

import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.JPopupMenu;
import javax.swing.JTable;

import com.lnbio.auxiliares.FiveCutPopUpMenu;
import com.lnbio.database.DBManager;
import com.lnbio.gui.ReportDrawer;

public class CentralCutMenuMouseAdapter extends MouseAdapter{
	private JTable table;
	private ReportDrawer owner;
	private int tableIndex;
    private final int SEQUENCE_COLUMN_INDEX = 1;
    private DBManager dbm;
	
	public CentralCutMenuMouseAdapter(ReportDrawer owner,DBManager dbm,int tableIndex,JTable table){
		this.table = table;
		this.owner = owner;
		this.dbm = dbm;
		this.tableIndex = tableIndex;
	}

	@Override
	public void mouseReleased(MouseEvent e) {
		launchPopUp(e);
	}
	
	@Override
	public void mousePressed(MouseEvent e) {
		launchPopUp(e);
	}
	
	private void launchPopUp(MouseEvent e){
		int r = table.rowAtPoint(e.getPoint());
		if (r >= 0 && r < table.getRowCount()) {
			table.setRowSelectionInterval(r, r);
		} else {
			table.clearSelection();
		}

		int rowindex = table.getSelectedRow();
		System.out.println(rowindex);
		if (rowindex < 0)
			return;
		if (e.isPopupTrigger() && e.getComponent() instanceof JTable ) {
			JPopupMenu popup = new FiveCutPopUpMenu(owner,dbm,String.valueOf(table.getModel().getValueAt(rowindex, SEQUENCE_COLUMN_INDEX)),tableIndex);
			popup.show(e.getComponent(), e.getX(), e.getY());
		}
	}
}

