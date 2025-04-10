#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *button;
    GtkWidget *label;
} AppWidgets;

void on_button_clicked(GtkWidget *button, gpointer data) {
    AppWidgets *widgets = (AppWidgets *)data;
    
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Select Image",
                                                    GTK_WINDOW(widgets->window),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    
    GtkFileFilter *filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Image files");
    gtk_file_filter_add_pattern(filter, "*.png");
    gtk_file_filter_add_pattern(filter, "*.jpg");
    gtk_file_filter_add_pattern(filter, "*.jpeg");
    gtk_file_filter_add_pattern(filter, "*.bmp");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        // Create output SVG filename
        char *svg_filename = malloc(strlen(filename) + 5);
        strcpy(svg_filename, filename);
        char *ext = strrchr(svg_filename, '.');
        if (ext) *ext = '\0';
        strcat(svg_filename, ".svg");

        // Temporary BMP filename
        char *bmp_filename = malloc(strlen(filename) + 5);
        strcpy(bmp_filename, filename);
        ext = strrchr(bmp_filename, '.');
        if (ext) *ext = '\0';
        strcat(bmp_filename, "_temp.bmp");

        // Construct conversion commands
        char command[2048];
        snprintf(command, sizeof(command), 
                "convert \"%s\" \"%s\" && potrace -s \"%s\" -o \"%s\" && rm \"%s\"",
                filename, bmp_filename, bmp_filename, svg_filename, bmp_filename);

        // Execute commands
        int result = system(command);
        
        // Update label with result
        if (result == 0) {
            gtk_label_set_text(GTK_LABEL(widgets->label), "Conversion successful!");
        } else {
            gtk_label_set_text(GTK_LABEL(widgets->label), "Conversion failed!");
        }

        g_free(filename);
        free(svg_filename);
        free(bmp_filename);
    }

    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    AppWidgets *widgets = g_new(AppWidgets, 1);

    widgets->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(widgets->window), "Image to SVG Converter");
    gtk_window_set_default_size(GTK_WINDOW(widgets->window), 300, 200);
    g_signal_connect(widgets->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(widgets->window), vbox);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);

    widgets->label = gtk_label_new("Click the button to select an image");
    gtk_box_pack_start(GTK_BOX(vbox), widgets->label, TRUE, TRUE, 0);

    widgets->button = gtk_button_new_with_label("Select Image");
    gtk_box_pack_start(GTK_BOX(vbox), widgets->button, FALSE, FALSE, 0);
    g_signal_connect(widgets->button, "clicked", G_CALLBACK(on_button_clicked), widgets);

    gtk_widget_show_all(widgets->window);

    gtk_main();

    g_free(widgets);

    return 0;
}
