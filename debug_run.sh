#!/bin/bash

# Navigate to the executable directory
cd build/UndergroundBeats_artefacts/Underground\ Beats.app/Contents/MacOS

# Run with output to log file
./Underground\ Beats > ~/underground_beats_log.txt 2>&1

# Display the log
echo "Application exited with status $?"
echo "Log file contents:"
cat ~/underground_beats_log.txt
