package xml;

import java.util.HashMap;

public class TranslationsManager {
	private final String defaultLanguageFile = "src/resources/translations/EnUS.xml";
	private final String tagName = "TextResource";
	private XMLReader xmlReader;
	private HashMap<String, String> textHM;
	
	public TranslationsManager(){
		xmlReader = new XMLReader(defaultLanguageFile,tagName);
		textHM = xmlReader.getSet();
	}
	
	public TranslationsManager(String language){
		// Define a linguagem a ser carregada
		
	}
	
	public String getText(String tag){
		String s = textHM.get(tag);
		if(s != null)
			return s;
		else
			return "null";
	}
}
