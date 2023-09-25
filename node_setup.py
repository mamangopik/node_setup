# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'node_setup.ui'
#
# Created by: PyQt5 UI code generator 5.14.1
#
# WARNING! All changes made in this file will be lost!


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(497, 387)
        self.centralwidget = QtWidgets.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.btn_write = QtWidgets.QPushButton(self.centralwidget)
        self.btn_write.setGeometry(QtCore.QRect(276, 265, 161, 31))
        self.btn_write.setObjectName("btn_write")
        self.label_2 = QtWidgets.QLabel(self.centralwidget)
        self.label_2.setGeometry(QtCore.QRect(73, 10, 67, 17))
        self.label_2.setObjectName("label_2")
        self.tb_log = QtWidgets.QTextEdit(self.centralwidget)
        self.tb_log.setGeometry(QtCore.QRect(10, 90, 211, 160))
        self.tb_log.setObjectName("tb_log")
        self.label_7 = QtWidgets.QLabel(self.centralwidget)
        self.label_7.setGeometry(QtCore.QRect(74, 65, 67, 17))
        self.label_7.setObjectName("label_7")
        self.btn_read = QtWidgets.QPushButton(self.centralwidget)
        self.btn_read.setGeometry(QtCore.QRect(27, 265, 170, 70))
        self.btn_read.setObjectName("btn_read")
        self.layoutWidget = QtWidgets.QWidget(self.centralwidget)
        self.layoutWidget.setGeometry(QtCore.QRect(230, 10, 251, 241))
        self.layoutWidget.setObjectName("layoutWidget")
        self.gridLayout = QtWidgets.QGridLayout(self.layoutWidget)
        self.gridLayout.setContentsMargins(0, 0, 0, 0)
        self.gridLayout.setObjectName("gridLayout")
        self.label_3 = QtWidgets.QLabel(self.layoutWidget)
        self.label_3.setObjectName("label_3")
        self.gridLayout.addWidget(self.label_3, 0, 0, 1, 1)
        self.tb_broker = QtWidgets.QLineEdit(self.layoutWidget)
        self.tb_broker.setObjectName("tb_broker")
        self.gridLayout.addWidget(self.tb_broker, 3, 0, 1, 1)
        self.tb_ssid = QtWidgets.QLineEdit(self.layoutWidget)
        self.tb_ssid.setObjectName("tb_ssid")
        self.gridLayout.addWidget(self.tb_ssid, 5, 0, 1, 1)
        self.tb_id = QtWidgets.QLineEdit(self.layoutWidget)
        self.tb_id.setObjectName("tb_id")
        self.gridLayout.addWidget(self.tb_id, 1, 0, 1, 1)
        self.tb_password = QtWidgets.QLineEdit(self.layoutWidget)
        self.tb_password.setObjectName("tb_password")
        self.gridLayout.addWidget(self.tb_password, 7, 0, 1, 1)
        self.label_4 = QtWidgets.QLabel(self.layoutWidget)
        self.label_4.setObjectName("label_4")
        self.gridLayout.addWidget(self.label_4, 2, 0, 1, 1)
        self.label_5 = QtWidgets.QLabel(self.layoutWidget)
        self.label_5.setObjectName("label_5")
        self.gridLayout.addWidget(self.label_5, 4, 0, 1, 1)
        self.label_6 = QtWidgets.QLabel(self.layoutWidget)
        self.label_6.setObjectName("label_6")
        self.gridLayout.addWidget(self.label_6, 6, 0, 1, 1)
        self.lb_port = QtWidgets.QListWidget(self.centralwidget)
        self.lb_port.setGeometry(QtCore.QRect(10, 30, 211, 31))
        self.lb_port.setObjectName("lb_port")
        self.btn_sync_rtc = QtWidgets.QPushButton(self.centralwidget)
        self.btn_sync_rtc.setGeometry(QtCore.QRect(277, 304, 161, 31))
        self.btn_sync_rtc.setObjectName("btn_sync_rtc")
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 497, 22))
        self.menubar.setObjectName("menubar")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(MainWindow)
        self.statusbar.setObjectName("statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        _translate = QtCore.QCoreApplication.translate
        MainWindow.setWindowTitle(_translate("MainWindow", "VIBRASi"))
        self.btn_write.setText(_translate("MainWindow", "WRITE"))
        self.label_2.setText(_translate("MainWindow", "PORT"))
        self.label_7.setText(_translate("MainWindow", "LOG"))
        self.btn_read.setText(_translate("MainWindow", "READ"))
        self.label_3.setText(_translate("MainWindow", "ID"))
        self.label_4.setText(_translate("MainWindow", "BROKER"))
        self.label_5.setText(_translate("MainWindow", "SSID"))
        self.label_6.setText(_translate("MainWindow", "PASSWORD"))
        self.btn_sync_rtc.setText(_translate("MainWindow", "SYNC RTC"))


if __name__ == "__main__":
    import sys
    app = QtWidgets.QApplication(sys.argv)
    MainWindow = QtWidgets.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
