#!/bin/bash

# ── Adjust these paths to your actual binaries ──────────────────
SERVER_BIN="./server"
CLIENT_BIN="./program"
GUI_BIN="./../gui/build/telemetry_gui"

# ── Try common terminal emulators in order ──────────────────────
if command -v gnome-terminal &> /dev/null; then
    gnome-terminal --title="Telemetry Server" -- bash -c "$SERVER_BIN; exec bash" &
    sleep 1
    gnome-terminal --title="Telemetry Client" -- bash -c "$CLIENT_BIN; exec bash" &

    # Delay GUI launch 5 seconds
    sleep 5
    gnome-terminal --title="Telemetry GUI" -- bash -c "$GUI_BIN; exec bash" &

elif command -v xterm &> /dev/null; then
    xterm -title "Telemetry Server" -e "$SERVER_BIN; bash" &
    sleep 1
    xterm -title "Telemetry Client" -e "$CLIENT_BIN; bash" &

    # Delay GUI launch 10 seconds
    sleep 10
    xterm -title "Telemetry GUI" -e "$GUI_BIN; bash" &

elif command -v konsole &> /dev/null; then
    konsole --hold --title "Telemetry Server" -e "$SERVER_BIN" &
    sleep 1
    konsole --hold --title "Telemetry Client" -e "$CLIENT_BIN" &

    # Delay GUI launch 5 seconds
    sleep 5
    konsole --hold --title "Telemetry GUI" -e "$GUI_BIN" &

else
    echo "No supported terminal emulator found (gnome-terminal, xterm, konsole)."
    exit 1
fi

echo "Started server, client, and GUI in separate terminals (GUI delayed by 5s)."