import sys, os, time

interface = "vcan0"

def sendMessages():
    while True:
        try:
            print(".", end="")
            os.system(f"cansend {interface} 003#7902")
            os.system(f"cansend {interface} 004#7902")
            os.system(f"cansend {interface} 007#7902")
            os.system(f"cansend {interface} 030#3501")
            os.system(f"cansend {interface} 033#3501")
            os.system(f"cansend {interface} 034#3501")
            os.system(f"cansend {interface} 680#010F")
            os.system(f"cansend {interface} 685#0000")
            time.sleep(0.020)
        except KeyboardInterrupt:
            print("Exiting...")
            exit()

if __name__ == "__main__":
    sendMessages()