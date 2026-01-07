#ifndef __BINARYCLOCK_WIDGET_H__
#define __BINARYCLOCK_WIDGET_H__

#include <libhildondesktop/libhildondesktop.h>

G_BEGIN_DECLS

#define TYPE_BINARYCLOCK_WIDGET (binaryclock_widget_get_type())
#define BINARYCLOCK_WIDGET(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), TYPE_BINARYCLOCK_WIDGET, BinaryClockWidget))

typedef struct _BinaryClockWidget BinaryClockWidget;
typedef struct _BinaryClockWidgetClass BinaryClockWidgetClass;
typedef struct _BinaryClockWidgetPrivate BinaryClockWidgetPrivate;

struct _BinaryClockWidget {
  HDStatusMenuItem parent;
  BinaryClockWidgetPrivate *priv;
};

struct _BinaryClockWidgetClass {
  HDStatusMenuItemClass parent;
};

GType binaryclock_widget_get_type(void);

G_END_DECLS

#endif
