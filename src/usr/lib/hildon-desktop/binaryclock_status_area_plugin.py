#
# Copyright 2010 Ruediger Gad <r.c.g@gmx.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import cairo
from datetime import datetime
import gtk
import gobject
import hildon
import hildondesktop
import math

class BinaryClockStatusAreaPlugin(hildondesktop.StatusMenuItem):
    def __init__(self):
        hildondesktop.StatusMenuItem.__init__(self)

        self.clockStyle = 0
        self.showSeconds = False

        self.time = None
        self.oldTime = None

        button = hildon.Button(gtk.HILDON_SIZE_FINGER_HEIGHT, hildon.BUTTON_ARRANGEMENT_VERTICAL, "Binary Clock")
        image = gtk.Image()
        image.set_from_file("/usr/share/icons/hicolor/64x64/apps/binaryclock_status_area_plugin.png")
        button.set_image(image)
        button.set_alignment(0, 0.5, 0, 0)
        button.connect("clicked", self.settingsCb)
        self.add(button)
        button.show()

        self.drawingArea = gtk.DrawingArea()
      	self.drawingArea.connect("expose-event", self.exposeCb)

        self.set_status_area_widget(self.drawingArea)

        self.setSize()
      	self.drawingArea.show()

        self.show()

        gobject.timeout_add(1000, self.updateCb)

    def drawBcdClock(self):
        self.drawBitsVertically(2, 18, 3, self.time.hour / 10, 2)
        self.drawBitsVertically(12, 0, 3, self.time.hour % 10, 4)
        self.drawBitsVertically(26, 9, 3, self.time.minute / 10, 3)
        self.drawBitsVertically(36, 0, 3, self.time.minute % 10, 4)
        if self.showSeconds:
            self.drawBitsVertically(50, 9, 3, self.time.second / 10, 3)
            self.drawBitsVertically(60, 0, 3, self.time.second % 10, 4)

    def drawBinaryClock(self):
        if self.showSeconds:
            self.drawBitsHorizontally(12, 2, 4, self.time.hour, 5)
            self.drawBitsHorizontally(0, 14, 4, self.time.minute, 6)
            self.drawBitsHorizontally(0, 26, 4, self.time.second, 6)
        else :
            self.drawBitsHorizontally(18, 2, 6, self.time.hour, 5)
            self.drawBitsHorizontally(0, 20, 6, self.time.minute, 6)

    def drawBitsHorizontally(self, x, y, radius, value, n):
        for i in range(n):
            self.context.arc(x + (1.5 * radius) + (3 * radius * i), y + radius, radius, 0, 2 * math.pi)
            self.context.set_source_rgb(1, 1, 1)
            if (value >> (n - i - 1) & 1) > 0:
                self.context.fill_preserve()
            self.context.stroke()

    def drawBitsVertically(self, x, y, radius, value, n):
        for i in range(n):
            self.context.arc(x + radius, y + (1.5 * radius)  + (3 * radius * i), radius, 0, 2 * math.pi)
            self.context.set_source_rgb(1, 1, 1)
            if (value >> (n - i - 1) & 1) > 0:
                self.context.fill_preserve()
            self.context.stroke()

    def drawClock(self):
        if self.clockStyle == 0:
            self.drawBinaryClock()
        elif self.clockStyle == 1:
            self.drawBcdClock()
        else:
            self.drawBinaryClock()

    def exposeCb(self, widget, event):
        self.context = self.drawingArea.window.cairo_create()

        self.context.set_source_rgba(1.0, 1.0, 1.0, 0.0)
        self.context.set_operator(cairo.OPERATOR_SOURCE)
        self.context.paint()

        self.drawClock()
        return False

    def setSize(self):
        if (not self.showSeconds) and (self.clockStyle == 1):
            self.drawingArea.set_size_request(47, 36)
        elif (not self.showSeconds) and (self.clockStyle == 0):
            self.drawingArea.set_size_request(110, 36)
        else:
            self.drawingArea.set_size_request(76, 36)

    def settingsCb(self, widget, data=None):
        dialog = hildon.Dialog()
        dialog.set_title("Binary Clock Status Area Plugin Settings")

        self.showSecondsButton = hildon.CheckButton(gtk.HILDON_SIZE_FINGER_HEIGHT)
        self.showSecondsButton.set_active(self.showSeconds)
        self.showSecondsButton.set_label("Show seconds")
        dialog.vbox.pack_start(self.showSecondsButton, True, True, 0)
        self.showSecondsButton.show()

        self.selector = hildon.TouchSelector(text = True)
        self.selector.append_text("Binary Clock")
        self.selector.append_text("BCD Clock")
        self.styleButton = hildon.PickerButton(gtk.HILDON_SIZE_FINGER_HEIGHT, hildon.BUTTON_ARRANGEMENT_VERTICAL)
        self.styleButton.set_title("Clock Style")
        self.styleButton.set_selector(self.selector)
        self.styleButton.set_active(self.clockStyle)
        dialog.vbox.pack_start(self.styleButton, True, True, 0)
        self.styleButton.show()

        okButton = hildon.Button(gtk.HILDON_SIZE_FINGER_HEIGHT, hildon.BUTTON_ARRANGEMENT_VERTICAL, "Ok")
        okButton.connect("clicked", self.settingsOkCb, dialog)
        dialog.action_area.pack_start(okButton, True, True, 0)
        okButton.show()

        dialog.show_all()

    def settingsOkCb(self, widget, dialog):
        self.clockStyle = self.styleButton.get_active()
        self.showSeconds = self.showSecondsButton.get_active()

        self.setSize()
        self.drawingArea.queue_draw()
    
        dialog.destroy()

    def updateCb(self):
        if self.time != None:
            self.oldTime = self.time

        self.time = datetime.today()

        if self.showSeconds or (self.oldTime == None) or (self.oldTime.minute != self.time.minute):
            self.drawingArea.queue_draw()

        return True	


hd_plugin_type = BinaryClockStatusAreaPlugin


