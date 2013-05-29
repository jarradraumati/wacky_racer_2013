#control module
#ENCE461
#

from Tkinter import *
import threading
import select

import bluetooth
import time

MAC = '00:07:80:40:78:C4'
start_mc = 40
start_cc = 50
num_mc = 6
num_cc = 4
MC_LEFT, MC_RIGHT, MC_FORWARD, MC_BACK, MC_STOP, MC_SLEEP = [i for i in range (start_mc, start_mc+num_mc)]
CC_CAPTURE, CC_SLEEP_CAM, CC_WAKE_CAM, CC_SLEEP = [i for i in range (start_cc, start_cc+num_cc)]
CD_PHOTO_READY = 70
CD_PHOTO_LINE = 71
CD_FAULT = 73

BUFFER = 1024

bt_sock = bluetooth.BluetoothSocket(bluetooth.RFCOMM)

recvd_bytes = 0
line_num = 0

picture_out = open ('picture.csv', 'w')

def select_wait (r,w,e):
    global line_num, recvd_bytes
    while True:
        readable, writeable, exceptional = select.select (r, w, e)

        # Deal with sockets ready to be read
        # print readable
        for ready in readable:
            print ("An event happened, processing it")
            recv_data, recv_addr = ready.recvfrom (BUFFER)
            for i in recv_data:
                print (str(hex(ord(i))))
                #picture_out.write(str(hex(ord(i))))
                #picture_out.write (",")
                #recvd_bytes = recvd_bytes + 1
                #if (recvd_bytes % 256 == 0):
                    #picture_out.write(" : %d\n" % line_num)
                    #line_num = line_num + 1
                    


class App:

    def __init__(self, master):
        
        global bt_sock, picture_out
        self.s = bt_sock

        self.frame = Frame(master)
        
        self.bt_connected = 0

        self.speed = Scale(from_= 100, to = 0)
        self.speed.grid(row = 1,column = 4 )

        self.button_left = Button(text="left", command= lambda: self.send_command (MC_LEFT))
        self.button_left.grid(row = 1,column = 1)

        self.button_right = Button(text="right", command= lambda: self.send_command (MC_RIGHT))
        self.button_right.grid(row = 1,column =  3)
        
        self.button_forward = Button(text="forward", command= lambda: self.send_command (MC_FORWARD))
        self.button_forward.grid(row = 0,column = 2 )
        
        self.button_backward = Button(text="back", command= lambda: self.send_command (MC_BACK))
        self.button_backward.grid(row = 2,column = 2 )
        
        self.button_stop = Button(text="Stop", command= lambda: self.send_command (MC_STOP))
        self.button_stop.grid(row = 1,column = 2 )
        
        self.button_connect = Button(text="Connect", command=self.connect_bt)
        self.button_connect.grid(row = 4,column = 2 )
        
        self.button_connect = Button(text="Disconnect", command=self.disconnect_bt)
        self.button_connect.grid(row = 5,column = 2 )
        
        self.button_connect = Button(text="Take Photo", command= lambda: self.send_command (CC_CAPTURE))
        self.button_connect.grid(row = 6,column = 1 )
        
        self.button_connect = Button(text="Cam Fault", command= lambda: self.send_command (CD_FAULT))
        self.button_connect.grid(row = 6,column = 2 )
        
        self.button_connect = Button(text="CAM Wake", command= lambda: self.send_command (CC_WAKE_CAM))
        self.button_connect.grid(row = 6,column = 3 )
        
        self.button_connect = Button(text="CamBoard Wake", command= lambda: self.send_command (CC_SLEEP))
        self.button_connect.grid(row = 6,column = 4 )
        
        self.button_connect = Button(text="Photo Ready", command= lambda: self.send_command (CD_PHOTO_READY))
        self.button_connect.grid(row = 7,column = 1 )
        
        self.button_connect = Button(text="Retrieve Line", command= lambda: self.send_command (CD_PHOTO_LINE))
        self.button_connect.grid(row = 7,column = 2 )     
        
        self.button_connect = Button(text="Retrieve Line", command=self.close_file)
        self.button_connect.grid(row = 7,column = 3 )   
        
        root.bind ("<Left>", lambda event, a = MC_LEFT: self.send_command (a))
        root.bind ("<Right>", lambda event, a = MC_RIGHT: self.send_command (a))
        root.bind ("<Up>", lambda event, a = MC_FORWARD: self.send_command (a))
        root.bind ("<Down>", lambda event, a = MC_BACK: self.send_command (a))
        root.bind ("<s>", lambda event, a = MC_STOP: self.send_command (a))
        root.bind ("<c>", self.connect_bt)
        root.bind ("<d>", self.disconnect_bt)
        
    
    def close_file (self):
        picture_out.close ()
        
    def connect_bt(self, *args):
        if (not self.bt_connected):
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
                command = chr (args[0])
                print "Command: ", ord(command)
                self.s.send(command)
                #print self.s.recv(1024)
                #print self.s.recv(1024)
            elif (len(args) == 2):
                command = chr (args[1])
                print "Command: ", ord(command)
                self.s.send(command)
                #print self.s.recv(1024)

root = Tk()

app = App(root)

# Setup IO Thread
thread_io = threading.Thread (target = select_wait, args = ([bt_sock], [], []))
thread_io.daemon = True
thread_io.start ()

root.mainloop()


