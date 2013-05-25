#control module
#ENCE461
#

from Tkinter import *

class App:

    def __init__(self, master):

        frame = Frame(master)

        self.button_left = Button(text="left")
        self.button_left.grid(row = 1,column = 1)

        self.button_right = Button(text="right", command=self.say_hi)
        self.button_right.grid(row = 1,column =  3)
        
        self.button_forward = Button(text="forward", command=self.say_hi)
        self.button_forward.grid(row = 0,column = 2 )
        
        self.button_backward = Button(text="back", command=self.say_hi)
        self.button_backward.grid(row = 2,column = 2 )
        
        self.button_stop = Button(text="Stop", command=self.say_hi)
        self.button_stop.grid(row = 1,column = 2 )
        

    def say_hi(self):
        print "hi there, everyone!"

root = Tk()

app = App(root)

root.mainloop()

