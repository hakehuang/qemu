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


# pylint: disable=R0201,C0103,R1725,W1203

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
    VER = 0x0
    PAR = 0x4
    SR = 0x60
    CR = 0x64
    TRN = [0x20, 0x24, 0x28, 0x2C]
    RRN = [0x40, 0x44, 0x48, 0x4C]
    GIRN_OFFSET = 16
    GIRN_MASK = 0xF<<GIRN_OFFSET
    GIPN_OFFSET = 28
    GIPN_MASK = 0xF<<GIPN_OFFSET

    def __init__(self, session, path):
        '''
            init
        '''
        super(DBusMu, self).__init__(session, path)
        self.mua_data = {}
        self.mub_data = {}
        self.init_mua_regs()
        self.init_mub_regs()


    def init_mua_regs(self):
        '''
            init_mua registers
        '''
        self.mua_data[hex(self.VER)]  = 0x01000001
        self.mua_data[hex(self.SR)] = 0x00F00080
        self.mua_data[hex(self.CR)] = 0x0
        self.mua_data[hex(self.PAR)] = 0x0
        for _tr in self.TRN:
                self.mua_data[hex(_tr)] = 0x0
        for _rr in self.RRN:
                self.mua_data[hex(_rr)] = 0x0

    def init_mub_regs(self):
        '''
            init_mub registers
        '''
        self.mub_data[hex(self.VER)]  = 0x01000001
        self.mub_data[hex(self.SR)] = 0x00F00080
        self.mub_data[hex(self.CR)] = 0x0
        self.mub_data[hex(self.PAR)] = 0x0
        for _tr in self.TRN:
                self.mub_data[hex(_tr)] = 0x0
        for _rr in self.RRN:
                self.mub_data[hex(_rr)] = 0x0


    def mu_updates(self, _type = "read", offset = 0xFF):
        ''''
            update internal register and gnerate interrupts
            param:
                @type: string for read /write
                @offset: read / write offset
        '''
        if _type == "mua_read":
            if offset in self.RRN:
                # clear the sr Rx full of mua
                self.mua_data[hex(self.SR)] &= ~(1<<(24 + int((offset - self.RRN[0])/4)))
                # set the sr Tx empty of mub of mub
                self.mub_data[hex(self.SR)] |= 1<<(20 + int((offset - self.RRN[0])/4))
        elif _type == "mua_write":
            if offset in self.TRN:
                # write to Rxn of mub
                self.mub_data[hex(offset + self.TRN[0])] = self.mua_data[hex(offset)]
                #set Rn full of mub
                self.mub_data[hex(self.SR)] |= 1<<(24 + int((offset - self.TRN[0])/4))
                #clear the tx empty of mua
                self.mua_data[hex(self.SR)] &= ~(1<<(20 + int((offset - self.TRN[0])/4)))
            elif offset == self.CR:
                if self.mua_data[hex(self.CR)] & self.GIRN_MASK:
                    _v = self.mua_data[hex(self.CR)] & self.GIRN_MASK
                    _v = _v>>self.GIRN_OFFSET
                    self.mub_data[hex(self.CR)] |= _v<<self.GIPN_OFFSET
            elif offset == self.SR:
                _v = self.mua_data[hex(self.CR)] & self.GIPN_MASK
                _v = _v>>self.GIPN_OFFSET
                _v = ~_v
                if _v:
                    self.mub_data[hex(self.CR)] &= ~(_v<<self.GIRN_OFFSET)

        elif _type == "mub_read":
            if offset in self.RRN:
                # clear the sr Rx full of mub
                self.mub_data[hex(self.SR)] &= ~(1<<(24 + int((offset - self.RRN[0])/4)))
                # set the sr Tx empty of mua
                self.mua_data[hex(self.SR)] |= 1<<(20 + int((offset - self.RRN[0])/4))
        elif _type == "mub_write":
            if offset in self.TRN:
                # write to Rxn of mua
                self.mua_data[hex(offset + self.TRN[0])] = self.mub_data[hex(offset)]
                #set sr rx full of mua
                self.mua_data[hex(self.SR)] |= 1<<(24 + int((offset - self.TRN[0])/4))
                # clear sr tx empty of mub
                self.mub_data[hex(self.SR)] &= ~(1<<(20 + int((offset - self.TRN[0])/4)))
            elif offset == self.CR:
                if self.mub_data[hex(self.CR)] & self.GIRN_MASK:
                    _v = self.mub_data[hex(self.CR)] & self.GIRN_MASK
                    _v = _v>>self.GIRN_OFFSET
                    self.mua_data[hex(self.CR)] |= _v<<self.GIPN_OFFSET
            elif offset == self.SR:
                _v = self.mub_data[hex(self.CR)] & self.GIPN_MASK
                _v = _v>>self.GIPN_OFFSET
                _v = ~_v
                if _v:
                    self.mua_data[hex(self.CR)] &= ~(_v<<self.GIRN_OFFSET)
        else:
            pass

        if self.mua_data[hex(self.SR)] & self.mua_data[hex(self.CR)] != 0:
            self.MUASignal("mua interrupt")

        if self.mub_data[hex(self.SR)] & self.mub_data[hex(self.CR)] != 0:
            self.MUBSignal("mub interrupt")



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
                         in_signature='s', out_signature='as')
    def MUA_init(self, message):
        '''
            MUA init
        '''
        print("service: mua", str(message))
        self.mua_data.clear()
        self.init_mua_regs()
        return ["MUA Init", " from service.py", "with unique name",
                session_bus.get_unique_name()]

    @dbus.service.method("org.qemu.client.mua",
                         in_signature='', out_signature='')
    def RaiseException(self):
        '''
            exception call
        '''
        raise DemoException('The RaiseException method does what you might '
                            'expect')

    @dbus.service.method("org.qemu.client.mua",
                         in_signature='t', out_signature='u')
    def MUARead(self, offset):
        '''
            mua read
        '''
        ret = 0

        #logger.debug(hex(offset))
        if hex(offset) in self.mub_data:
            ret = self.mua_data[hex(offset)]

        self.mu_updates("mua_read", offset)
        return ret

    @dbus.service.method("org.qemu.client.mua",
                         in_signature='tt', out_signature='i')
    def MUAWrite(self, offset, value):
        '''
            write to MU A side
        '''
        logger.debug(f"write {hex(offset)}: {hex(value)}")
        data = self.mua_data[hex(offset)]
        if offset == 0x60: #SR
            # GIPn write 1 clear
            data &= ~(value&0xF0000000) #GIPn
            data &= ~(value&600) #RAIP/RDIP
            data |= value&0x0FFFFFFF
        elif offset == 0x64: #CR
            data = value
        elif offset in [0x20, 0x24, 0x28, 0x2C]:
            data = value
        else:
            return 0

        self.mua_data[hex(offset)] = data
        self.mu_updates("mua_write", offset)
        return 0

    @dbus.service.method("org.qemu.client",
                         in_signature='', out_signature='')
    def Exit(self):
        '''
            quit
        '''
        mainloop.quit()

    # used for irq
    @dbus.service.signal('org.qemu.client.mua')
    def MUASignal(self, message):
        '''
            MUA signal
        '''
        logger.info(message)

    @dbus.service.method('org.qemu.client.mua')
    def emitMUASignal(self):
        '''
            emit mua
        '''
        #you emit signals by calling the signal's skeleton method
        self.MUASignal('')
        return 'Signal emitted'

    # ========== mub operations ======================

    @dbus.service.method("org.qemu.client.mub",
                         in_signature='s', out_signature='as')
    def MUB_init(self, message):
        '''
            MUB init
        '''
        print("service: mub", str(message))
        self.mub_data.clear()
        self.init_mub_regs()
        return ["MUB Init", " from service.py", "with unique name",
                session_bus.get_unique_name()]


    @dbus.service.method("org.qemu.client.mub",
                         in_signature='t', out_signature='u')
    def MUBRead(self, offset):
        '''
            mub read
        '''
        ret = 0

        if hex(offset) in self.mub_data:
            ret = self.mub_data[hex(offset)]

        self.mu_updates("mub_read")
        return ret

    @dbus.service.method("org.qemu.client.mub",
                         in_signature='tt', out_signature='i')
    def MUBWrite(self, offset, value):
        '''
            mub write
        '''
        logger.debug(f"write {hex(offset)}: {hex(value)}")
        data = self.mua_data[hex(offset)]
        if offset == 0x60: #SR
            # GIPn write 1 clear
            data &= ~(value&0xF0000000) #GIPn
            data &= ~(value&600) #RAIP/RDIP
            data |= value&0x0FFFFFFF
        elif offset == 0x64: #CR
            data = value
        elif offset in [0x20, 0x24, 0x28, 0x2C]:
            data = value
        else:
            return 0

        self.mub_data[hex(offset)] = data
        self.mu_updates("mub_write")

        return 0


    # used for irq
    @dbus.service.signal('org.qemu.client.mub')
    def MUBSignal(self, message):
        '''
            mub signal
        '''
        logger.info(message)

    @dbus.service.method('org.qemu.client.mub')
    def emitMUBSignal(self):
        '''
            emit mub
        '''
        #you emit signals by calling the signal's skeleton method
        self.MUBSignal("")
        return 'Signal emitted'

if __name__ == '__main__':
    dbus.mainloop.glib.DBusGMainLoop(set_as_default=True)

    session_bus = dbus.SessionBus()
    name = dbus.service.BusName("org.qemu.client", session_bus)
    _object = DBusMu(session_bus, '/org/qemu/client')

    mainloop = GLib.MainLoop()
    print("Running example service.")
    print(usage)
    mainloop.run()
