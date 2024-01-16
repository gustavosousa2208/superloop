#!/bin/bash

while true; do
    output=$(bluetoothctl scan on)
    
    # Check if the output contains the target Bluetooth device address
    if [[ $output =~ "8C:41:F2:89:47:4D" ]]; then
        echo "Found the Bluetooth device!"
        break
    fi

    # Sleep for a short duration before the next iteration
    sleep 1
done

bluetoothctl pair 8C:41:F2:89:47:4D
bluetoothctl connect 8C:41:F2:89:47:4D
bluetoothctl trust 8C:41:F2:89:47:4D


