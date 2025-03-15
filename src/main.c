#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *entry;
    GtkWidget *result_view;
    GtkWidget *combo_box;
    GtkWidget *history_list;
    GtkWidget *dark_mode_switch;
    GtkWidget *history_frame;
    GtkWidget *info_bar;
    GtkWidget *info_label;
    GtkCssProvider *provider;
    gboolean dark_mode;
} AppWidgets;

// Function prototypes
void decimalToBinary(const char *num, char *output);
void binaryToDecimal(const char *num, char *output);
void decimalToOctal(const char *num, char *output);
void octalToDecimal(const char *num, char *output);
void decimalToHexadecimal(const char *num, char *output);
void hexadecimalToDecimal(const char *num, char *output);
void add_to_history(AppWidgets *app, const char *input, const char *result, const char *operation);
gboolean validate_input(const char *input, int conversion_type);
void show_error_message(AppWidgets *app, const char *message);
void load_css(AppWidgets *app);
void toggle_dark_mode(GtkSwitch *widget, gboolean state, AppWidgets *app);
void clear_history(GtkButton *button, AppWidgets *app);
void copy_to_clipboard(GtkButton *button, AppWidgets *app);

void reverse_string(char *str);

void on_convert_clicked(GtkWidget *widget, AppWidgets *app) {
    const gchar *input = gtk_entry_get_text(GTK_ENTRY(app->entry));
    int choice = gtk_combo_box_get_active(GTK_COMBO_BOX(app->combo_box));
    char result[100] = "";
    char conversion_type[50] = "";

    if (strlen(input) == 0) {
        show_error_message(app, "Please enter a value!");
        return;
    }

    // Validate input based on conversion type
    if (!validate_input(input, choice)) {
        show_error_message(app, "Invalid input for the selected conversion type!");
        return;
    }

    switch (choice) {
        case 0: 
            decimalToBinary(input, result); 
            strcpy(conversion_type, "Decimal to Binary");
            break;
        case 1: 
            binaryToDecimal(input, result); 
            strcpy(conversion_type, "Binary to Decimal");
            break;
        case 2: 
            decimalToOctal(input, result); 
            strcpy(conversion_type, "Decimal to Octal");
            break;
        case 3: 
            octalToDecimal(input, result); 
            strcpy(conversion_type, "Octal to Decimal");
            break;
        case 4: 
            decimalToHexadecimal(input, result); 
            strcpy(conversion_type, "Decimal to Hexadecimal");
            break;
        case 5: 
            hexadecimalToDecimal(input, result); 
            strcpy(conversion_type, "Hexadecimal to Decimal");
            break;
        default: 
            strcpy(result, "Invalid selection!");
    }

    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->result_view)), result, -1);
    add_to_history(app, input, result, conversion_type);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    AppWidgets app;
    app.dark_mode = FALSE;
    
    // Create CSS provider
    app.provider = gtk_css_provider_new();
    
    // Create main window
    app.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(app.window), "Advanced Number System Converter");
    gtk_container_set_border_width(GTK_CONTAINER(app.window), 12);
    gtk_window_set_default_size(GTK_WINDOW(app.window), 600, 450);
    gtk_window_set_position(GTK_WINDOW(app.window), GTK_WIN_POS_CENTER);

    // Create outer box
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(app.window), main_box);
    
    // Create info bar for error messages
    app.info_bar = gtk_info_bar_new();
    gtk_info_bar_set_message_type(GTK_INFO_BAR(app.info_bar), GTK_MESSAGE_ERROR);
    app.info_label = gtk_label_new("");
    GtkWidget *content_area = gtk_info_bar_get_content_area(GTK_INFO_BAR(app.info_bar));
    gtk_container_add(GTK_CONTAINER(content_area), app.info_label);
    gtk_widget_set_no_show_all(app.info_bar, TRUE);
    gtk_box_pack_start(GTK_BOX(main_box), app.info_bar, FALSE, FALSE, 0);
    
    // Create header with title and dark mode switch
    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label), "<span size='x-large' font_weight='bold'>Number System Converter</span>");
    gtk_box_pack_start(GTK_BOX(header_box), title_label, FALSE, FALSE, 0);
    
    // Dark mode switch
    GtkWidget *dark_mode_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *dark_mode_label = gtk_label_new("Dark Mode");
    app.dark_mode_switch = gtk_switch_new();
    gtk_box_pack_start(GTK_BOX(dark_mode_box), dark_mode_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(dark_mode_box), app.dark_mode_switch, FALSE, FALSE, 0);
    g_signal_connect(app.dark_mode_switch, "state-set", G_CALLBACK(toggle_dark_mode), &app);
    gtk_box_pack_end(GTK_BOX(header_box), dark_mode_box, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(main_box), header_box, FALSE, FALSE, 0);
    
    // Create horizontal box for main content
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), content_box, TRUE, TRUE, 0);
    
    // Left panel - Conversion inputs
    GtkWidget *left_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_box_pack_start(GTK_BOX(content_box), left_panel, TRUE, TRUE, 0);
    
    // Frame for converter
    GtkWidget *converter_frame = gtk_frame_new("Converter");
    gtk_frame_set_shadow_type(GTK_FRAME(converter_frame), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(left_panel), converter_frame, FALSE, FALSE, 0);
    
    GtkWidget *converter_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(converter_box), 12);
    gtk_container_add(GTK_CONTAINER(converter_frame), converter_box);
    
    // Conversion type
    GtkWidget *combo_label = gtk_label_new("Conversion Type:");
    gtk_widget_set_halign(combo_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(converter_box), combo_label, FALSE, FALSE, 0);
    
    app.combo_box = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(app.combo_box), NULL, "Decimal to Binary");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(app.combo_box), NULL, "Binary to Decimal");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(app.combo_box), NULL, "Decimal to Octal");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(app.combo_box), NULL, "Octal to Decimal");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(app.combo_box), NULL, "Decimal to Hexadecimal");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(app.combo_box), NULL, "Hexadecimal to Decimal");
    gtk_combo_box_set_active(GTK_COMBO_BOX(app.combo_box), 0);
    gtk_box_pack_start(GTK_BOX(converter_box), app.combo_box, FALSE, FALSE, 0);
    
    // Input field
    GtkWidget *input_label = gtk_label_new("Enter Value:");
    gtk_widget_set_halign(input_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(converter_box), input_label, FALSE, FALSE, 0);
    
    app.entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(converter_box), app.entry, FALSE, FALSE, 0);
    
    // Convert button
    GtkWidget *convert_button = gtk_button_new_with_label("Convert");
    GtkWidget *convert_icon = gtk_image_new_from_icon_name("system-run", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(convert_button), convert_icon);
    g_signal_connect(convert_button, "clicked", G_CALLBACK(on_convert_clicked), &app);
    gtk_box_pack_start(GTK_BOX(converter_box), convert_button, FALSE, FALSE, 0);
    
    // Result view
    GtkWidget *result_frame = gtk_frame_new("Result");
    gtk_frame_set_shadow_type(GTK_FRAME(result_frame), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(left_panel), result_frame, TRUE, TRUE, 0);
    
    GtkWidget *result_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(result_box), 12);
    gtk_container_add(GTK_CONTAINER(result_frame), result_box);
    
    app.result_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app.result_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(app.result_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app.result_view), GTK_WRAP_WORD_CHAR);
    GtkWidget *result_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(result_scroll), 
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(result_scroll), app.result_view);
    gtk_box_pack_start(GTK_BOX(result_box), result_scroll, TRUE, TRUE, 0);
    
    // Copy button
    GtkWidget *copy_button = gtk_button_new_with_label("Copy to Clipboard");
    GtkWidget *copy_icon = gtk_image_new_from_icon_name("edit-copy", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(copy_button), copy_icon);
    g_signal_connect(copy_button, "clicked", G_CALLBACK(copy_to_clipboard), &app);
    gtk_box_pack_start(GTK_BOX(result_box), copy_button, FALSE, FALSE, 0);
    
    // Right panel - History
    app.history_frame = gtk_frame_new("Conversion History");
    gtk_frame_set_shadow_type(GTK_FRAME(app.history_frame), GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(content_box), app.history_frame, TRUE, TRUE, 0);
    
    GtkWidget *history_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(history_box), 12);
    gtk_container_add(GTK_CONTAINER(app.history_frame), history_box);
    
    GtkWidget *history_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(history_scroll), 
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(history_box), history_scroll, TRUE, TRUE, 0);
    
    // Create list for history
    GtkListStore *history_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, 
                                                    G_TYPE_STRING, G_TYPE_STRING);
    app.history_list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(history_store));
    g_object_unref(history_store);
    
    // Add columns
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Time", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app.history_list), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Operation", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app.history_list), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Input", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app.history_list), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Result", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(app.history_list), column);
    
    gtk_container_add(GTK_CONTAINER(history_scroll), app.history_list);
    
    // Clear history button
    GtkWidget *clear_button = gtk_button_new_with_label("Clear History");
    GtkWidget *clear_icon = gtk_image_new_from_icon_name("edit-clear", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(clear_button), clear_icon);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(clear_history), &app);
    gtk_box_pack_start(GTK_BOX(history_box), clear_button, FALSE, FALSE, 0);
    
    // Connect signals
    g_signal_connect(app.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Load CSS
    load_css(&app);
    
    // Show all widgets
    gtk_widget_show_all(app.window);
    gtk_widget_hide(app.info_bar);
    
    gtk_main();
    return 0;
}



void reverse_string(char *str) {
    if (!str) return;
    int i = 0;
    int j = strlen(str) - 1;
    char temp;
    
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

void decimalToBinary(const char *num, char *output) {
    long int decimal = atol(num);
    if (decimal == 0) {
        strcpy(output, "0");
        return;
    }
    
    int i = 0;
    while (decimal > 0) {
        output[i++] = (decimal % 2) + '0';
        decimal /= 2;
    }
    output[i] = '\0';
    strrev(output);
}

void binaryToDecimal(const char *num, char *output) {
    long int decimal = 0;
    int i = 0;
    int len = strlen(num);
    
    for (i = 0; i < len; i++) {
        if (num[i] == '1') {
            decimal += 1 << (len - i - 1);
        }
    }
    
    sprintf(output, "%ld", decimal);
}

void decimalToOctal(const char *num, char *output) {
    long int decimal = atol(num);
    if (decimal == 0) {
        strcpy(output, "0");
        return;
    }
    
    int i = 0;
    while (decimal > 0) {
        output[i++] = (decimal % 8) + '0';
        decimal /= 8;
    }
    output[i] = '\0';
    strrev(output);
}

void octalToDecimal(const char *num, char *output) {
    long int decimal = 0;
    int i = 0;
    int len = strlen(num);
    
    for (i = 0; i < len; i++) {
        decimal = decimal * 8 + (num[i] - '0');
    }
    
    sprintf(output, "%ld", decimal);
}

void decimalToHexadecimal(const char *num, char *output) {
    long int decimal = atol(num);
    if (decimal == 0) {
        strcpy(output, "0");
        return;
    }
    
    int i = 0;
    int remainder;
    while (decimal > 0) {
        remainder = decimal % 16;
        if (remainder < 10) {
            output[i++] = remainder + '0';
        } else {
            output[i++] = remainder - 10 + 'A';
        }
        decimal /= 16;
    }
    output[i] = '\0';
    strrev(output);
}

void hexadecimalToDecimal(const char *num, char *output) {
    long int decimal = 0;
    int i = 0;
    int len = strlen(num);
    char c;
    
    for (i = 0; i < len; i++) {
        c = toupper(num[i]);
        if (c >= '0' && c <= '9') {
            decimal = decimal * 16 + (c - '0');
        } else if (c >= 'A' && c <= 'F') {
            decimal = decimal * 16 + (c - 'A' + 10);
        }
    }
    
    sprintf(output, "%ld", decimal);
}

void add_to_history(AppWidgets *app, const char *input, const char *result, const char *operation) {
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(app->history_list)));
    GtkTreeIter iter;
    
    // Get current time
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", t);
    
    // Add to history
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 
                      0, time_str,
                      1, operation,
                      2, input,
                      3, result,
                      -1);
}

gboolean validate_input(const char *input, int conversion_type) {
    int i;
    int len = strlen(input);
    
    switch (conversion_type) {
        case 0: // Decimal to Binary
        case 2: // Decimal to Octal
        case 4: // Decimal to Hexadecimal
            // Validate decimal
            for (i = 0; i < len; i++) {
                if (!isdigit(input[i])) {
                    return FALSE;
                }
            }
            break;
            
        case 1: // Binary to Decimal
            // Validate binary
            for (i = 0; i < len; i++) {
                if (input[i] != '0' && input[i] != '1') {
                    return FALSE;
                }
            }
            break;
            
        case 3: // Octal to Decimal
            // Validate octal
            for (i = 0; i < len; i++) {
                if (input[i] < '0' || input[i] > '7') {
                    return FALSE;
                }
            }
            break;
            
        case 5: // Hexadecimal to Decimal
            // Validate hexadecimal
            for (i = 0; i < len; i++) {
                if (!isxdigit(input[i])) {
                    return FALSE;
                }
            }
            break;
            
        default:
            return FALSE;
    }
    
    return TRUE;
}

void show_error_message(AppWidgets *app, const char *message) {
    gtk_label_set_text(GTK_LABEL(app->info_label), message);
    gtk_widget_show(app->info_bar);
    
    // Hide the message after 3 seconds
    g_timeout_add_seconds(3, (GSourceFunc)gtk_widget_hide, app->info_bar);
}

void load_css(AppWidgets *app) {
    const char *css_data = 
        "window { background-color: #f0f0f0; }"
        "button { border-radius: 4px; padding: 5px; }"
        "button:hover { background-color: #e0e0e0; }"
        "frame { border-radius: 4px; }"
        ".dark { background-color: #333; color: #fff; }"
        ".dark button { background-color: #555; color: #fff; border-color: #666; }"
        ".dark button:hover { background-color: #666; }"
        ".dark entry, .dark textview { background-color: #444; color: #fff; border-color: #666; }";
    
    gtk_css_provider_load_from_data(app->provider, css_data, -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), 
                                              GTK_STYLE_PROVIDER(app->provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

void toggle_dark_mode(GtkSwitch *widget, gboolean state, AppWidgets *app) {
    app->dark_mode = state;
    
    if (state) {
        gtk_style_context_add_class(gtk_widget_get_style_context(app->window), "dark");
    } else {
        gtk_style_context_remove_class(gtk_widget_get_style_context(app->window), "dark");
    }
}

void clear_history(GtkButton *button, AppWidgets *app) {
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(app->history_list)));
    gtk_list_store_clear(store);
}

void copy_to_clipboard(GtkButton *button, AppWidgets *app) {
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->result_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gchar *text = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
    
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text(clipboard, text, -1);
    
    g_free(text);
    
    // Show a temporary message
    show_error_message(app, "Result copied to clipboard!");
}