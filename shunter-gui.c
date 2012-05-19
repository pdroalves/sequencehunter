#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAJORV 0
#define MINORV 0
#define LOG_MAX 10e7

GtkEntry *hunt_entry;
GtkEntry *hunt;
gchar ver[10];
GtkWidget *log_window;
char *log;
const gchar *authors[] = {
	"Pedro Alves",
	"Marcio Chaim Bajgelman"
};
static GtkWidget *spinner_sensitive = NULL;
GtkWidget *status;
gint start_pos;
gint end_pos;

static void log_update(GtkWidget *widget,char *text);
void show_about(GtkWidget *widget, gpointer data);

#define NUM_ENTRIES 7
static GtkActionEntry entries[] =
{
	{"File",GTK_STOCK_FILE,"_Arquivo",NULL,NULL,NULL},
	{"New",GTK_STOCK_NEW,"_Novo",NULL,"Limpa configurações",NULL},
	{"Open",GTK_STOCK_OPEN,"_Abrir",NULL,"Carrega biblioteca",NULL},
	{"Save",GTK_STOCK_SAVE,"_Salvar modelos",NULL,"Salva resultados em disco",NULL},
	{"Quit",GTK_STOCK_QUIT,"_Sair",NULL,"Fecha a aplicação",G_CALLBACK(gtk_main_quit)},
	{"Help",GTK_STOCK_HELP,"A_juda",NULL,NULL,NULL},
	{"About",GTK_STOCK_ABOUT,"S_obre",NULL,NULL,G_CALLBACK(show_about)}
};



/* converts integer into string */
char* itoa_(unsigned long num) {
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

static void
on_hunt_clicked (GtkButton *button, gpointer user_data)
{
  gtk_widget_show(spinner_sensitive);
  gtk_spinner_start (GTK_SPINNER (spinner_sensitive));
}

static void
on_hunt_end (GtkButton *button, gpointer user_data)
{
  gtk_spinner_stop (GTK_SPINNER (spinner_sensitive));
}

void set_hunt(GtkWidget *widget, gpointer data){
	gchar *text;
	
	printf("Seleção detectada!\n");
	
	if(gtk_editable_get_selection_bounds(GTK_EDITABLE(hunt_entry),&start_pos,&end_pos)){
		text = gtk_editable_get_chars(GTK_EDITABLE(hunt_entry),start_pos,end_pos);
		gtk_entry_set_text(hunt,text);
		char *log_seq;
		log_seq = (char*)malloc((strlen(text)+30)*sizeof(char));
		strcpy(log_seq,"\nSequência de busca definida: ");
		strcat(log_seq,text);
		log_update(NULL,log_seq);
	}
	return;
}

static void do_highlight(GtkWidget *widget,gpointer data){
	printf("Highlight!");
	gtk_editable_select_region(hunt_entry,start_pos,end_pos);
	
	return;
}

static void do_resize(GtkExpander *expander,GtkWindow *window){
	printf("Expander!");
	if(gtk_expander_get_expanded(expander)){
		gtk_window_set_default_size(window,800,200);
	}else{
		gtk_window_set_default_size(window,500,200);
	}
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

static void set_hunt_ready(GtkWidget *widget,GtkWidget *hunt_button){
	GtkTextBuffer *buffer;
	
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (log_window));
	if(strcmp(gtk_text_buffer_get_text(buffer,0,4,FALSE), "Pronto")){
		gtk_widget_set_sensitive(hunt_button,TRUE);
		log_update(NULL,"\nPronto para começar.");
	}
	return;
}

void log_init(){
	GtkTextBuffer *buffer;
	
	gtk_text_view_set_editable(GTK_TEXT_VIEW(log_window),FALSE);

	gtk_text_view_set_border_window_size(GTK_TEXT_VIEW(log_window),GTK_TEXT_WINDOW_TOP,5);
	gtk_text_view_set_border_window_size(GTK_TEXT_VIEW(log_window),GTK_TEXT_WINDOW_LEFT,5);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (log_window));
	log = (char*)malloc(LOG_MAX*sizeof(char));
	strcpy(log,"Iniciando Sequence Hunter ");
	strcat(log,ver);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (log_window));
	gtk_text_buffer_set_text (buffer, log, -1);
	return;
}

static void log_update(GtkWidget *widget,char *text){
	
	GtkTextBuffer *buffer;
	strcat(log,text);
	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (log_window));
	gtk_text_buffer_set_text (buffer, log, -1);
	return;
}

void show_about(GtkWidget *widget, gpointer data)
{
  GtkWidget *dialog = gtk_about_dialog_new();
  gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Sequence Hunter");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), ver);
  gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(dialog),authors); 
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
	
	GtkWidget *new;
	GtkWidget *load;
	GtkWidget *save;
	GtkWidget *sep;
	GtkWidget *exit;
	
	GtkWidget *hbox,*vbox;
	GtkWidget *hunt_label;
	GtkWidget *hunt_button;
	GtkWidget *spinner;
	GtkWidget *set_button;
	GtkActionGroup *group;
	GtkUIManager *uimanager;
	
	GtkWidget *label_seq_intro;
	GtkWidget *load_data;
	GtkWidget *data_integrity;
	GtkWidget *hunt_seq;
	GtkWidget *cuda_support;
	GtkTextBuffer *buffer;
	
	GtkWidget *scrolled;
	GtkWidget *expander;
  
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
	//gtk_widget_set_sensitive(hunt_button, FALSE);
	set_button = gtk_button_new_with_mnemonic("_Set");
	hbox = gtk_hbox_new(FALSE,2);
	vbox = gtk_vbox_new(FALSE,2);
	label_seq_intro = gtk_label_new("Sequência configurada para busca:");
	strcpy(ver,"RC - ");
	strcat(ver,itoa_(MAJORV));
	strcat(ver,".");
	strcat(ver,itoa_(MINORV));
	start_pos = end_pos = -1;
	
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
	log_init();
	spinner = gtk_spinner_new ();
	spinner_sensitive = spinner;
	
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
	
	
	//Configura log_window	
	scrolled = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW (scrolled), log_window);
	
	//Configura janela
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),title);
	gtk_window_set_resizable(GTK_WINDOW(window),TRUE);
	gtk_window_set_default_size(GTK_WINDOW(window),500,200);
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);

	//Filemenu
	group = gtk_accel_group_new();
	menubar = gtk_menu_bar_new();
	file = gtk_menu_item_new_with_mnemonic("_Arquivo");
	filemenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM (file), filemenu);
	gtk_menu_shell_append (GTK_MENU_SHELL (menubar), file);

	//Configurando menu
	group = gtk_action_group_new("MainActionGroup");
	gtk_action_group_add_actions (group, entries, NUM_ENTRIES,NULL);
	
	uimanager = gtk_ui_manager_new();
	gtk_ui_manager_insert_action_group(uimanager,group,0);
	gtk_ui_manager_add_ui_from_file(uimanager,"menu.ui",NULL);
	
	menubar = gtk_ui_manager_get_widget(uimanager,"/MenuBar");

	gtk_box_pack_start(GTK_BOX(vbox), menubar,FALSE,FALSE,1);
	gtk_window_add_accel_group (GTK_WINDOW (window),gtk_ui_manager_get_accel_group(uimanager));
	
	//Configura botões file menu
	g_signal_connect(window,"delete-event",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(hunt,"changed",G_CALLBACK(hunt_seq_set),hunt_seq);
	g_signal_connect(hunt_entry,"insert_text",G_CALLBACK(insert_text_handler),NULL);
	g_signal_connect(set_button,"clicked",G_CALLBACK(set_hunt),NULL);
	g_signal_connect(status,"preedit-changed",G_CALLBACK(set_hunt_ready),hunt_button);
    g_signal_connect (G_OBJECT (hunt_button), "clicked",G_CALLBACK (on_hunt_clicked), spinner);
    g_signal_connect (hunt,"activate",G_CALLBACK(do_highlight),NULL);
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
	
	hbox = gtk_hbox_new(FALSE,1);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_hbox_new(TRUE,2), FALSE, FALSE,150);
	gtk_box_pack_start(GTK_BOX(hbox),hunt_button,TRUE,TRUE,10);
	gtk_box_pack_start(GTK_BOX(hbox),spinner,FALSE,FALSE,2);
	gtk_box_pack_start(GTK_BOX(vbox),hbox, FALSE, FALSE,20);
	
	expander = gtk_expander_new ("Mais detalhes:");
    gtk_box_pack_start (GTK_BOX(vbox), expander, TRUE, TRUE, 10);
	gtk_container_add (GTK_CONTAINER (expander), scrolled);
 
	gtk_container_add(GTK_CONTAINER(window),vbox);
	gtk_rc_parse("gtkrc");
	gtk_widget_show_all(window);
	gtk_widget_hide(spinner);
	gtk_main();
	return 0;
}
