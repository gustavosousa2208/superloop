import sys, os, time

def sendMessages():
    while True:
        try:
            print(".", end="")
            os.system("cansend vcan1 003#7902")
            os.system("cansend vcan1 004#7902")
            os.system("cansend vcan1 007#7902")
            os.system("cansend vcan1 030#3501")
            os.system("cansend vcan1 033#3501")
            os.system("cansend vcan1 034#3501")
            os.system("cansend vcan1 680#010F")
            os.system("cansend vcan1 685#0000")
            time.sleep(0.100)
        except KeyboardInterrupt:
            print("Exiting...")
            exit()

if __name__ == "__main__":
    sendMessages()