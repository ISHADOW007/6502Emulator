//
// Created by shobhit on 7/1/24.
//

#include "ui_render.hpp"
#include "instructionCycle.hpp"

#include <gtk-3.0/gtk/gtk.h>
#include <gtk-3.0/gtk/gtkwidget.h>
#include <gtk-3.0/gtk/gtkstyleprovider.h>
#include <gtk-3.0/gtk/gtkstylecontext.h>

#include <iostream>
#include <algorithm>

void onWindowDestroy(GtkWidget *widget, gpointer user_data) {
    window = NULL;
    gtk_widget_destroy(widget);
    gtk_main_quit();
    printf("closing\n");
}

void load_css(void) {
    GtkCssProvider *provider;
    GdkDisplay  *display;
    GdkScreen *screen;

    const gchar *css_style_file = "/home/shobhit/Desktop/6502Emulator/src/style.css";
    GFile *css_fp = g_file_new_for_path(css_style_file);
    std::cout << "GFile: " << css_fp << std::endl;
    GError  *error = NULL;

    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);

    gtk_css_provider_load_from_file(provider, css_fp, &error);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    if (error)
    {
        // Display a warning if the stylesheet is not loaded
        g_warning ("%s", error->message);
        std::cout << "Error: " << error->message << std::endl;
        // Free the memory allocated for the error
        // and acknowledge the error has been processed
        g_clear_error (&error);
    }
    //gtk_css_provider_load_from_path(provider, css_style_file, &error);
    g_object_unref(provider);
}

GtkTreeViewColumn* retTreeCol(char *ColumnHeading, GtkCellRenderer* cell, GtkWidget* treeview, int col_atr) {
    GtkTreeViewColumn* col = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title( col, ColumnHeading);
    gtk_tree_view_column_pack_start (col, cell, TRUE);
    gtk_tree_view_column_add_attribute (col, cell, "text", col_atr);
    gtk_tree_view_append_column (GTK_TREE_VIEW (treeview), col);
}

void addRow(gpointer user_data) {
    GtkTreeIter iter;
    gtk_list_store_append(colList, &iter);

    std::string PC_str = std::to_string(local_procRef->PC);
    std::string A_str = std::to_string(local_procRef->A);
    std::string X_str = std::to_string(local_procRef->X);
    std::string Y_str = std::to_string(local_procRef->Y);
    std::string SP_str = std::to_string(local_procRef->SP);
    std::string FR_str = std::to_string(local_procRef->FR);
    std::string FR_str_bin = decToBin(FR_str);

    gtk_list_store_set(colList, &iter,
                       0, PC_str.c_str(),
                       1, A_str.c_str(),
                       2, X_str.c_str(),
                       3, Y_str.c_str(),
                       4, SP_str.c_str(),
                       5, FR_str_bin.c_str(),
                       -1);
}

gpointer cont_rowUpdateThread(gpointer user_data) {
    g_idle_add (G_SOURCE_FUNC(GUI_Binding_contRun_InstrCycle), NULL);
    //g_usleep(10000);
}

gpointer step_rowUpdateThread(gpointer user_data) {
    g_idle_add (G_SOURCE_FUNC(GUI_Binding_stepRun_InstrCycle), NULL);
}

void onLoadButtonClick(GtkButton *button,GtkTextBuffer* txtBuff) {
    GtkTextIter s_iter, e_iter;
    gtk_text_buffer_get_start_iter(txtBuff, &s_iter);
    gtk_text_buffer_get_end_iter(txtBuff, &e_iter);
    char *str = gtk_text_buffer_get_text(txtBuff, &s_iter, &e_iter, FALSE);
    std::string OpcodeBuffer(str);
    cl.CodeLoadFromStrBuffer(OpcodeBuffer, 0x0400, local_memRef);
    //local_procRef->VMInit(local_memRef);
    //printf("%s\n", str);
    local_memRef->readMem(0x400);
    local_memRef->readMem(0x401);
    local_memRef->readMem(0x402);
    local_memRef->readMem(0x403);
    local_memRef->readMem(0x404);
    local_memRef->readMem(0x405);
}

void onContinuousRunButtonClick(GtkButton *button,GtkTextBuffer* txtBuff) {
    g_thread_new("RowUpdate1", cont_rowUpdateThread, NULL);
}

void onStepRunButtonClick(GtkButton *button,GtkTextBuffer* txtBuff) {
    //local_procRef->VMInit(local_memRef);
    g_thread_new("RowUpdate2", step_rowUpdateThread, NULL);
}

// clears textview, list store, and memory
void onClearButtonClick(GtkButton *button,GtkTextBuffer* txtBuff) {
    GtkTextIter s_iter, e_iter;
    gtk_text_buffer_get_start_iter(txtBuff, &s_iter);
    gtk_text_buffer_get_end_iter(txtBuff, &e_iter);
    gtk_text_buffer_delete(txtBuff, &s_iter, &e_iter);

    gtk_list_store_clear(colList);

    local_memRef->resetMemory();

    firstPress = 1;
}

void GUI_Binding_contRun_InstrCycle(gpointer user_data) {
    Processor *proc = local_procRef;
    Memory *mem = local_memRef;
    proc->VMInit(mem);
    uint8_t opcode;

    while( (opcode = IC.IFetch()) != 0x00 ) {
        IC.Execute(opcode);
        addRow(NULL);
    }
}

void GUI_Binding_stepRun_InstrCycle(gpointer user_data) {
    //static int firstPress = 1;
    Processor *proc = local_procRef;
    Memory *mem = local_memRef;
    if(firstPress == 1)
        proc->VMInit(mem);
    firstPress++;
    uint8_t opcode;
    if( (opcode = IC.IFetch()) != 0x00 ) {
        IC.Execute(opcode);
        addRow(NULL);
    }
}

std::string decToBin(std::string decNum) {
    std::string temp = "";
    temp.clear();
    int num = std::stoi(decNum, 0, 10);
    int rem = 0;
    int n = num;
    while(n > 0) {
        rem = n%2;
        n = n/2;
        if(rem == 0) temp += '0';
        else temp += '1';
        temp += ' ';
    }
    // pad string if the contents of flag register is less than 8
    while(temp.length() < 15) {
        temp.append("0 ");
    }
    reverse(temp.begin(), temp.end());
    temp[5] = '-';
    return temp;
}

void setupUI() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 600);
    gtk_window_set_title(GTK_WINDOW(window), "6502 Emulator");
    //gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    load_css();

    // to add multiple widgets to the window
    GtkWidget *fix = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fix);

    /**** ROWS AND COLUMNS TO OUTPUT VALUE OF REGISTERS ****/
    // GTK ListStore
    // used inside GTK TreeView
    colList = gtk_list_store_new(6, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                              G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    // GTK TreeView
    GtkWidget *treeview = gtk_tree_view_new();
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(colList));

    GtkCellRenderer *cellRenderer = gtk_cell_renderer_text_new();

    GtkTreeViewColumn* PC_col = retTreeCol("Program Counter", cellRenderer, treeview, 0);
    GtkTreeViewColumn* A_col = retTreeCol("A", cellRenderer, treeview, 1);
    GtkTreeViewColumn* X_col = retTreeCol("X", cellRenderer, treeview, 2);
    GtkTreeViewColumn* Y_col = retTreeCol("Y", cellRenderer, treeview, 3);
    GtkTreeViewColumn* SP_col = retTreeCol("SP", cellRenderer, treeview, 4);
    GtkTreeViewColumn* FR_col = retTreeCol("N V - B D I Z C", cellRenderer, treeview, 5);

    /**** FOR TEXT BUFFER AND TEXTVIEW ****/
    // GTK textview
    GtkWidget *textview = gtk_text_view_new();
    // GTK text buffer
    GtkTextBuffer *txtBuff = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview));

    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(textview), GTK_WRAP_CHAR);

    /*** GTK SCROLLED WINDOW FOR TEXT VIEW ****/
    GtkWidget *scr_window_textview = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scr_window_textview),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scr_window_textview), 300);
    gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scr_window_textview), 400);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scr_window_textview), GTK_SHADOW_IN);

    // adding textview widget to scrollable window
    gtk_container_add(GTK_CONTAINER(GTK_SCROLLED_WINDOW(scr_window_textview)), textview);

    /*** GTK SCROLLED WINDOW FOR TREE VIEW ****/
    GtkWidget *scr_window_treeview = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scr_window_treeview),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scr_window_treeview), 400);
    //gtk_scrolled_window_set_max_content_height(GTK_SCROLLED_WINDOW(scr_window_treeview), 600);
    gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(scr_window_treeview), 400);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scr_window_treeview), GTK_SHADOW_IN);

    // adding treeview widget to scrollable window
    gtk_container_add(GTK_CONTAINER(GTK_SCROLLED_WINDOW(scr_window_treeview)), treeview);

    //gtk_widget_class_set_css_name(GTK_WIDGET_GET_CLASS (window), "window");
    gtk_widget_set_name(window, "window");

    // button to load opcode into memory
    GtkWidget *loadButton = gtk_button_new_with_label("Load");
    gtk_widget_set_name(loadButton, "loadButton");

    // button to execute all instruction in one step
    GtkWidget *continuousRunButton = gtk_button_new_with_label("Continuous Run");
    gtk_widget_set_name(continuousRunButton, "continuousRunButton");

    // button to execute one instruction in one step
    GtkWidget *stepRunButton = gtk_button_new_with_label("Step Run");
    gtk_widget_set_name(stepRunButton, "stepRunButton");

    // button to clear textview, treeview, and memory
    GtkWidget *clearButton = gtk_button_new_with_label("Clear");
    gtk_widget_set_name(clearButton, "clearButton");


    // put widgets inside GtkFixed
    gtk_fixed_put(GTK_FIXED(fix), scr_window_treeview, 50, 60);
    gtk_fixed_put(GTK_FIXED(fix), scr_window_textview, 500, 60);
    gtk_fixed_put(GTK_FIXED(fix), loadButton, 500, 380);
    gtk_fixed_put(GTK_FIXED(fix), continuousRunButton, 570, 380);
    gtk_fixed_put(GTK_FIXED(fix), stepRunButton, 720, 380);
    gtk_fixed_put(GTK_FIXED(fix), clearButton, 820, 380);


    // for closing application
    g_signal_connect(window, "destroy", G_CALLBACK(onWindowDestroy), NULL);
    // on clicking Load button
    g_signal_connect(loadButton, "clicked", G_CALLBACK(onLoadButtonClick), txtBuff);
    // on clicking Continuous Run button
    g_signal_connect(continuousRunButton, "clicked", G_CALLBACK(onContinuousRunButtonClick), txtBuff);
    // on clicking Step Run button
    g_signal_connect(stepRunButton, "clicked", G_CALLBACK(onStepRunButtonClick), txtBuff);
    // on clicking Clear Run button
    g_signal_connect(clearButton, "clicked", G_CALLBACK(onClearButtonClick), txtBuff);
}

void mainUI(int *f_argc, char ***f_argv) {
    gtk_init(f_argc, f_argv);
    setupUI();
    gtk_widget_show_all(window);
    gtk_main();
}
