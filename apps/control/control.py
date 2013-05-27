#control module
#ENCE461
#

from Tkinter import *

import bluetooth
import time

MAC = '00:07:80:40:78:C4'

num_commands = 8
LEFT, RIGHT, FORWARD, BACK, STOP, ASSUME_CONTROL, CAM_OFF, CAM_ON = [i for i in range(num_commands)]

class App:

    def __init__(self, master):

        self.frame = Frame(master)
        
        self.bt_connected = 0

        self.speed = Scale(from_= 100, to = 0)
        self.speed.grid(row = 1,column = 4 )

        self.button_left = Button(text="left", command= lambda: self.send_command (LEFT))
        self.button_left.grid(row = 1,column = 1)

        self.button_right = Button(text="right", command= lambda: self.send_command (RIGHT))
        self.button_right.grid(row = 1,column =  3)
        
        self.button_forward = Button(text="forward", command= lambda: self.send_command (FORWARD))
        self.button_forward.grid(row = 0,column = 2 )
        
        self.button_backward = Button(text="back", command= lambda: self.send_command (BACK))
        self.button_backward.grid(row = 2,column = 2 )
        
        self.button_stop = Button(text="Stop", command= lambda: self.send_command (STOP))
        self.button_stop.grid(row = 1,column = 2 )
        
        self.button_connect = Button(text="Connect", command=self.connect_bt)
        self.button_connect.grid(row = 4,column = 2 )
        
        self.button_connect = Button(text="Disconnect", command=self.disconnect_bt)
        self.button_connect.grid(row = 5,column = 2 )
        
        root.bind ("<Left>", lambda event, a = LEFT: self.send_command (a))
        root.bind ("<Right>", lambda event, a = RIGHT: self.send_command (a))
        root.bind ("<Up>", lambda event, a = FORWARD: self.send_command (a))
        root.bind ("<Down>", lambda event, a = BACK: self.send_command (a))
        root.bind ("<s>", lambda event, a = STOP: self.send_command (a))
        root.bind ("<c>", self.connect_bt)
        root.bind ("<d>", self.disconnect_bt)
        

    def connect_bt(self, *args):
        if (not self.bt_connected):
            self.s = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
            print "Connecting..."
            try:
                self.s.connect((MAC,1))
                self.bt_connected = 1
                print "Connected"
            except bluetooth.btcommon.BluetoothError, e:
                self.bt_connected = 0
                print 'Failed,', e
                self.s.close()
        else:
            print "Already Connected"
            
    def disconnect_bt (self, *args):
        if (self.bt_connected):
            self.s.close ()
            print "Disconnected"
            self.bt_connected = 0
        else:
            print "Already Disconnected"
            
            
        
    def send_command(self, *args):
        if self.bt_connected:
            if (len(args) == 1):
                print "Command: ", args[0]
                self.s.send(str(args[0]))
                #print self.s.recv(1024)
                #print self.s.recv(1024)
            elif (len(args) == 2):
                print "Command: ", args[1]
                self.s.send(str(args[1]))
                #print self.s.recv(1024)

root = Tk()

app = App(root)

root.mainloop()


