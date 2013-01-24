package Auxiliares;
import javax.swing.JTextField;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.PlainDocument;


public class JBaseTextField extends JTextField {
	public JBaseTextField(int cols) {
		super(cols);
	}

	protected Document createDefaultModel() {
		return new BaseDocument();
	}

	static class BaseDocument extends PlainDocument {

		public void insertString(int offs, String txt, AttributeSet a) throws BadLocationException {
			// Aceita apenas as bases ACGTN e converte sempre para a forma maiuscula
			if(txt == null)
				return;
			
			for(int i = txt.length()-1; i >= 0; i--){
				char str = txt.charAt(i);
				
				if (str == 'A' || str == 'a' ||
						str == 'C' || str == 'c' ||
						str == 'G' || str == 'g' ||
						str == 'T' || str == 't' ||
						str == 'N' || str == 'n'){
					char upper = Character.toUpperCase(str);
					super.insertString(offs, String.valueOf(upper), a);
				}
			}
		}
	}
}
