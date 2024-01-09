#include "funcs.h"
#include "shares.h"

// no raspberry é event2 e na orange é event8

void * readDS4(void *arg ) {
    const char *device_path = "/dev/input/event8";
    int fd = open(device_path, O_RDONLY | O_NONBLOCK);

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

        if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
            if (ev.type == EV_ABS) {
                printf("Axis %d: %d\n", ev.code, ev.value);
            } else if (ev.type == EV_KEY) {
                printf("Button %d: %d\n", ev.code, ev.value);
            }
        } else if (rc == LIBEVDEV_READ_STATUS_SYNC) {
            // Resync can happen for various reasons; it's not an error.
            continue;
        } 
        // else if (rc < 0) {
        //     // An error occurred.
        //     printf("Failed to handle events: %s\n", strerror(-rc));
        //     break;
        // }
    }

    libevdev_free(dev);
    close(fd);

    return 0;
}
