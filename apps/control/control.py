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

BUFFER = 1024

root = Tk()
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
            print ("saw something!")
            recv_data, recv_addr = ready.recvfrom (BUFFER)
            print ("%d" %recv_data)
            #for i in recv_data:
                #picture_out.write(str(hex(ord(i))))
                #picture_out.write (",")
                #recvd_bytes = recvd_bytes + 1
                #if (recvd_bytes % 256 == 0):
                    #picture_out.write(" : %d\n" % line_num)
                    #line_num = line_num + 1
                    


class App:

    def __init__(self, master):
        
        self.possible_speed = [100,90,80,70,60,50,40,30,20,10,0,-10,-20,-30,-40,-50,-60,-70,-80,-90,-100]
        
        global bt_sock, picture_out
        self.s = bt_sock
        
        self._job_S = None
        self._job_T = None
        
        self.frame = Frame(master)
        
        self.bt_connected = 0
        
        self.speed = Scale(from_= 100, to = -100, command= self.updateSpeed)
        self.speed.grid(row = 1,column = 5 )
        
        self.turn = Scale(from_= 100, to = -100,orient=HORIZONTAL, command= self.updateTurn)
        self.turn.grid(row = 2,column = 5 )

        self.button_left = Button(text="left", command= lambda: self.left_turn ())
        self.button_left.grid(row = 1,column = 2)

        self.button_right = Button(text="right", command= lambda: self.right_turn () )
        self.button_right.grid(row = 1,column =  4)
        
        self.button_forward = Button(text="forward", command= lambda: self.forward_speed () )
        self.button_forward.grid(row = 0,column = 3 )
        
        self.button_backward = Button(text="back", command= lambda: self.backwards_speed () )
        self.button_backward.grid(row = 2,column = 3 )
        
        self.button_stop = Button(text="Stop", bg = 'red', command= lambda: self.stop () )
        self.button_stop.grid(row = 1,column = 3 )
        
        self.button_connect = Button(text="Connect", command=self.connect_bt)
        self.button_connect.grid(row = 4,column = 2 )
        
        self.button_connect = Button(text="Disconnect", command=self.disconnect_bt)
        self.button_connect.grid(row = 5,column = 3 )
        
        self.button_connect = Button(text="Take Photo", command= lambda: self.send_command (CC_CAPTURE))
        self.button_connect.grid(row = 6,column = 2 )
        
        self.button_connect = Button(text="CAM Sleep", command= lambda: self.send_command (CC_SLEEP_CAM))
        self.button_connect.grid(row = 6,column = 3 )
        
        self.button_connect = Button(text="CAM Wake", command= lambda: self.send_command (CC_WAKE_CAM))
        self.button_connect.grid(row = 6,column = 4 )
        
        self.button_connect = Button(text="CamBoard Wake", command= lambda: self.send_command (CC_SLEEP))
        self.button_connect.grid(row = 6,column = 5 )
        
        self.button_connect = Button(text="Photo Ready", command= lambda: self.send_command (CD_PHOTO_READY))
        self.button_connect.grid(row = 7,column = 2 )
        
        self.button_connect = Button(text="Retrieve Line", command= lambda: self.send_command (CD_PHOTO_LINE))
        self.button_connect.grid(row = 7,column = 3 )     
        
        self.button_connect = Button(text="Close File", command=self.close_file)
        self.button_connect.grid(row = 7,column = 4 )   
        
        root.bind ("<Left>", lambda event: self.left_turn ())
        root.bind ("<Right>", lambda event: self.right_turn ())
        root.bind ("<Up>", lambda event: self.forward_speed () )
        root.bind ("<Down>", lambda event: self.backwards_speed ())
        root.bind ("<s>", lambda event: self.stop ())
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
            command = chr (args[0])
            self.s.send(command)
            #print self.s.recv(1024)
            

            
            #if (len(args) == 1):
                #command = chr (args[0])
                #print "Command: ", ord(command)
                #self.s.send(command)
                ##print self.s.recv(1024)
                ##print self.s.recv(1024)
            #elif (len(args) == 2):
                #command = chr (args[1])
                #print "Command: ", ord(command)
                #self.s.send(command)
                ##print self.s.recv(1024)
                
    def updateTurn(self, event):
        if self._job_T:
            root.after_cancel(self._job_T)
        self._job_T = root.after(300, self.update_bt_turn)
        
    
    def updateSpeed(self, event):
        if self._job_S:
            root.after_cancel(self._job_S)
        self._job_S = root.after(300, self.update_bt_speed)
        
    def update_bt_turn(self):
        self._job_T = None
        self.tempVal = self.turn.get()
        if self.tempVal not in self.possible_speed:
            self.turn.set(int(round(self.tempVal/100.0,1)*100.0))
        else:    
            print "turn value:", self.tempVal
            if self.tempVal > 0:
                self.tempVal = 40 + ((self.tempVal/10) - 1)
                self.send_command(self.tempVal)
                print "speed comm value:", self.tempVal
            elif self.tempVal < 0:
                self.tempVal = 50 + (((self.tempVal*-1)/10) - 1)
                self.send_command(self.tempVal)
                print "speed comm value:", self.tempVal
            elif self.tempVal == 0:
                self.send_command(60)
                print "speed comm value:", 60
    
        
    def update_bt_speed(self):
        self._job_S = None
        self.tempVal = self.speed.get()
        if self.tempVal not in self.possible_speed:
            self.speed.set(int(round(self.tempVal/100.0,1)*100.0))
        else:    
            print "speed value:", self.tempVal
            if self.tempVal > 0:
                self.tempVal = 20 + ((self.tempVal/10) - 1)
                self.send_command(self.tempVal)
                print "speed comm value:", self.tempVal
            elif self.tempVal < 0:
                self.tempVal = 30 + (((self.tempVal*-1)/10) - 1)
                self.send_command(self.tempVal)
                print "speed comm value:", self.tempVal
            elif self.tempVal == 0:
                self.send_command(62)
                print "speed comm value:", 62
    
    def stop(self):
        self.speed.set(0)
        self.turn.set(0)
        self.send_command(61)
        print "speed comm value:", 61
        
        
    def forward_speed(self):
        currentSpeed = self.speed.get()
        self.speed.set(currentSpeed + 10)
    
    def backwards_speed(self):
        currentSpeed = self.speed.get()
        self.speed.set(currentSpeed - 10)
        
    def left_turn(self):
        currentSpeed = self.turn.get()
        self.turn.set(currentSpeed + 10)
    
    def right_turn(self):
        currentSpeed = self.turn.get()
        self.turn.set(currentSpeed - 10)

app = App(root)

# Setup IO Thread
thread_io = threading.Thread (target = select_wait, args = ([bt_sock], [], []))
thread_io.daemon = True
thread_io.start ()

root.mainloop()


