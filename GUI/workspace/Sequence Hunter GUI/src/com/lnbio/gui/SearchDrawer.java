package com.lnbio.gui;


import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.List;

import javax.swing.BorderFactory;
import javax.swing.Box;
import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFileChooser;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JSpinner;
import javax.swing.JSplitPane;
import javax.swing.JTabbedPane;
import javax.swing.JTable;
import javax.swing.ListSelectionModel;
import javax.swing.SpinnerModel;
import javax.swing.SpinnerNumberModel;
import javax.swing.border.Border;
import javax.swing.border.EmptyBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.table.DefaultTableCellRenderer;

import com.lnbio.auxiliares.JBaseTextField;
import com.lnbio.auxiliares.JTxtFileFilter;
import com.lnbio.hunt.Library;
import com.lnbio.tables.JLibPreviewTableModel;
import com.lnbio.tables.JTableRenderer;
import com.lnbio.xml.TranslationsManager;



public class SearchDrawer implements ActionListener{
	private String searchSeq;
	private JBaseTextField seqOriginal;
	private static JLabel seqBusca;
	private JButton setSeqButton;
	private JList<String> jl ;
	private DefaultListModel<String> listModel;
	private JTabbedPane libContainer;
	private boolean emptyLibPreview;
	private int xSize = 100;
	private int ySize = 100;
	private ArrayList<String> libs = new ArrayList<String>();
	private JPanel seqBuscaPanel;
	private JPanel seqBuscaRightPanel;
	private JPanel seqBuscaLeftPanel;
	private JSplitPane jsp;
	private TranslationsManager tm;

	public SearchDrawer(int xSize,int ySize){
		this.xSize = xSize;
		this.ySize = ySize;
		this.tm = TranslationsManager.getInstance();
		seqOriginal = new JBaseTextField(25);
		seqOriginal.setMaximumSize(new Dimension(2*xSize, 30));
		seqBusca = new JLabel();
		jl = new JList<String>();
		listModel = new DefaultListModel<String>();  
		jl.setModel(listModel);
		libContainer = new JTabbedPane(JTabbedPane.TOP,JTabbedPane.SCROLL_TAB_LAYOUT);
		seqBuscaPanel = new JPanel();
		seqBuscaPanel.setBorder(BorderFactory.createLineBorder(Color.gray));
		seqBuscaRightPanel = new JPanel(new BorderLayout());
		seqBuscaLeftPanel = new JPanel();
		drawSearchContainer();
	}

	public JPanel getContainer(){
		return seqBuscaPanel;
	}

	private void drawSearchContainer(){		
		// Configura tab para sequencias
		seqBuscaLeftPanel.setLayout(new GridBagLayout());
		GridBagConstraints c = new GridBagConstraints();
		seqBuscaLeftPanel.setBorder(new EmptyBorder(10,10,10,10));
		
		//Adiciona nova linha hbox
		// Configura linha
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.05;
	    c.weightx = 0.2;
	    c.gridx = 0;
	    c.gridy = 0;
	    c.gridwidth = 1;
		seqBuscaLeftPanel.add(new JLabel(tm.getText("targetSequenceSetLabel")),c);
		
		// Configura linha
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.05;
	    c.weightx = 0.7;
	    c.gridx = 1;
	    c.gridy = 0;
	    c.gridwidth = 1;
	    seqBuscaLeftPanel.add(seqOriginal,c);
	    
		// Configura linha
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.05;
	    c.weightx = 0.1;
	    c.gridx = 2;
	    c.gridy = 0;
	    c.gridwidth = 1;
		setSeqButton = new JButton(tm.getText("targetSequenceSetButton"));
		setSeqButton.addActionListener(this);
		setSeqButton.setActionCommand("Set");
		seqBuscaLeftPanel.add(setSeqButton,c);
		
		// Configura linha
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.05;
	    c.weightx = 0.2;
	    c.gridx = 0;
	    c.gridy = 1;
	    c.gridwidth = 1;
	    seqBuscaLeftPanel.add(new JLabel(tm.getText("targetSequenceLabel")),c);
	    
		// Configura linha
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.05;
	    c.weightx = 0.8;
	    c.gridx = 1;
	    c.gridy = 1;
	    c.gridwidth = 2;
	    seqBuscaLeftPanel.add(seqBusca,c);
	
		// Nova linha com as previews
		// Configura linha
	    c.fill = GridBagConstraints.HORIZONTAL;
	    c.weighty = 0.05;
	    c.weightx = 0.2;
	    c.gridx = 0;
	    c.gridy = 2;
	    c.gridwidth = 1;
	    seqBuscaLeftPanel.add(new JLabel(tm.getText("librariesLoadedLabel")),c);
	    
		// Configura linha na libsLoaded
		JScrollPane jscrlp = new JScrollPane(jl);
		// Configura linha
	    c.fill = GridBagConstraints.BOTH;
	    c.weighty = 0.20;
	    c.weightx = 0.8;
	    c.gridx = 1;
	    c.gridy = 2;
	    c.gridwidth = 2;
		seqBuscaLeftPanel.add(jscrlp,c);

		// Adiciona nova linha hbox
		JPanel loads = new JPanel(new FlowLayout(FlowLayout.RIGHT));
		JButton loadLib = new JButton(tm.getText("loadButton"));
		JButton unloadLib = new JButton(tm.getText("unloadButton"));
		loadLib.setActionCommand("Load");
		unloadLib.setActionCommand("Unload");
		loadLib.addActionListener(this);
		unloadLib.addActionListener(this);
		loads.add(loadLib);
		loads.add(unloadLib);
		// Configura linha
	    c.fill = GridBagConstraints.NONE;
	    c.weighty = 0.05;
	    c.weightx = 1;
	    c.gridx = 1;
	    c.gridy = 3;
	    c.gridwidth = 1;
	    c.anchor = GridBagConstraints.EAST;
		seqBuscaLeftPanel.add(loads,c);
		
		// Adiciona nova linha hbox
		JPanel parameters = new JPanel(new BorderLayout());
		Box vbox = Box.createVerticalBox();

		// Full Sequence
		JPanel fulLSequencePanel = new JPanel(new BorderLayout());
		JCheckBox fullSequenceCheckBox = new JCheckBox(tm.getText("parameterStoreFullSequence"));
		fullSequenceCheckBox.setSelected(false);
		SummaryDrawer.setStoreFullSequence(false);
		fullSequenceCheckBox.addItemListener(new ItemListener(){
			// Ativa suporte a armazenamento total das sequencias processadas
			@Override
			public void itemStateChanged(ItemEvent e) {
				if(e.getStateChange() == ItemEvent.SELECTED){
					SummaryDrawer.setStoreFullSequence(true);
				}else if(e.getStateChange() == ItemEvent.DESELECTED){
					SummaryDrawer.setStoreFullSequence(false);
				}
			}

		});
		fulLSequencePanel.add(fullSequenceCheckBox,BorderLayout.WEST);

		// CLinha		
		JCheckBox CLinhaCutCheckBox = new JCheckBox(tm.getText("parameter5lCutSequence"));
		CLinhaCutCheckBox.setSelected(false);
		JPanel ClDistPanel = new JPanel();
		JPanel ClTamPanel = new JPanel();
		SpinnerModel spinnermodelTam = new SpinnerNumberModel(0, 0, 1e20, 1);
		SpinnerModel spinnermodelDist = new SpinnerNumberModel(0, 0, 1e20, 1);
		final JSpinner spinnerTam = new JSpinner(spinnermodelTam); 
		final JSpinner spinnerDist = new JSpinner(spinnermodelDist); 
		Dimension d = spinnerTam.getPreferredSize();  
		d.width = 50;  
		spinnerTam.setPreferredSize(d);  
		spinnerDist.setPreferredSize(d);  
		spinnerTam.setEnabled(false);
		spinnerDist.setEnabled(false);
		spinnerTam.addChangeListener(new ChangeListener() {
			// Seta tamanho da região 5l
			@Override
			public void stateChanged(ChangeEvent arg0) {
				Object obj = arg0.getSource();
				if(obj instanceof JSpinner){
					JSpinner spinner = (JSpinner)obj;
					SummaryDrawer.set5lTam(Math.round((Double)spinner.getValue()));
				}
			}
		});
		spinnerDist.addChangeListener(new ChangeListener(){
			// Seta posição da região 5l
			@Override
			public void stateChanged(ChangeEvent arg0) {
				Object obj = arg0.getSource();
				if(obj instanceof JSpinner){
					JSpinner spinner = (JSpinner)obj;
					SummaryDrawer.set5lDim(Math.round((Double)spinner.getValue()));
				}
			}
		});
		CLinhaCutCheckBox.addItemListener(new ItemListener(){
			// Ativa recorte 5l
			@Override
			public void itemStateChanged(ItemEvent e) {
				if(e.getStateChange() == ItemEvent.SELECTED){
					spinnerTam.setEnabled(true);
					spinnerDist.setEnabled(true);
					SummaryDrawer.setCLinhaSequences(true);
				}else if(e.getStateChange() == ItemEvent.DESELECTED){
					spinnerTam.setEnabled(false);
					spinnerDist.setEnabled(false);
					SummaryDrawer.setCLinhaSequences(false);
				}
			}

		});
		JPanel CLinhaPanel = new JPanel(new BorderLayout());
		Box vboxInterno = Box.createVerticalBox();
		ClDistPanel.add(new JLabel(tm.getText("parameter5lCutDistOption")));
		ClDistPanel.add(spinnerDist);
		ClTamPanel.add(new JLabel(tm.getText("parameter5lCutTamOption")));
		ClTamPanel.add(spinnerTam);
		vboxInterno.add(ClDistPanel);
		vboxInterno.add(ClTamPanel);
		CLinhaPanel.add(CLinhaCutCheckBox,BorderLayout.WEST);
		CLinhaPanel.add(vboxInterno,BorderLayout.EAST);		
		// Configura linha
	    c.fill = GridBagConstraints.BOTH;
	    c.weighty = 0.20;
	    c.weightx = 1;
	    c.gridx = 0;
	    c.gridy = 4;
	    c.gridwidth = 2;
	    
	    JLabel optionsLabel = new JLabel(tm.getText("parameterOptionsLabel"));
	    optionsLabel.setFont(optionsLabel.getFont().deriveFont(optionsLabel.getFont().getStyle() | Font.BOLD));
	    
		vbox.add(optionsLabel);
		vbox.add(fulLSequencePanel);
		vbox.add(CLinhaPanel);
		parameters.add(vbox);
		seqBuscaLeftPanel.add(parameters,c);
		
		// Configura linha
		drawEmptyLibsContainer();
	    seqBuscaRightPanel.add(libContainer);  
	    
		seqBuscaPanel.setLayout(new BorderLayout());
		//JScrollPane jscrp = new JScrollPane(seqBuscaLeftPanel);
		jsp = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,true,seqBuscaLeftPanel,seqBuscaRightPanel);
		jsp.setOneTouchExpandable(true);
	    c.fill = GridBagConstraints.BOTH;
	    c.weighty = 1;
	    c.weightx = 1;
	    c.gridx = 0;
	    c.gridy = 0;
		seqBuscaPanel.add(jsp,BorderLayout.CENTER);

		return;
	}

	private void drawEmptyLibsContainer(){	
		emptyLibPreview = true;
		JPanel jp = new JPanel (new BorderLayout());
		libContainer.removeAll();		
		JLabel emptyLabel = new JLabel(tm.getText("libPreviewEmptyMsg"));
		jp.add(emptyLabel,BorderLayout.CENTER);
		libContainer.add(jp);
		return;
	}

	private void loadLibPreview(String libPath){
		if(emptyLibPreview){
			libContainer.removeAll();		
			emptyLibPreview = false;
		}
		// Esse metodo deve ser chamado para carregar uma lib
		try{
			// Abre biblioteca
			Library lib = new Library(new File(libPath));
			if(!lib.canRead()){
				throw new FileNotFoundException();
			}
			
			// Inicializa container que irah guardar os dados
			JPanel jp = new JPanel(new BorderLayout());
			final JLibPreviewTableModel jltm = new JLibPreviewTableModel(lib);
			
			// Inicializa tabela que irah mostrar as sequencias
			JTable jtabPreviewLibs = new JTable(jltm);
			
			// Seta configuracoes de render para a tabela
			DefaultTableCellRenderer indexRenderer = new DefaultTableCellRenderer();
			indexRenderer.setHorizontalAlignment(DefaultTableCellRenderer.CENTER);
			jtabPreviewLibs.getColumnModel().getColumn(0).setCellRenderer(new JTableRenderer(indexRenderer));
			DefaultTableCellRenderer seqRenderer = new DefaultTableCellRenderer();
			jtabPreviewLibs.getColumnModel().getColumn(1).setCellRenderer(new JTableRenderer(seqRenderer));
			jtabPreviewLibs.setAutoResizeMode(JTable.AUTO_RESIZE_OFF);
			jtabPreviewLibs.getColumnModel().getColumn(0).setPreferredWidth(40);
			jtabPreviewLibs.getColumnModel().getColumn(1).setPreferredWidth((int)(ySize*0.84));
			//SelectionListener listener = new SelectionListener(jtabPreviewLibs);
			jtabPreviewLibs.getSelectionModel().setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
			jtabPreviewLibs.setAutoscrolls(true);

			// Insere JTable dentro de JScrollPane
			JScrollPane jscrlpLib  = new JScrollPane(jtabPreviewLibs);
			
			// Define lazy load
			JScrollBar jsb = jscrlpLib.getVerticalScrollBar();
			jsb.addAdjustmentListener(new AdjustmentListener(){
				@Override
				public void adjustmentValueChanged(AdjustmentEvent e) {
					JScrollBar jsb = (JScrollBar) e.getSource();
					int jsbMax = jsb.getMaximum();
					int jsbPos = jsb.getValue();
					if(jsbMax*0.8 <= jsbPos){
						jltm.loadMore();
					}
				}					
			});

			// Insere o JScrollPane com a Lib carregada dentro do JPane
			//jp.add(new JLabel(Long.toString(lib.getFileSize())));
			jp.add(jscrlpLib,BorderLayout.CENTER);

			// Adiciona aba com a lib carregada
			libContainer.addTab(lib.getFilename(),jp);

			//initTabsComponents(libContainer);

			Drawer.writeToLog(libPath+tm.getText("libFileLoad"));
		}catch(FileNotFoundException e){
			Drawer.writeToLog(libPath+tm.getText("libFileDoNotLoad"));
		}


		return;
	}
	
	private void unloadLibPreview(int index){
		if(index < libContainer.getTabCount()){
			libContainer.remove(index);
			if(libContainer.getTabCount() == 0){
				drawEmptyLibsContainer();
			}
		}
		return;
	}
	
	private void unloadAllLibPreviews(){
		libContainer.removeAll();
		drawEmptyLibsContainer();
	}

	public static String getTargetSeq(){
		return seqBusca.getText();
	}

	@Override
	public void actionPerformed(ActionEvent ae) {
		switch(ae.getActionCommand()){
		case "Set":
			if(seqOriginal.getText().length() > 0){
				if(seqOriginal.getSelectedText() != null)
					searchSeq = seqOriginal.getSelectedText();
				else	
					searchSeq = seqOriginal.getText();
				Drawer.writeToLog(tm.getText("targetSequenceLabel") + searchSeq);
				seqBusca.setText(searchSeq);
				SummaryDrawer.setTargetSeq(searchSeq);
				libContainer.repaint();
			}
			break;
		case "Load":
			JFileChooser jfc = new JFileChooser(System.getProperty("user.dir"));
			//jfc.setCurrentDirectory(new File(h.getOutput()));
			jfc.setFileFilter(new JTxtFileFilter());
			jfc.setMultiSelectionEnabled(true);
			if(jfc.showOpenDialog(null) == JFileChooser.APPROVE_OPTION){
				for(File f: jfc.getSelectedFiles()){
					String txt = f.getAbsolutePath();
					if(f.canRead()){						
						libs.add(txt);
						loadLibPreview(txt);
						SummaryDrawer.addLoadedLib(txt);
						listModel.addElement(txt);
					}else{
						Drawer.writeToLog(txt+tm.getText("libFileDoNotLoad"));
					}
				}
			}
			break;
		case "Unload":
			List<String> elements =jl.getSelectedValuesList();
			for(String ele: elements){
				int index = libs.indexOf(ele);
				unloadLibPreview(index);
				SummaryDrawer.removeLoadedLib(ele);
				libs.remove(ele);
				listModel.removeElement(ele);
				Drawer.writeToLog("File "+ele+tm.getText("libFileUnload"));
			}
			break;
		}
	}
	
	private void unloadAllLibs(){
		for(int i = 0;i < libs.size();i++){
			try{
				SummaryDrawer.removeLoadedLib(libs.get(i));
			}catch(IndexOutOfBoundsException e){
				System.out.println(e.getMessage());
			}
		}
		libs.clear();
		listModel.removeAllElements();
	}
	
	private void unsetTarget(){
		searchSeq = "";
		seqOriginal.setText("");
		seqBusca.setText("");
		SummaryDrawer.setTargetSeq("");
	}
	public void cleanHunt(){
		// Unload em todas as libs abertas e grava null em searchSeq	
		unloadAllLibs();
		unloadAllLibPreviews();
		unsetTarget();
		Drawer.writeToLog(tm.getText("cleanHuntMsg"));
	}

}
