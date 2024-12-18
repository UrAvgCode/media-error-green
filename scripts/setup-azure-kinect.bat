@echo off

:: Set environment variables
setx AZUREKINECT_SDK "C:\Program Files\Azure Kinect SDK v1.4.2"
setx AZUREKINECT_BODY_SDK "C:\Program Files\Azure Kinect Body Tracking SDK"

:: Add Sensor SDK bin folder to PATH
setx PATH "%PATH%;%AZUREKINECT_SDK%\sdk\windows-desktop\amd64\release\bin"

:: Add Body SDK bin folder to PATH
setx PATH "%PATH%;%AZUREKINECT_BODY_SDK%\sdk\windows-desktop\amd64\release\bin"

:: Add Body SDK tools folder to PATH
setx PATH "%PATH%;%AZUREKINECT_BODY_SDK%\tools"

echo environment variables and PATH updated successfully