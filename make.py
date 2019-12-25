#!/usr/bin/python

import py2deb
import os

p = py2deb.Py2deb("binaryclock-plugin")
p.description = """This package replaces the standard clock in the status area with a binary clock.
Configuration options are available via the status area menu.
Possible options are different styles (binary or BCD) and the possibility to hide or show seconds."""
p.author = "Ruediger Gad"
p.mail = "r.c.g@gmx.de"
p.depends = "python2.5, python-gobject, python-hildon, python-hildondesktop "
p.section = "user/desktop"
p.icon="binaryclock_status_area_plugin.png"
p.arch = "all"
p.urgency = "low"
p.distribution = "fremantle"
p.repository = "extras-devel"
p.xsbc_bugtracker = "https://garage.maemo.org/tracker/?atid=6866&?group_id=1995"

p.postinstall = """#!/bin/sh

sed -i s/X-Status-Area-Permanent-Item=Clock/#X-Status-Area-Permanent-Item=Clock/g /etc/hildon-desktop/status-menu.plugins

echo "[binaryclock_status_area_plugin.desktop]
X-Desktop-File=/usr/share/applications/hildon-status-menu/binaryclock_status_area_plugin.desktop
X-Status-Menu-Position=22
X-Status-Area-Permanent-Item=Clock
" >> /etc/hildon-desktop/status-menu.plugins

pkill -f /usr/bin/hildon-status-menu"""

p.preremove = """#!/bin/sh

sed -i '/\[binaryclock_status_area_plugin.desktop\]/,/X-Status-Area-Permanent-Item=Clock\\n/ d' /etc/hildon-desktop/status-menu.plugins

sed -i s/#X-Status-Area-Permanent-Item=Clock/X-Status-Area-Permanent-Item=Clock/g /etc/hildon-desktop/status-menu.plugins

pkill -f /usr/bin/hildon-status-menu"""

dir_name = "src"
for root, dirs, files in os.walk(dir_name):
  real_dir = root[len(dir_name):]
  fake_file = []
  for f in files:
    fake_file.append(root + os.sep + f + "|" + f)
  if len(fake_file) > 0:
    p[real_dir] = fake_file

changelogInfo = """
 * Use cairo for painting.
 * Enable transparency.
 * Add new icons.
 * Add license info.
"""

version = "0.3.0"
build = "1"
print p
r = p.generate(version, build, changelog = changelogInfo, tar = True, dsc = True, changes = True, build = False, src = True)

