#!/bin/sh
#
#--------------------------------------------------------------------
# Define main path for external include files
#
CP_INC_PATH="/Volumes/DevSoft/MicroControl/Libraries/CANpie";

if [[ "$OSTYPE" == "linux-gnu" ]]; then
   echo "Setting include path for Linux";
   INC_PATH=$CP_INC_PATH/PCAN_Basic_Linux_4.1.0/pcanbasic
elif [[ "$OSTYPE" == "darwin"* ]]; then
   echo "Setting include path for Mac OSX";
   INC_PATH=$CP_INC_PATH/PCAN_Basic_Mac_0.6.0
else
   echo "Operating system not supported";
   exit;
fi

#--------------------------------------------------------------------
# Create 'qcan_peak/include' sym-link
#
test -d qcan_peak/include && { echo "Link to directory 'qcan_peak/include' already present";} || 
   { echo "Create sym-link for 'qcan_peak/include' directory";
     ln -s $INC_PATH qcan_peak/include;} 
     
     
