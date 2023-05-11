#!/usr/bin/env python3

from __future__ import print_function

usage = """Usage:
python3 imx-s400-mu-service.py &
python3 imx-s400-mu-client.py
python3 imx-s400-mu-client.py --exit-service
"""

# Copyright (C) 2004-2006 Red Hat Inc. <http://www.redhat.com/>
# Copyright (C) 2005-2007 Collabora Ltd. <http://www.collabora.co.uk/>
#
# SPDX-License-Identifier: MIT
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use, copy,
# modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.

import sys
from traceback import print_exc
from gi.repository import GLib

import dbus
import dbus.mainloop.glib

loop = None

def catchall_mu_signals_handler(hello_string):
    global loop
    print("Received a hello signal and it says ", hello_string)
    GLib.timeout_add(500, loop.quit)

def hello_signal_handler(hello_string):
    print ("Received signal (by connecting using remote object) and it says: "
           + hello_string)

def main():
    global loop
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    bus = dbus.SessionBus()

    try:
        remote_object = bus.get_object("org.qemu.client",
                                       "/org/qemu/client")

        # you can either specify the dbus_interface in each call...
        hello_reply_list = remote_object.HelloWorld("Hello from dbus_mu_client.py!",
            dbus_interface = "org.qemu.client")
        remote_object.connect_to_signal("MUSignal", hello_signal_handler, dbus_interface="org.qemu.client.mua", arg0="Hello")
    except dbus.DBusException:
        print_exc()
        print(usage)
        sys.exit(1)

    bus.add_signal_receiver(catchall_mu_signals_handler, dbus_interface = "org.qemu.client.mua", signal_name = "MUSignal")

    print("client:", hello_reply_list)

    if sys.argv[1:] == ['--exit-service']:
        iface = dbus.Interface(remote_object, "org.qemu.client")
        iface.Exit()
        return

    # ... or create an Interface wrapper for the remote object
    iface = dbus.Interface(remote_object, "org.qemu.client.mua")

    read_reply = iface.MUARead(0x0)

    print("client:", read_reply)

    write_reply = iface.MUAWrite(0x40, 1)

    print("client:", write_reply)

    signal_reply = iface.emitMUASignal()

    print("signal:", signal_reply)

    # D-Bus exceptions are mapped to Python exceptions
    try:
        iface.RaiseException()
    except dbus.DBusException as e:
        print("client:", str(e))

    # introspection is automatically supported
    print("client:", remote_object.Introspect(dbus_interface="org.freedesktop.DBus.Introspectable"))



    loop = GLib.MainLoop()
    loop.run()

if __name__ == '__main__':
    main()
