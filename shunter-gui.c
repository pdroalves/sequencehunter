#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAJORV 0
#define MINORV 0

GtkEntry *hunt_entry;
GtkEntry *hunt;

/* converts integer into string */
char* itoa(unsigned long num) {
        char* retstr = calloc(12, sizeof(char));
        if (sprintf(retstr, "%ld", num) > 0) {
                return retstr;
        } else {
                return NULL;
        }
}

int isbase(const gchar *c){
	if( strcmp(c,"A") == 0 || strcmp(c,"a") == 0 || strcmp(c,"C") == 0 || strcmp(c,"c") == 0 || strcmp(c,"T") == 0 || strcmp(c,"t") == 0 || strcmp(c,"G") == 0 || strcmp(c,"g") == 0 || strcmp(c,"N") == 0 || strcmp(c,"n") ==0)
		return 0;
	return 1;
}

void insert_text_handler (GtkEntry *entry,
							const gchar *text,
							gint length,
							gint *position,
							gpointer data){
	GtkEditable *editable = GTK_EDITABLE(entry);
	int i, count=0;
	gchar *result = g_new (gchar, length);

	for (i=0; i < length; i++) {
		if (isbase(&text[i]))
		continue;
		result[count++] = islower(text[i]) ? toupper(text[i]) : text[i];
	}

	if (count > 0) {
		g_signal_handlers_block_by_func (G_OBJECT (editable), G_CALLBACK (insert_text_handler), data);
		gtk_editable_insert_text (editable, result, count, position);
		g_signal_handlers_unblock_by_func (G_OBJECT (editable), G_CALLBACK (insert_text_handler), data);
	}
	g_signal_stop_emission_by_name (G_OBJECT (editable), "insert_text");

	g_free (result);
}


void set_hunt(GtkWidget *widget, gpointer data){
	gchar *text;
	gint start_pos;
	gint end_pos;
	printf("Seleção detectada!\n");
	if(gtk_editable_get_selection_bounds(GTK_EDITABLE(hunt_entry),&start_pos,&end_pos)){
		text = gtk_editable_get_chars(GTK_EDITABLE(hunt_entry),start_pos,end_pos);
		gtk_entry_set_text(hunt,text);
	}
	return;
}

void hunt_seq_set(GtkWidget *widget,GtkWidget *hunt_seq){
	
	int seq_size;
	GdkColor color;
	GtkTextBuffer *buffer;
	seq_size = gtk_entry_get_text_length(hunt);
	
	if(seq_size > 0){
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (hunt_seq));
		gtk_text_buffer_set_text (buffer, "Configurado.", -1);

		gdk_color_parse ("green", &color);
		gtk_widget_modify_text (hunt_seq, GTK_STATE_NORMAL, &color);
	}else{
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (hunt_seq));
		gtk_text_buffer_set_text (buffer, "Sequência não configurada.", -1);

		gdk_color_parse ("red", &color);
		gtk_widget_modify_text (hunt_seq, GTK_STATE_NORMAL, &color);
	}
}

void show_about(GtkWidget *widget, gpointer data)
{

  char ver[10];

  strcpy(ver,itoa(MAJORV));
  strcat(ver,".");
  strcat(ver,itoa(MINORV));

  GtkWidget *dialog = gtk_about_dialog_new();
  gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Sequence Hunter");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), ver); 
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), 
      "Copyright");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
     "Comentários.");
  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog), 
      "http://www.site.net");
  //gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
  //g_object_unref(pixbuf), pixbuf = NULL;
  gtk_dialog_run(GTK_DIALOG (dialog));
  gtk_widget_destroy(dialog);

}

int main (int argc, char *argv[]){
	
	GtkWidget *window;
	GtkWidget *menubar;
	GtkWidget *file;
	GtkWidget *about;
	GtkWidget *filemenu;
	
	GtkWidget *toolbar;
	GtkWidget *new;
	GtkWidget *load;
	GtkWidget *save;
	GtkWidget *sep;
	GtkWidget *exit;
	
	GtkWidget *hbox,*vbox;
	GtkWidget *hunt_label;
	GtkWidget *hunt_button;
	GtkWidget *set_button;
	GtkAccelGroup *group;
	
	GtkWidget *label_seq_intro;
	GtkWidget *status;
	GtkWidget *load_data;
	GtkWidget *data_integrity;
	GtkWidget *hunt_seq;
	GtkWidget *cuda_support;
	GtkTextBuffer *buffer;
	
	GtkWidget *log_window;
	GtkWidget *scrolled;
	
	GtkTextIter start, end;
	PangoFontDescription *font_desc;
	GdkColor color;
	GtkTextTag *tag;
	char *title;

	gtk_init(&argc,&argv);

	title = (char*)malloc(50*sizeof(char));
	strcpy(title,"Sequence Hunter");
	hunt_label = gtk_label_new("Sequência a ser buscada:");
	hunt_entry = gtk_entry_new();
	hunt = gtk_entry_new();
	gtk_entry_set_editable(hunt,FALSE);
	hunt_button = gtk_button_new_with_mnemonic("_Hunt");
	set_button = gtk_button_new_with_mnemonic("_Set");
	hbox = gtk_hbox_new(FALSE,2);
	vbox = gtk_vbox_new(FALSE,2);
	label_seq_intro = gtk_label_new("Sequência configurada para busca:");
	
	//Configura texto de status
	status = gtk_text_view_new ();
	load_data = gtk_text_view_new ();
	data_integrity = gtk_text_view_new ();
	hunt_seq = gtk_text_view_new ();
	cuda_support = gtk_text_view_new ();
	log_window = gtk_text_view_new();
	
	gtk_text_view_set_editable(status,FALSE);
	gtk_text_view_set_editable(load_data,FALSE);
	gtk_text_view_set_editable(data_integrity,FALSE);
	gtk_text_view_set_editable(hunt_seq,FALSE);
	gtk_text_view_set_editable(cuda_support,FALSE);
	gtk_text_view_set_editable(log_window,FALSE);
	
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (status));
	gtk_text_buffer_set_text (buffer, "Não está pronto.", -1);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (load_data));
	gtk_text_buffer_set_text (buffer, "Não carregado.", -1);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (data_integrity));
	gtk_text_buffer_set_text (buffer, "Não carregado.", -1);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (hunt_seq));
	gtk_text_buffer_set_text (buffer, "Sequência não configurada.", -1);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (cuda_support));
	gtk_text_buffer_set_text (buffer, "Sem suporte.", -1);

	/* Change default color throughout the widget */
	gdk_color_parse ("red", &color);
	gtk_widget_modify_text (status, GTK_STATE_NORMAL, &color);
	gdk_color_parse ("red", &color);
	gtk_widget_modify_text (load_data, GTK_STATE_NORMAL, &color);
	gdk_color_parse ("red", &color);
	gtk_widget_modify_text (data_integrity, GTK_STATE_NORMAL, &color);
	gdk_color_parse ("red", &color);
	gtk_widget_modify_text (hunt_seq, GTK_STATE_NORMAL, &color);
	gdk_color_parse ("red", &color);
	gtk_widget_modify_text (cuda_support, GTK_STATE_NORMAL, &color);

	/* Change left margin throughout the widget */
	gtk_text_view_set_left_margin (GTK_TEXT_VIEW (status), 3);
	
	//Configura log_window	
	scrolled = gtk_scrolled_window_new(gtk_adjustment_new(10,5,20,1,1,20),gtk_adjustment_new(10,5,20,1,1,20));
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW (scrolled), log_window);
	
	//Configura janela
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),title);
	gtk_window_set_resizable(GTK_WINDOW(window),TRUE);
	gtk_window_set_default_size(GTK_WINDOW(window),600,500);
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);

	//Filemenu
	group = gtk_accel_group_new();
	menubar = gtk_menu_bar_new();
	file = gtk_menu_item_new_with_mnemonic("_Arquivo");
	filemenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM (file), filemenu);
	gtk_menu_shell_append (GTK_MENU_SHELL (menubar), file);
	
	//Toolbar
	toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

	gtk_container_set_border_width(GTK_CONTAINER(toolbar), 2);

	new = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), new, -1);

	load = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), load, -1);	

	save = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), save, -1);


	gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 5);

	//Configura file menu
	new = gtk_image_menu_item_new_from_stock (GTK_STOCK_NEW,group);
	load = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN,group);
	save = gtk_image_menu_item_new_from_stock (GTK_STOCK_SAVE,group);
	about = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT,group);
	sep = gtk_separator_menu_item_new();
	exit = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT,group);
	gtk_menu_shell_append (GTK_MENU_SHELL (filemenu), new);
	gtk_menu_shell_append (GTK_MENU_SHELL (filemenu),load);
	gtk_menu_shell_append (GTK_MENU_SHELL (filemenu),save);
	gtk_menu_shell_append (GTK_MENU_SHELL (filemenu),about);
	gtk_menu_shell_append (GTK_MENU_SHELL (filemenu),sep);
	gtk_menu_shell_append (GTK_MENU_SHELL (filemenu),exit);
	
	gtk_box_pack_start(GTK_BOX(vbox), menubar,FALSE,FALSE,1);
	gtk_window_add_accel_group (GTK_WINDOW (window),group);

	//Configura botões file menu
	g_signal_connect(window,"delete-event",G_CALLBACK(gtk_main_quit),NULL);
	//g_signal_connect(hunt,"clicked",G_CALLBACK(start_hunt),NULL);
	g_signal_connect(GTK_MENU(exit),"activate",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(GTK_MENU(about),"activate",G_CALLBACK(show_about),NULL);
	g_signal_connect(hunt_entry,"insert_text",G_CALLBACK(insert_text_handler),NULL);
	g_signal_connect(set_button,"clicked",G_CALLBACK(set_hunt),NULL);
	g_signal_connect(hunt,"changed",G_CALLBACK(hunt_seq_set),hunt_seq);
	//Desenha a janela
	
	hbox = gtk_hbox_new(FALSE,2);
	gtk_box_pack_start(GTK_BOX(hbox), hunt_label, FALSE,TRUE,4);
	gtk_box_pack_start(GTK_BOX(vbox),hbox, FALSE, FALSE,5);
	
	hbox = gtk_hbox_new(FALSE,2);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(hunt_entry),TRUE,TRUE,4);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(set_button),FALSE,FALSE,1);
	gtk_box_pack_start(GTK_BOX(vbox),hbox, FALSE, FALSE,2);

	hbox = gtk_hbox_new(FALSE,2);
	gtk_box_pack_start(GTK_BOX(hbox),label_seq_intro,FALSE,FALSE,10);
	gtk_box_pack_start(GTK_BOX(hbox),hunt,TRUE,TRUE,40);
	gtk_box_pack_start(GTK_BOX(vbox),hbox, FALSE, TRUE,12);
	
	hbox = gtk_hbox_new(TRUE,2);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Dados carregados:"),FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hbox),load_data,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(vbox),hbox, FALSE, FALSE,2);
	
	hbox = gtk_hbox_new(TRUE,2);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Integridade dos dados:"),FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hbox),data_integrity,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(vbox),hbox, FALSE, FALSE,2);
	
	hbox = gtk_hbox_new(TRUE,2);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Sequência para busca:"),FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hbox),hunt_seq,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(vbox),hbox, FALSE, FALSE,2);
	
	hbox = gtk_hbox_new(TRUE,2);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Suporte a CUDA:"),FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hbox),cuda_support,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(vbox),hbox, FALSE, FALSE,2);
	
	hbox = gtk_hbox_new(TRUE,2);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Status:"),FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hbox),status,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(vbox),hbox, FALSE, FALSE,2);
	
	gtk_box_pack_start(GTK_BOX(vbox),scrolled,TRUE,TRUE,10);
	
	gtk_container_add(GTK_CONTAINER(window),vbox);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
