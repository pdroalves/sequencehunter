package com.lnbio.xml;

import java.util.HashMap;

public class TranslationsManager {
	// Padrão singleton
	private final static String defaultLanguageFile = "com/lnbio/resources/translations/EnUS.xml";
	private final static String tagName = "TextResource";
	private static XMLReader xmlReader;
	private static HashMap<String, String> textHM;
	private static TranslationsManager instance = new TranslationsManager();
	
	private TranslationsManager(){
		
	}
	
	private TranslationsManager(String language){
		// Define a linguagem a ser carregada

		xmlReader = new XMLReader(language,tagName);
		textHM = xmlReader.getSet();
	}
	
	public static TranslationsManager getInstance(){
		return instance;
	}
	
	public void setDefaultLanguage(){
		xmlReader = new XMLReader(defaultLanguageFile,tagName);
		textHM = xmlReader.getSet();
	}
	
	public String getText(String tag){
		String s = textHM.get(tag);
		if(s != null)
			return s;
		else
			return "null";
	}
}
