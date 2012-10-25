#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cuda_functions.h"
#include "estruturas.h"
#include "load_data.h"

#define MAJORV 0
#define MINORV 0
#define LOG_MAX 10e7

static GtkWidget *list_library;
static GtkWidget *list_examples;
GtkEntry *hunt_entry;
GtkEntry *hunt;
gchar ver[10];
GtkWidget *log_window;
char *log;
const gchar *authors[] = {
	"Marcio Chaim Bajgelman",
	"Pedro Alves"
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

static enum{
		ARQUIVO,
		COUNT_SEQ,
		N_COL1
};

static enum{
		SEQ,
		ARQUIVO_SEQ,
		N_COL2
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

static GtkWidget *setup_list_library_added(){

        GtkWidget *sc_win;
        GtkListStore *store;
        GtkCellRenderer *cell;
        GtkTreeViewColumn *column;

        sc_win = gtk_scrolled_window_new(NULL,NULL);
        store = gtk_list_store_new(N_COL1, G_TYPE_STRING,G_TYPE_STRING);
        list_library = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
        gtk_tree_view_set_show_expanders(GTK_TREE_VIEW (list_library),TRUE);

		cell = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes("File",cell,"text",ARQUIVO,NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(list_library),column);
		
		cell = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes("Valid Sequences",cell,"text",COUNT_SEQ,NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(list_library),column);

		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sc_win), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

        gtk_container_add(GTK_CONTAINER(sc_win), list_library);

        g_object_unref(G_OBJECT(store));

        return sc_win;
}

static GtkWidget *setup_list_library_examples(){

        GtkWidget *sc_win;
        GtkListStore *store;
        GtkCellRenderer *cell;
        GtkTreeViewColumn *column;

        sc_win = gtk_scrolled_window_new(NULL,NULL);
        store = gtk_list_store_new(N_COL2, G_TYPE_STRING, G_TYPE_STRING);
        list_examples = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
        gtk_tree_view_set_show_expanders(GTK_TREE_VIEW (list_examples),TRUE);

		cell = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes("Sequence",cell,"text",SEQ,NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(list_examples),column);

		cell = gtk_cell_renderer_text_new();
		column = gtk_tree_view_column_new_with_attributes("File",cell,"text",ARQUIVO_SEQ,NULL);
		gtk_tree_view_append_column(GTK_TREE_VIEW(list_examples),column);

		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sc_win), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

        gtk_container_add(GTK_CONTAINER(sc_win), list_examples);

        g_object_unref(G_OBJECT(store));

        return sc_win;
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
	return;
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


void open_library(GtkWidget *widget, gpointer data)
{	
	
	GtkWidget *dialog;
	char **filename;

	filename = (char**)malloc(sizeof(char*));

	dialog = gtk_file_chooser_dialog_new ("Open File",
				      GTK_WINDOW(widget),
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER(dialog),TRUE);
	filename[0] = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	open_file(filename,1);
	
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

	GtkWidget *toolbar;
	GtkToolItem *open;
	GtkToolItem *about;
	GtkToolItem *sep;
	GtkToolItem *exit;
	
	GtkWidget *general_vbox,*hunt_vbox,*hunt_hbox,*lib_load_vbox,*lib_examples_vbox,*hbox,*vbox;
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
	GtkWidget *lib_examples_expander;
	
	GtkWidget *library_manager_label;
	GtkWidget *list_library_view;
	GtkTreeSelection *selection;
	GtkWidget *add_button;
	GtkWidget *remove_button;
	GtkWidget *process_button;
	
	GtkWidget *library_examples_label;
	GtkWidget *list_library_examples_view;
	GtkWidget *more_button;
	GtkWidget *less_button;
	
	GdkColor color;
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
	label_seq_intro = gtk_label_new("Sequência configurada para busca:");
	strcpy(ver,"RC - ");
	strcat(ver,itoa_(MAJORV));
	strcat(ver,".");
	strcat(ver,itoa_(MINORV));
	start_pos = end_pos = -1;
	
	spinner = gtk_spinner_new ();
	spinner_sensitive = spinner;
	
	//Configura texto de status
	status = gtk_text_view_new ();
	load_data = gtk_text_view_new ();
	data_integrity = gtk_text_view_new ();
	hunt_seq = gtk_text_view_new ();
	cuda_support = gtk_text_view_new ();
	log_window = gtk_text_view_new();
	
	log_init();
	
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
	if( check_gpu_mode() ){
		gdk_color_parse ("green", &color);
		gtk_text_buffer_set_text (buffer, "Suportado.", -1);
		log_update(NULL,"\nCUDA Mode");		
	}else{
		gdk_color_parse ("red", &color);
		gtk_text_buffer_set_text (buffer, "Sems suporte.", -1);
		log_update(NULL,"\nOpenMP Mode");
	}	
	gtk_widget_modify_text (cuda_support, GTK_STATE_NORMAL, &color);
	
	
	/* Change default color throughout the widget */
	gdk_color_parse ("red", &color);
	gtk_widget_modify_text (status, GTK_STATE_NORMAL, &color);
	gdk_color_parse ("red", &color);
	gtk_widget_modify_text (load_data, GTK_STATE_NORMAL, &color);
	gdk_color_parse ("red", &color);
	gtk_widget_modify_text (data_integrity, GTK_STATE_NORMAL, &color);
	gdk_color_parse ("red", &color);
	gtk_widget_modify_text (hunt_seq, GTK_STATE_NORMAL, &color);
	
	
	// Configura list_view das bibliotecas adicionadas
	library_manager_label = gtk_label_new("Librarys to Process:");
	list_library_view = setup_list_library_added();
	selection  = gtk_tree_view_get_selection(GTK_TREE_VIEW(list_library));
	
	add_button = gtk_button_new_with_mnemonic("_Add");
	remove_button = gtk_button_new_with_mnemonic("_Remove");
	process_button = gtk_button_new_with_mnemonic("_Process");
	
	//g_signal_connect(add_button,"clicked",G_CALLBACK(add_library),selection);
	//g_signal_connect(remove_button,"clicked",G_CALLBACK(remove_library),selection);
	
	
	// Configura list_view dos exemplos das bibliotecas adicionadas
	library_examples_label = gtk_label_new("Some examples:");
	list_library_examples_view = setup_list_library_examples();
	
		//Configura log_window	
	scrolled = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW (scrolled), log_window);
	
	//Configura janela
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),title);
	gtk_window_set_resizable(GTK_WINDOW(window),TRUE);
	gtk_window_set_default_size(GTK_WINDOW(window),800,800);
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);

	//Filemenu
	toolbar = gtk_toolbar_new();
	gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_ICONS);

	gtk_container_set_border_width(GTK_CONTAINER(toolbar), 2);

	open = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), open, -1);
	
	sep = gtk_separator_tool_item_new();
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), sep, -1);

	about = gtk_tool_button_new_from_stock(GTK_STOCK_ABOUT);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), about,-1);

	exit = gtk_tool_button_new_from_stock(GTK_STOCK_QUIT);
	gtk_toolbar_insert(GTK_TOOLBAR(toolbar), exit, -1);
	
	//Configura botões file menu
	g_signal_connect(open,"clicked",G_CALLBACK(open_library),window);
	g_signal_connect(about,"clicked",G_CALLBACK(show_about),window);
	g_signal_connect(exit,"clicked",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(window,"delete-event",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(hunt,"changed",G_CALLBACK(hunt_seq_set),hunt_seq);
	g_signal_connect(hunt_entry,"insert_text",G_CALLBACK(insert_text_handler),NULL);
	g_signal_connect(set_button,"clicked",G_CALLBACK(set_hunt),NULL);
	g_signal_connect(status,"preedit-changed",G_CALLBACK(set_hunt_ready),hunt_button);
    g_signal_connect (G_OBJECT (hunt_button), "clicked",G_CALLBACK (on_hunt_clicked), spinner);
    g_signal_connect (hunt,"activate",G_CALLBACK(do_highlight),NULL);
	//Desenha a janela

	//GtkWidget *general_vbox,*hunt_vbox,*lib_load_vbox,*lib_examples_vbox,*hbox,*vbox;
		
	general_vbox = gtk_vbox_new(FALSE,5);
	gtk_box_pack_start(GTK_BOX(general_vbox), toolbar, FALSE, FALSE, 5);
	hunt_hbox = gtk_hbox_new(FALSE,2);	
	
	lib_load_vbox = gtk_vbox_new(FALSE,2);
	
	hbox = gtk_hbox_new(TRUE,6);
	vbox = gtk_vbox_new(FALSE,2);
	gtk_box_pack_start(GTK_BOX(lib_load_vbox), library_manager_label, FALSE,FALSE,4); // Label: "Librarys files"
	gtk_box_pack_start(GTK_BOX(hbox),list_library_view, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox),add_button,FALSE,FALSE,1);
	gtk_box_pack_start(GTK_BOX(vbox),remove_button,FALSE,FALSE,1);
	gtk_box_pack_start(GTK_BOX(vbox),process_button,FALSE,FALSE,1);
	gtk_box_pack_start(GTK_BOX(hbox),vbox, FALSE, TRUE,1);
	gtk_box_pack_start(GTK_BOX(lib_load_vbox),hbox, FALSE, FALSE,5);
	
	lib_examples_vbox = gtk_vbox_new(FALSE,2);
	gtk_box_pack_start(GTK_BOX(lib_examples_vbox), library_examples_label, FALSE,TRUE,4); // Label: "Librarys files"
	gtk_box_pack_start(GTK_BOX(lib_examples_vbox),list_library_examples_view, TRUE, TRUE, 5);
	
	lib_examples_expander = gtk_expander_new ("");
    gtk_container_add (GTK_CONTAINER (lib_examples_expander), lib_examples_vbox);
		
	hunt_vbox = gtk_vbox_new(FALSE,2);
	
	hbox = gtk_hbox_new(FALSE,2);
	gtk_box_pack_start(GTK_BOX(hbox), hunt_label, FALSE,TRUE,4);
	gtk_box_pack_start(GTK_BOX(hunt_vbox),hbox, FALSE, FALSE,5);
	
	hbox = gtk_hbox_new(FALSE,2);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(hunt_entry),TRUE,TRUE,4);
	gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(set_button),FALSE,FALSE,1);
	gtk_box_pack_start(GTK_BOX(hunt_vbox),hbox, FALSE, FALSE,2);

	hbox = gtk_hbox_new(FALSE,2);
	gtk_box_pack_start(GTK_BOX(hbox),label_seq_intro,FALSE,FALSE,10);
	gtk_box_pack_start(GTK_BOX(hbox),GTK_WIDGET(hunt),TRUE,TRUE,40);
	gtk_box_pack_start(GTK_BOX(hunt_vbox),hbox, FALSE, TRUE,12);
	
	hbox = gtk_hbox_new(TRUE,2);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Dados carregados:"),FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hbox),load_data,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hunt_vbox),hbox, FALSE, FALSE,2);
	
	hbox = gtk_hbox_new(TRUE,2);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Integridade dos dados:"),FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hbox),data_integrity,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hunt_vbox),hbox, FALSE, FALSE,2);
	
	hbox = gtk_hbox_new(TRUE,2);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Sequência para busca:"),FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hbox),hunt_seq,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hunt_vbox),hbox, FALSE, FALSE,2);
	
	hbox = gtk_hbox_new(TRUE,2);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Suporte a CUDA:"),FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hbox),cuda_support,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hunt_vbox),hbox, FALSE, FALSE,2);
	
	hbox = gtk_hbox_new(TRUE,2);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_label_new("Status:"),FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hbox),status,FALSE,FALSE,5);
	gtk_box_pack_start(GTK_BOX(hunt_vbox),hbox, FALSE, FALSE,2);
	
	hbox = gtk_hbox_new(FALSE,1);
	gtk_box_pack_start(GTK_BOX(hbox),gtk_hbox_new(TRUE,2), FALSE, FALSE,150);
	gtk_box_pack_start(GTK_BOX(hbox),hunt_button,TRUE,TRUE,10);
	gtk_box_pack_start(GTK_BOX(hbox),spinner,FALSE,FALSE,2);
	gtk_box_pack_start(GTK_BOX(hunt_vbox),hbox, FALSE, FALSE,20);
	
	vbox = gtk_vbox_new(FALSE,2);
	gtk_box_pack_start(GTK_BOX(vbox),lib_load_vbox, TRUE, TRUE,5);
	gtk_box_pack_start(GTK_BOX(vbox),hunt_vbox, TRUE, FALSE,5);
	gtk_box_pack_start(GTK_BOX(hunt_hbox),vbox, TRUE, FALSE,5);	
	gtk_box_pack_start(GTK_BOX(hunt_hbox),lib_examples_expander, TRUE, FALSE,5);
	
	gtk_box_pack_start(GTK_BOX(general_vbox),hunt_hbox, TRUE, TRUE,5);
	
    gtk_box_pack_start (GTK_BOX(general_vbox), scrolled, TRUE, TRUE, 10);
 
	gtk_container_add(GTK_CONTAINER(window),general_vbox);
	gtk_rc_parse("gtkrc");
	gtk_widget_show_all(window);
	gtk_widget_hide(spinner);
	gtk_main();
	return 0;
}
