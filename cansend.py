import sys, os, time

interface = "vcan0"

# TODO: o formato de mensagens via can mudou, mudar o código de acordo

def sendMessages():
    while True:
        try:
            print(".", end="")
            # os.system(f"cansend {interface} 003#7902")
            # os.system(f"cansend {interface} 004#7902")
            # os.system(f"cansend {interface} 007#7902")
            # os.system(f"cansend {interface} 030#3501")
            # os.system(f"cansend {interface} 033#3501")
            # os.system(f"cansend {interface} 034#3501")
            # os.system(f"cansend {interface} 680#010F")
            # os.system(f"cansend {interface} 685#0000")
            
            os.system(f"cansend {interface} 701#0001000100010001")
            os.system(f"cansend {interface} 702#000200020002")
            os.system(f"cansend {interface} 703#0003790200030003")
            
            time.sleep(0.1)
            
        except KeyboardInterrupt:
            print("Exiting...")
            exit()

if __name__ == "__main__":
    sendMessages()
