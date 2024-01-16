#include "funcs.h"
#include "shares.h"

// no raspberry é event2 e na orange é event8

void * readDS4(void *arg ) {
    struct can_frame thisFrame;
    const char *device_path = "/dev/input/event2";
    int fd = open(device_path, O_RDONLY | O_NONBLOCK);
    bool remote = false;

    if (fd < 0) {
        perror("Unable to open joystick device");
        return 1;
    } 

    struct libevdev *dev = NULL;
    int rc = libevdev_new_from_fd(fd, &dev);

    if (rc < 0) {
        fprintf(stderr, "Failed to initialize libevdev: %s\n", strerror(-rc));
        close(fd);
        return 1;
    }

    printf("Reading joystick events. Press Ctrl+C to exit.\n");

    while (1) {
        struct input_event ev;
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);

        // não to tratando erros do evdev, simplesmente ignorando e printando o que vier
        if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
            // printf("Event: %s %s %d\n",
            //         libevdev_event_type_get_name(ev.type),
            //         libevdev_event_code_get_name(ev.type, ev.code),
            //         ev.value);

            switch(ev.code) {
                case 315:
                    if(remote) break;
                    if (ev.value == 1) {
                        thisFrame.can_id = 0x222;
                        thisFrame.can_dlc = 1;

                        thisFrame.data[0] = 0x06;

                        remote = true;
                    }
                    if(sendInverterDataOnce(thisFrame) == -1){
                        printf("couldnt send");
                        return 1;
                    }
                    break;
                case 304:
                    if (ev.value == 1) {
                        thisFrame.can_id = 0x222;
                        thisFrame.can_dlc = 1;

                        thisFrame.data[0] = 0x00    ;
                    }
                    if(sendInverterDataOnce(thisFrame) == -1){
                        printf("couldnt send");
                        return 1;
                    }
                    break;
                default:
                    break;
            }
            

        }
    }

    libevdev_free(dev);
    close(fd);

    return 0;
}
