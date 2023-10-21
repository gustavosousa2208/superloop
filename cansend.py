import sys, os, time

def sendMessages():
    while True:
        try:
            print(".", end="")
            os.system("cansend vcan1 680#010F")
            os.system("cansend vcan1 004#7902")
            time.sleep(1e-3)
        except KeyboardInterrupt:
            print("Exiting...")
            exit()

if __name__ == "__main__":
    sendMessages()