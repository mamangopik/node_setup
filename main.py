from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtCore import Qt,QThread,QObject
from pyqtgraph import PlotWidget
import pyqtgraph
import pyqtgraph.SignalProxy

import sys
import os
import time
import threading
from threading import Event
import serial
import serial.tools.list_ports
import json

import node_setup

serial_attributes = {
    'available_ports':[],
    'available_ports_displayed':0,
    'reader_serial_obj':None,
    'node_saved_info':{}
}

class app(node_setup.Ui_MainWindow):
    def __init__(self,attrributes):
        # app = app widget
        # window = window widget
        # ui = ui_instance()
        # ui.setupUi(window widget)
        # ui.retretranslateUira(window widget)
        # window widget.show()
        # sys.exit(app widget.exec())
        super(node_setup.Ui_MainWindow, self).__init__()
        app1 = QtWidgets.QApplication(sys.argv)
        window = QtWidgets.QMainWindow()
        window1Main = node_setup.Ui_MainWindow()
        window1Main.setupUi(window)
        app1.aboutToQuit.connect( self.exitHandler)
        self.window1Main = window1Main

        self.timer = pyqtgraph.QtCore.QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(500)

        self.timer2 = pyqtgraph.QtCore.QTimer()
        self.timer2.timeout.connect(self.scan_serial_port)
        self.timer2.start(500)

        self.serial_comm = attributes['serial_reader']
        self.event_stoper = attributes['stop_event']
        self.serial_listener = self.make_new_reader_thread()
        self.old_ports_available = []
        self.reader_lifetime = 10
        self.reader_start_time  = time.time()
        self.reader_is_run = False

        self.tb_broker = self.window1Main.tb_broker.text()
        self.tb_ssid = self.window1Main.tb_ssid.text()
        self.tb_password = self.window1Main.tb_password.text()
        self.tb_id = self.window1Main.tb_id.text()

        self.window1Main.btn_write.clicked.connect(self.write_data)
        self.window1Main.btn_read.clicked.connect(self.read_device_info)
        self.window1Main.btn_sync_rtc.clicked.connect(self.sello)
        self.window1Main.lb_port.itemDoubleClicked.connect(self.port_clicked)

        self.window1Main.retranslateUi(window)
        window.show()
        sys.exit(app1.exec())
    def make_new_reader_thread(self):
        return threading.Thread(target=read_serial_data, args=(self.event_stoper,))
    def write_data(self):
        self.tb_broker = self.window1Main.tb_broker.text()
        self.tb_ssid = self.window1Main.tb_ssid.text()
        self.tb_password = self.window1Main.tb_password.text()
        self.tb_id = self.window1Main.tb_id.text()

        print(self.tb_broker)
        print(self.tb_id)
        print(self.tb_ssid)
        print(self.tb_password)

        self.serial_comm.transmit(f">setssid:{self.tb_ssid}")
        time.sleep(0.5)
        self.serial_comm.transmit(f">setpwd:{self.tb_password}")
        time.sleep(0.5)
        self.serial_comm.transmit(f">settopic:{self.tb_id}")
        time.sleep(0.5)
        self.serial_comm.transmit(f">setbroker:{self.tb_broker}")
        time.sleep(0.5)
        self.serial_comm.transmit(">reboot:")
        time.sleep(0.5)

    def read_device_info(self):
        if(self.serial_comm.serial.is_open):
            self.serial_comm.transmit('>getVAR:')
            print(serial_attributes['node_saved_info'])
            try:
                self.window1Main.tb_broker.setText(serial_attributes['node_saved_info']['setup_var']['broker'])
                self.window1Main.tb_ssid.setText(serial_attributes['node_saved_info']['setup_var']['SSID'])
                self.window1Main.tb_password.setText(serial_attributes['node_saved_info']['setup_var']['password'])
                self.window1Main.tb_id.setText(serial_attributes['node_saved_info']['setup_var']['topic'])
                serial_attributes['node_saved_info'] = {}
            except:
                pass



    def sello(self):
        print('sello')
    def update(self):
        if(serial_attributes['available_ports_displayed']==1):
            if(self.old_ports_available!=serial_attributes['available_ports']):
                self.window1Main.lb_port.clear()
                self.window1Main.lb_port.addItems(serial_attributes['available_ports'])
                serial_attributes['available_ports_displayed']=0
                self.old_ports_available = serial_attributes['available_ports']
        else:
            print('no update needed')

    def port_clicked(self, clickedItem):
        print("port" + clickedItem.text() + "got clicked")
        self.serial_comm.config_serial(str(clickedItem.text()),115200)
        self.serial_comm.open()
        self.reader_start_time = time.time()

        if(not self.reader_is_run):
            try:
                self.serial_listener.start()
                self.reader_is_run = True
            except:
                self.serial_listener = self.make_new_reader_thread()
                self.serial_listener.start()
                self.reader_is_run = True
        
        serial_attributes['reader_serial_obj'] = self.serial_comm.serial

    def scan_serial_port(self):
        self.serial_comm.list_serial_ports()
    

    def exitHandler(self):
        self.serial_comm.serial.close()
        self.event_stoper.set()
        self.serial_listener.join()
        time.sleep(1)
        print('bye')





class serial_comm():
    def __init__(self):
        self.serial = serial.Serial()
        try:
            self.list_serial_ports()
        except Exception as e:
            print(e)
    def config_serial(self,
                    portname,
                    baudrate):

        self.serial.port = portname
        self.serial.baudrate = baudrate
    def open(self):
        if self.serial.is_open:
            self.serial.close()
        try:
            self.serial.open()
        except:
            try:
                self.serial.close()
                time.sleep(2)
                self.serial.open()
            except Exception as e:
                print(e)
    
    def transmit(self,data):
        data = data.encode()+b'\n'
        print("data to send over serial->",str(data))
        try:
            self.serial.write(data)
        except:
            try:
                self.serial.close()
                time.sleep(2)
                self.serial.open()
                time.sleep(1)
                self.serial.write(data)
            except Exception as e:
                print(e)
    
    def list_serial_ports(self):
        # Get a list of available serial ports
        serial_attributes['available_ports'] = []
        ports = serial.tools.list_ports.comports()
        if not ports:
            pass
        else:
            for port, desc, hwid in sorted(ports):
                # print(f"- {port}: {desc} ({hwid})")
                serial_attributes['available_ports'].append(port)
            serial_attributes['available_ports_displayed']=1
    def make_connection(self):
        try:
            pass
        except:
            pass


t_waiting  = time.time()
data_serial = ""
def read_serial_data(stop_event):
    global t_waiting
    while not stop_event.is_set():
        if time.time()-t_waiting > 0.002:
            try:
                serial_attributes['node_saved_info'] = json.loads(data_serial)
            except:
                pass
            data_serial = ""
        try:
            data = serial_attributes['reader_serial_obj'].readline().decode().strip()
            if data:
                t_waiting  = time.time()
                data_serial+=str(data)
                # print(data_serial)
        except Exception as e:
            print(e)
        time.sleep(1/1000)
    print('bye bye')

if __name__ == '__main__':
    stop_event = threading.Event()

    myserial = serial_comm()

    attributes = {
        'serial_reader':myserial,
        'stop_event':stop_event
    }

    app(attributes)

# Received data: SSID:SIMON BATAPA SHMS
# Received data: len:17