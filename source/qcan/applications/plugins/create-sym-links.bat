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
if not exist .\qcan_ixxat\include mklink /d .\qcan_ixxat\include %MCL_PATH%\ixxat_VCI_3_5_4_4533\sdk\Microsoft_VisualC\inc
if not exist .\qcan_peak\include mklink /d .\qcan_peak\include %MCL_PATH%\PCAN_Basic_Windows_4.0.3.75\Include

echo.&pause
