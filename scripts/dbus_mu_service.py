#!/usr/bin/env python3

from __future__ import print_function
import logging

from gi.repository import GLib

from enum import IntEnum

import dbus
import dbus.service
import dbus.mainloop.glib

usage = """Usage:
python3 dbus_mu_service.py &
python3 dbus_mu_client.py
python3 dbus_mu_client.py --exit-service
"""

# Copyright (C) 2004-2006 Red Hat Inc. <http://www.redhat.com/>
# Copyright (C) 2005-2007 Collabora Ltd. <http://www.collabora.co.uk/>
# Copyright (C) 2022 NXP
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


logging.basicConfig(level = logging.DEBUG)
logger = logging.getLogger()

class DemoException(dbus.DBusException):
    '''
        DemoException
    '''
    _dbus_error_name = 'org.qemu.client.mu.Exception'


class DBusMu(dbus.service.Object):
    '''
        DdbusMu class
    '''

    def __init__(self, session, path):
        '''
            init
        '''
        super(DBusMu, self).__init__(session, path)
        self.mua_data = {}
        self.mub_data = {}


    @dbus.service.method("org.qemu.client",
                         in_signature='s', out_signature='as')
    def HelloWorld(self, hello_message):
        '''
            HelloWorld
        '''
        print("service:", str(hello_message))
        return ["Hello", " from example-service.py", "with unique name",
                session_bus.get_unique_name()]

    @dbus.service.method("org.qemu.client.mua",
                         in_signature='', out_signature='')
    def RaiseException(self):
        raise DemoException('The RaiseException method does what you might '
                            'expect')

    @dbus.service.method("org.qemu.client.mua",
                         in_signature='t', out_signature='u')
    def MUARead(self, offset):
        ret = 0

        logger.debug(hex(ret))
        if hex(offset) in self.mub_data:
            ret = self.mub_data[hex(offset)]
        return ret

    @dbus.service.method("org.qemu.client.mua",
                         in_signature='tt', out_signature='i')
    def MUAWrite(self, offset, value):
        logger.debug(f"write {hex(offset)}: {hex(value)}")
        self.mua_data[hex(offset)] = value
        return 0

    @dbus.service.method("org.qemu.client",
                         in_signature='', out_signature='')
    def Exit(self):
        mainloop.quit()

    # used for irq
    @dbus.service.signal('org.qemu.client.mua')
    def MUASignal(self, message):
        logger.info(message)

    @dbus.service.method('org.qemu.client.mua')
    def emitMUASignal(self):
        #you emit signals by calling the signal's skeleton method
        self.MUBSignal('read')
        return 'Signal emitted'

    @dbus.service.method("org.qemu.client.mub",
                         in_signature='t', out_signature='u')
    def MUBRead(self, offset):
        ret = 0

        logger.debug(hex(ret))
        return ret

    @dbus.service.method("org.qemu.client.mub",
                         in_signature='tt', out_signature='i')
    def MUBWrite(self, offset, value):
        logger.debug(f"write {hex(offset)}: {hex(value)}")
        return 0


    # used for irq
    @dbus.service.signal('org.qemu.client.mub')
    def MUBSignal(self, message):
        logger.info(message)

    @dbus.service.method('org.qemu.client.mub')
    def emitMUBSignal(self):
        #you emit signals by calling the signal's skeleton method
        self.MUASignal('read')
        return 'Signal emitted'

if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    session_bus = dbus.SessionBus()
    name = dbus.service.BusName("org.qemu.client", session_bus)
    object = DBusMu(session_bus, '/org/qemu/client')

    mainloop = GLib.MainLoop()
    print("Running example service.")
    print(usage)
    mainloop.run()
