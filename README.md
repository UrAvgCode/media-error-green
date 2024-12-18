# Media Error Green

## Getting Started

To get started with this project, follow these steps:

### 1. Clone the repository
    ```sh
    git clone https://github.com/UrAvgCode/media-error-green.git --recurse-submodules
    ```

### 2. Run the setup script
    ```sh
    cd media-error-green/scripts
    ./setup.sh
    ```

### 3. Install Azure Kinect Dependencies
* Install the [Azure Kinect Sensor SDK](https://download.microsoft.com/download/d/c/1/dc1f8a76-1ef2-4a1a-ac89-a7e22b3da491/Azure%20Kinect%20SDK%201.4.2.exe).
* Install the [Azure Kinect Body Tracking SDK](https://www.microsoft.com/en-us/download/details.aspx?id=104221).
* Add an environment variable for `AZUREKINECT_SDK` and set it to `C:\Program Files\Azure Kinect SDK v1.4.2`.
* Add an environment variable for `AZUREKINECT_BODY_SDK` and set it to `C:\Program Files\Azure Kinect Body Tracking SDK`.


* Add `%AZUREKINECT_SDK%\sdk\windows-desktop\amd64\release\bin` to the `PATH` variable.
* Add `%AZUREKINECT_BODY_SDK%\sdk\windows-desktop\amd64\release\bin` to the `PATH` variable.
* Add `%AZUREKINECT_BODY_SDK%\tools` to the `PATH` variable.
