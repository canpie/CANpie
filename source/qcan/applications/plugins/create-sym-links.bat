@echo off
:--------------------------------------------------------------------
: The setlocal enabledelayedexpansion enableextensions with the !var! 
: rather than %var% is necessary to prevent odd behavior, so that 
: variables behave properly when within a FOR or an IF. It's an 
: oddity with batch files. See set /? where that is mentioned further.
:
setlocal enabledelayedexpansion enableextensions

:--------------------------------------------------------------------
: Define main path for include files
:
set MCL_PATH="O:\MicroControl\Libraries\CANpie"


:--------------------------------------------------------------------
: Create sym-links to include patch of each plugin
:
if not exist .\qcan_ixxatusb\include mklink /d .\qcan_ixxatusb\include %MCL_PATH%\ixxat_VCI_3_5_4_4533\sdk\Microsoft_VisualC\inc
if not exist .\qcan_peakusb\include mklink /d .\qcan_peakusb\include %MCL_PATH%\peak_PCANBasic_4.0.3.75\Include


echo.&pause
