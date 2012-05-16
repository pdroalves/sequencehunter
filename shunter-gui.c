#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAJORV 0
#define MINORV 0

/* converts integer into string */
char* itoa(unsigned long num) {
        char* retstr = calloc(12, sizeof(char));
        if (sprintf(retstr, "%ld", num) > 0) {
                return retstr;
        } else {
                return NULL;
        }
}

void show_about(GtkWidget *widget, gpointer data)
{

  char ver[10];

  strcpy(ver,itoa(MAJORV));
  strcat(ver,".");
  strcat(ver,itoa(MINORV));

  GtkWidget *dialog = gtk_about_dialog_new();
  gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "Gerador de Sequências");
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
	GtkWidget *new;
	GtkWidget *load;
	GtkWidget *save;
	GtkWidget *exit;
	GtkWidget *hbox,*vbox;
	GtkWidget *hunt;
	GtkAccelGroup *group;
	char *title;
	
	gtk_init(&argc,&argv);

	title = (char*)malloc(50*sizeof(char));
	strcpy(title,"Sequence Hunter");
	hunt = gtk_button_new_with_mnemonic("_Hunt");
	hbox = gtk_hbox_new(FALSE,2);
	vbox = gtk_vbox_new(FALSE,2);
	

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),title);
	gtk_window_set_resizable(GTK_WINDOW(window),TRUE);
	gtk_window_set_default_size(GTK_WINDOW(window),300,600);
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);

	group = gtk_accel_group_new();
	menubar = gtk_menu_bar_new();
	file = gtk_menu_item_new_with_label("Arquivo");

	filemenu = gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM (file), filemenu);
	gtk_menu_shell_append (GTK_MENU_SHELL (menubar), file);

	//File menu
	new = gtk_image_menu_item_new_from_stock (GTK_STOCK_NEW,group);
	load = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN,group);
	save = gtk_image_menu_item_new_from_stock (GTK_STOCK_SAVE,group);
	about = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT,group);
	exit = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT,group);
	gtk_menu_shell_append (GTK_MENU_SHELL (filemenu), new);
	gtk_menu_shell_append (GTK_MENU_SHELL (filemenu),load);
	gtk_menu_shell_append (GTK_MENU_SHELL (filemenu),save);
	gtk_menu_shell_append (GTK_MENU_SHELL (filemenu),about);
	gtk_menu_shell_append (GTK_MENU_SHELL (filemenu),exit);

	gtk_box_pack_start(GTK_BOX(vbox), menubar,FALSE,FALSE,1);
	gtk_window_add_accel_group (GTK_WINDOW (window),group);

	g_signal_connect(window,"delete-event",G_CALLBACK(gtk_main_quit),NULL);
	//g_signal_connect(hunt,"clicked",G_CALLBACK(start_hunt),NULL);
	g_signal_connect(GTK_MENU(exit),"activate",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(GTK_MENU(about),"activate",G_CALLBACK(show_about),NULL);
	
	//Desenha a tabela
	/*gtk_box_pack_start(GTK_BOX(vbox), list_view, TRUE, TRUE, 4);
	gtk_box_pack_start(GTK_BOX(hbox), novo, TRUE, TRUE, 2);
	//gtk_box_pack_start(GTK_BOX(hbox), editar,TRUE,TRUE,2);
	gtk_box_pack_start(GTK_BOX(hbox), remove, TRUE, TRUE, 2);*/
	gtk_box_pack_start(GTK_BOX(vbox),hbox, FALSE, FALSE,2);

	/*hbox = gtk_hbox_new(FALSE,2);
	gtk_box_pack_start(GTK_BOX(hbox), gerar, TRUE, TRUE, 2);
	gtk_box_pack_start(GTK_BOX(vbox), hbox,FALSE,FALSE,2);*/

	gtk_container_add(GTK_CONTAINER(window),vbox);
	gtk_widget_show_all(window);
	gtk_main();
	return 0;
}
