from pyPS4Controller.controller import Controller

    
class MyController(Controller):

    def __init__(self, **kwargs):
        Controller.__init__(self, **kwargs)
        self.a, self.b = 0, 0

    def on_x_press(self):
       print("Hello world")

    def on_x_release(self):
       print("Goodbye world")
       
    def on_options_press(self):
        self.a = 1
    def on_options_release(self):
        self.a = 0
    def on_playstation_button_press(self):
        self.b = 1
    def on_playstation_button_release(self):    
        self.b = 0
        
    def disconnect(self):
        print("Controller disconnected")
        

if __name__ == "__main__":
    while True:
        controller = MyController(interface="/dev/input/js0", connecting_using_ds4drv=False)
        # you can start listening before controller is paired, as long as you pair it within the timeout window
        controller.listen(timeout=60, on_disconnect=controller.disconnect)
    
# FIXME: se não tiver js0 em /dev da modprobe em joydev e usbhid https://wiki.archlinux.org/title/Gamepad
# 