import sys, os, time

def sendMessages():
    while True:
        try:
            print(".", end="")
            os.system("cansend vcan1 680#010F")
            os.system("cansend vcan1 004#0279")
            time.sleep(1)
        except KeyboardInterrupt:
            print("Exiting...")
            exit()

if __name__ == "__main__":
    sendMessages()