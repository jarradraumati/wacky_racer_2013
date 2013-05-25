#control module
#ENCE461
#

from Tkinter import *

class App:

    def __init__(self, master):

        frame = Frame(master)

        self.speed = Scale(from_= 100, to = 0)
        self.speed.grid(row = 1,column = 4 )

        self.button_left = Button(text="left", command=self.highlight_left)
        self.button_left.grid(row = 1,column = 1)

        self.button_right = Button(text="right", command=self.say_hi)
        self.button_right.grid(row = 1,column =  3)
        
        self.button_forward = Button(text="forward", command=self.say_hi)
        self.button_forward.grid(row = 0,column = 2 )
        
        self.button_backward = Button(text="back", command=self.say_hi)
        self.button_backward.grid(row = 2,column = 2 )
        
        self.button_stop = Button(text="Stop", command=self.say_hi)
        self.button_stop.grid(row = 1,column = 2 )
        
        self.button_change = Button(text="Change Speed", command=self.check_val)
        self.button_change.grid(row = 4,column = 2 )
        

    def say_hi(self):
        print "hi there, everyone!"
    
    def check_val(self):
        v = self.speed.get()
        print v
    def highlight_left(self):
        self.button_left.configure(bg = "red")
        
        

root = Tk()

app = App(root)

root.mainloop()

