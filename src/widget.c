#include <cairo.h>
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#include <hildon/hildon.h>
#include <math.h>
#include <time.h>

#include "globals.h"
#include "widget.h"

#define GET_PRIVATE(o) G_TYPE_INSTANCE_GET_PRIVATE(o, TYPE_BINARYCLOCK_WIDGET, BinaryClockWidgetPrivate)

#define GCONF_ROOT "/apps/binaryclock"
#define GCONF_STYLE GCONF_ROOT "/style"
#define GCONF_SECONDS GCONF_ROOT "/seconds"

struct _BinaryClockWidgetPrivate {
  GtkWidget *area;
  cairo_t *cr;
  int style;
  gboolean seconds;
  struct tm t;
  struct tm old;
  gboolean has_old;
  GConfClient *gconf;
};

HD_DEFINE_PLUGIN_MODULE(BinaryClockWidget, binaryclock_widget, HD_TYPE_STATUS_MENU_ITEM)

static void draw_h(BinaryClockWidget *p, int x, int y, int r, int v, int n) {
  for (int i = 0; i < n; i++) {
    cairo_arc(p->priv->cr, x + (1.5 * r) + (3 * r * i), y + r, r, 0, 2 * M_PI);
    cairo_set_source_rgb(p->priv->cr, 1, 1, 1);
    if ((v >> (n - i - 1)) & 1)
      cairo_fill_preserve(p->priv->cr);
    cairo_stroke(p->priv->cr);
  }
}

static void draw_v(BinaryClockWidget *p, int x, int y, int r, int v, int n) {
  for (int i = 0; i < n; i++) {
    cairo_arc(p->priv->cr, x + r, y + (1.5 * r) + (3 * r * i), r, 0, 2 * M_PI);
    cairo_set_source_rgb(p->priv->cr, 1, 1, 1);
    if ((v >> (n - i - 1)) & 1)
      cairo_fill_preserve(p->priv->cr);
    cairo_stroke(p->priv->cr);
  }
}

static void draw(BinaryClockWidget *p) {
  if (p->priv->style == 1) {
    draw_v(p, 2, 18, 3, p->priv->t.tm_hour / 10, 2);
    draw_v(p, 12, 0, 3, p->priv->t.tm_hour % 10, 4);
    draw_v(p, 26, 9, 3, p->priv->t.tm_min / 10, 3);
    draw_v(p, 36, 0, 3, p->priv->t.tm_min % 10, 4);
    if (p->priv->seconds) {
      draw_v(p, 50, 9, 3, p->priv->t.tm_sec / 10, 3);
      draw_v(p, 60, 0, 3, p->priv->t.tm_sec % 10, 4);
    }
  } else {
    if (p->priv->seconds) {
      draw_h(p, 12, 2, 4, p->priv->t.tm_hour, 5);
      draw_h(p, 0, 14, 4, p->priv->t.tm_min, 6);
      draw_h(p, 0, 26, 4, p->priv->t.tm_sec, 6);
    } else {
      draw_h(p, 18, 2, 6, p->priv->t.tm_hour, 5);
      draw_h(p, 0, 20, 6, p->priv->t.tm_min, 6);
    }
  }
}

static gboolean expose_cb(GtkWidget *w, GdkEventExpose *e, gpointer d) {
  BinaryClockWidget *p = d;
  p->priv->cr = gdk_cairo_create(w->window);
  cairo_set_source_rgba(p->priv->cr, 1, 1, 1, 0);
  cairo_set_operator(p->priv->cr, CAIRO_OPERATOR_SOURCE);
  cairo_paint(p->priv->cr);
  draw(p);
  cairo_destroy(p->priv->cr);
  return FALSE;
}

static void set_size(BinaryClockWidget *p) {
  int w = 110, h = 36;
  if (p->priv->style == 1 && !p->priv->seconds) {
    w = 47;
    h = 36;
  } else if (p->priv->seconds) {
    w = 76;
    h = 36;
  }
  gtk_widget_set_size_request(p->priv->area, w, h);
}

static gboolean tick(gpointer d) {
  BinaryClockWidget *p = d;
  time_t tt = time(NULL);
  if (p->priv->has_old)
    p->priv->old = p->priv->t;
  localtime_r(&tt, &p->priv->t);
  if (p->priv->seconds || !p->priv->has_old || p->priv->old.tm_min != p->priv->t.tm_min)
    gtk_widget_queue_draw(p->priv->area);
  p->priv->has_old = TRUE;
  return TRUE;
}

static void settings_ok(GtkWidget *b, gpointer d) {
  BinaryClockWidget *p = d;
  GtkWidget **w = g_object_get_data(G_OBJECT(b), "w");
  p->priv->seconds = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w[0]));
  p->priv->style = hildon_picker_button_get_active(HILDON_PICKER_BUTTON(w[1]));
  gconf_client_set_bool(p->priv->gconf, GCONF_SECONDS, p->priv->seconds, NULL);
  gconf_client_set_int(p->priv->gconf, GCONF_STYLE, p->priv->style, NULL);
  set_size(p);
  gtk_widget_queue_draw(p->priv->area);
  gtk_widget_destroy(gtk_widget_get_toplevel(b));
}

static void settings(GtkWidget *b, gpointer d) {
  BinaryClockWidget *p = d;

  GtkWidget *dlg = gtk_dialog_new();
  gtk_window_set_title(GTK_WINDOW(dlg), "Binaryclock plugin settings");
  gtk_window_set_transient_for(GTK_WINDOW(dlg), GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(p))));
  gtk_container_set_border_width(GTK_CONTAINER(dlg), 5);

  GtkWidget *vbox = gtk_vbox_new(FALSE, 5);
  gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dlg)->vbox), vbox);

  GtkWidget *sec = gtk_check_button_new_with_label("Show seconds");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(sec), p->priv->seconds);

  GtkWidget *sel = hildon_touch_selector_new_text();
  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(sel), "Binary Clock");
  hildon_touch_selector_append_text(HILDON_TOUCH_SELECTOR(sel), "BCD Clock");

  GtkWidget *pick = hildon_picker_button_new(HILDON_SIZE_FINGER_HEIGHT, HILDON_BUTTON_ARRANGEMENT_VERTICAL);
  hildon_picker_button_set_selector(HILDON_PICKER_BUTTON(pick), HILDON_TOUCH_SELECTOR(sel));
  hildon_picker_button_set_active(HILDON_PICKER_BUTTON(pick), p->priv->style);

  GtkWidget *ok = hildon_button_new_with_text(HILDON_SIZE_FINGER_HEIGHT, HILDON_BUTTON_ARRANGEMENT_VERTICAL, "Ok", NULL);
  GtkWidget **w = g_new(GtkWidget *, 2);
  w[0] = sec;
  w[1] = pick;
  g_object_set_data_full(G_OBJECT(ok), "w", w, g_free);
  g_signal_connect(ok, "clicked", G_CALLBACK(settings_ok), p);

  gtk_box_pack_start(GTK_BOX(vbox), sec, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), pick, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dlg)->action_area), ok, FALSE, FALSE, 0);

  gtk_widget_show_all(dlg);
}

static void binaryclock_widget_init(BinaryClockWidget *p) {
  p->priv = GET_PRIVATE(p);
  p->priv->gconf = gconf_client_get_default();
  p->priv->style = gconf_client_get_int(p->priv->gconf, GCONF_STYLE, NULL);
  p->priv->seconds = gconf_client_get_bool(p->priv->gconf, GCONF_SECONDS, NULL);

  GtkWidget *btn = hildon_button_new_with_text(HILDON_SIZE_FINGER_HEIGHT, HILDON_BUTTON_ARRANGEMENT_VERTICAL, "Binary Clock", NULL);
  GtkWidget *img = gtk_image_new_from_file(PATH_RESOURCES "binaryclock_status_area_plugin.png");
  hildon_button_set_image(HILDON_BUTTON(btn), img);
  hildon_button_set_alignment(HILDON_BUTTON(btn), 0, 0.5, 0, 0);
  g_signal_connect(btn, "clicked", G_CALLBACK(settings), p);

  p->priv->area = gtk_drawing_area_new();
  g_signal_connect(p->priv->area, "expose-event", G_CALLBACK(expose_cb), p);

  set_size(p);

  // hd_status_plugin_item_set_status_area_widget needs a container
  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), p->priv->area, FALSE, FALSE, 0);
  gtk_widget_show_all(hbox);

  hd_status_plugin_item_set_status_area_widget(HD_STATUS_PLUGIN_ITEM(p), hbox);

  gtk_container_add(GTK_CONTAINER(p), btn);
  gtk_widget_show_all(GTK_WIDGET(p));

  g_timeout_add(1000, tick, p);
}

static void binaryclock_widget_class_init(BinaryClockWidgetClass *k) {
  g_type_class_add_private(k, sizeof(BinaryClockWidgetPrivate));
}

static void binaryclock_widget_class_finalize(BinaryClockWidgetClass *k) {}
