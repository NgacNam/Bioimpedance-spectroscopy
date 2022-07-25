from tokenize import Double
from PyQt5 import QtCore, QtWidgets, QtSerialPort
from PyQt5.QtWidgets import QMainWindow, QApplication, QWidget, QPushButton, QAction, QLineEdit, QMessageBox, QLabel, QSizePolicy, QTextEdit
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import pyqtSlot
import sys
import matplotlib.pyplot as plt
import numpy as np
import math
import serial
import csv
import pandas as pd
from sympy import true

sys_init = 'zzzzzzzzzzzzzzzzzzzzzzzzzzzzzz'
csvname = 'result.csv'

inc_step = 51
min_freq = 5000
max_freq = 100000 + (100000-min_freq)/inc_step
Rcalibrate = 10000
freq_count = 0
cal_check = False
Magnitude = 0
freqrange = np.arange(min_freq, max_freq, (max_freq - min_freq)/inc_step)
xs = np.array([])
Impedance_str = np.array([])
Rgain = np.array([])
sys_phase = np.array([])
Phase_str = np.array([])
Igain = np.array([])
gainfactor = np.array([])
xs = np.append(xs, freqrange)


class App(QMainWindow):
    def __init__(self):
        super().__init__()
        self.title = 'Do tro khang'
        self.left = 0
        self.top = 50
        self.width = 800
        self.height = 700
        self.UIcompnents()
        self.serial = QtSerialPort.QSerialPort(
            'COM6',
            baudRate=QtSerialPort.QSerialPort.Baud9600,
            readyRead=self.receive
        )
        self.show()

    def UIcompnents(self):
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)

        # Connect button
        '''self.connect_button = QPushButton(self)
        self.connect_button.setText("Connect")
        self.connect_button.move(0, 80)
        self.connect_button.clicked.connect(self.on_toggled)'''

        # Create textbox
        self.Rcal_textbox = QLineEdit(self)
        self.Rcal_textbox.move(100, 110)
        self.Rcal_textbox.resize(100, 30)

        # Create input Rcalibrate
        self.Rcal_button = QPushButton('Input Rcal', self)
        self.Rcal_button.move(0, 110)
        self.Rcal_button.clicked.connect(self.input_Rcal)


        # Measure button
        self.measure_button = QPushButton(self)
        self.measure_button.setText("Measure")
        self.measure_button.move(0, 150)
        self.measure_button.clicked.connect(self.measure)

        # Calibrate
        self.calibrate_button = QPushButton(self)
        self.calibrate_button.setText("Calibrate")
        self.calibrate_button.move(0, 190)
        self.calibrate_button.clicked.connect(self.calibrate)

        # Create CSV textbox
        self.csv_textbox = QLineEdit(self)
        self.csv_textbox.move(100, 230)
        self.csv_textbox.resize(100, 30)

        # CSV button
        self.csv_button = QPushButton(self)
        self.csv_button.setText("Save csv file")
        self.csv_button.move(0, 230)
        self.csv_button.clicked.connect(self.makecsv)

        # Plot button
        self.plot_button = QPushButton(self)
        self.plot_button.setText("Plot")
        self.plot_button.move(0, 270)
        self.plot_button.clicked.connect(self.plot)

        '''# Send textbox
        self.message_send = QLineEdit(self)
        self.message_send.move(250, 110)
        self.message_send.resize(300, 30)

        # Send button
        self.send_btn = QPushButton(self)
        self.send_btn.setText("Send")
        self.send_btn.move(560, 110)
        self.plot_button.clicked.connect(self.send)'''
        
        # UART data
        self.uart_rx = QTextEdit(self)
        self.uart_rx.move(250, 150)
        self.uart_rx.resize(500, 500)
        
    
    @QtCore.pyqtSlot()
    def receive(self):
        while self.serial.canReadLine():
            text = self.serial.readLine().data().decode()
            text = text.rstrip('\r\n')
            self.uart_rx.append(text)

    @QtCore.pyqtSlot()
    def send(self):
        self.serial.write(self.message_send.text().encode())

    @QtCore.pyqtSlot(bool)
    def on_toggled(self, checked):
        self.connect_button.setText("Disconnect" if checked else "Connect")
        if checked:
            if not self.serial.isOpen():
                if not self.serial.open(QtCore.QIODevice.ReadWrite):
                    self.connect_button.setChecked(False)
        else:
            self.serial.close()

    def measure(self):
        global Impedance_str
        global Phase_str
        global Rgain
        global Igain
        global Magnitude
        global cal_check
        global freq_count
        global sys_phase
        global Rcalibrate
        global gainfactor
        Impedance_str = np.array([])
        Phase_str = np.array([])
        freq_count = 0
        #global P_sys, P

        s = serial.Serial('COM6', 9600)
        s.write(sys_init.encode())

        while True:
            data = s.readline()
            self.uart_rx.append(data.decode())
            data_list = data.split(b',')
            real = np.double(data_list[0])

            data1 = data_list[1]
            data1_list = data1.split(b'\n')
            im = np.double(data1_list[0])

            Magnitude = math.sqrt(np.double(real)**2 + np.double(im)**2)

            if cal_check == False:
                Rgain = np.append(Rgain, real)
                Igain = np.append(Igain, im)
                
                gainfactor = np.append(gainfactor, (10**12) * ((1/np.double(Rcalibrate)) / math.sqrt(np.double(Rgain[freq_count])**2 + np.double(Igain[freq_count])**2)))
                if (real > 0 and im > 0):
                    P_sys = math.atan(np.double(Igain[freq_count]) / np.double(Rgain[freq_count])) * 57.2957795
                if (real > 0 and im < 0):
                    P_sys = 360 + math.atan(np.double(Igain[freq_count]) / np.double(Rgain[freq_count])) * 57.2957795
                if ((real < 0 and im > 0) or (real < 0 and im < 0)):
                    P_sys = 180 + math.atan(np.double(Igain[freq_count]) / np.double(Rgain[freq_count])) * 57.2957795
                sys_phase = np.append(sys_phase, P_sys)
            
            
            if cal_check == True:
                Impedance_str = np.append(Impedance_str, (10**12) / (np.double(gainfactor[freq_count]) * np.double(Magnitude)))
                if (real > 0 and im > 0):
                    P = math.atan(np.double(im)/np.double(real)) * 57.2957795 - np.double(sys_phase[freq_count])
                if (real > 0 and im < 0):
                    P = 360 + math.atan(np.double(im)/np.double(real)) * 57.2957795 - np.double(sys_phase[freq_count])
                if ((real < 0 and im > 0) or (real < 0 and im < 0)):
                    P =  180 + math.atan(np.double(im)/np.double(real)) * 57.2957795 - np.double(sys_phase[freq_count])
                Phase_str = np.append(Phase_str, P)
            freq_count += 1
            #print('f=', freq_count)
            #print('rg=', Rgain)
            #print('ig=', Igain)
            print(Impedance_str)
            print(Phase_str)

            if freq_count >= inc_step:
                freq_count = 0
                break
        cal_check = True

    def calibrate(self):
        global cal_check, Rcalibrate, Rgain, Igain, gainfactor, sys_phase, Impedance_str, Phase_str
        Impedance_str = np.array([])
        Rgain = np.array([])
        sys_phase = np.array([])
        Phase_str = np.array([])
        Igain = np.array([])
        gainfactor = np.array([])
        cal_check = False
        self.measure()

    def makecsv(self):
        global Impedance_str
        global Phase_str
        global freqrange
        global csvname

        textboxValue = self.csv_textbox.text()
        QMessageBox.question(self, 'CSV name', "Save CSV as " +
                             textboxValue, QMessageBox.Ok)
        self.csv_textbox.setText("")
        csvname = textboxValue

        csvdata = pd.DataFrame({'Frequency':freqrange,
                                'Impedance':Impedance_str,
                                'Phase':Phase_str})
        csvdata.to_csv(''+ csvname +'.csv')


    def plot(self):

        plt.ion()
        plt.show()

        plt.close('all')
        fig, Impedance_plot = plt.subplots()
        Phase_plot = Impedance_plot.twinx()

        Impedance_plot.set_xlim(0, 100000)
        Impedance_plot.set_ylim(0, np.max(Impedance_str)*2)
        Phase_plot.set_ylim(-90, 90)

        Impedance_plot.set_title('AD5933')
        Impedance_plot.set_xlabel('Frequency')
        Impedance_plot.set_ylabel('Impedance')
        Phase_plot.set_ylabel('im')

        line1, = Impedance_plot.plot(xs, Impedance_str, 'b', label='Impedance')
        line2, = Phase_plot.plot(xs, Phase_str, 'r', label='Phase')
        
        Impedance_plot.legend(handles=[line1, line2])

    # @pyqtSlot()
    def input_Rcal(self):
        global Rcalibrate
        textboxValue = self.Rcal_textbox.text()
        QMessageBox.question(self, 'Rcal', "Rcal =  " +
                             textboxValue, QMessageBox.Ok)
        self.Rcal_textbox.setText("")
        Rcalibrate = textboxValue

if __name__ == "__main__":
    app = QApplication(sys.argv)
    ex = App()
    sys.exit(app.exec_())
