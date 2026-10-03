
# Base Application

1. [Project Structure](#structure)
2. [Project Software Architecture](#arch)
3. [Project Building Steps](#build)
4. [Project Resources](#resources)
5. [Services](#services_id)
6. [Data and Configuration Manager](#data_manager)
7. [UI Service and Lvgl HAL](#ui_service)
8. [CAN Service](#can)
9. [Backlight Service](#backlight)
10. [Audio Service](#audio)
11. [Access Service](#auth)
12. [Logger and debug](#logger)
13. [Norms and coding rules](#rules)

---

<a id="structure"></a>
## 1. Project Structure

The project main target is the Linux Yocto IMX8 board using DRM as UI backend but it can be compiled to run on the Ubuntu host using SDL as the main driver platform.
More details are provided in the project architecture [chapter](#arch)

The project source code is structured in 2 main folders:
- **/include** for C++ headers;
- **/src** for C++ sources;

There are some additional folders as follows:
- **/cmake** contains cmake scripts and cmake specific files;

- **/config** contains initialization and configurations files used at the application startup - when the project is compiled they are cpoied into the /deploy folder for the specific target;

- **/deploy** contains the application executable and additional files, specific to each platform - host or target. The build script will deploy the application from this folder into the target.

- **/docs** contains the project documentation;

- **/external** contains external or compiled libraries. **/lib_lvgl** contains the lvgl project sources and **/lib_lvgl/libs/** contains the compiled lvgl library for both host and target;

- **/resources** contains project resources like fonts, images, sounds etc. Some resources are compiled directly into the executable, see [project building steps](#build);

- **/scripts** contains python helper scripts;

- **/simulators** contains python simulators like for example the can_simulator.py;

- **/templates** contains file templates for convenience;

- **/tests** contains project tests and unit tests (TODO); 

---

<a id="arch"></a>
## 2. Project Software Architecture

The project architecture is based on services that communicates through events. The main objects are packet into an external project called **Project Core** and which is compiled and used as a static library.

A **service** is an object that is responsable doing one thing like for example the UI or CAN services which are responsable with UI and CAN communications.
Each service has a run() method which is runned in another thread and where all the servie work is done and also where the incoming or outgoing events are handled.
Some services may handle they own worker thread like for example the [Audio Service](#audio).
It inherits from an object **ServiceBaseQueue** that has already all the basics so that the service only need to subscribe and register a callback to the desired event and the rest is automated.

An **event** is an object used to pass information between the services. With some exxceptions, they do not carry information but they are used to inform that something happened that must act upon. 
The event system is based on the observer pattern with the notable difference that the events are not passed between services but are dispatched by a central unit called the Event Manager.

The **Event Manager** is the central piece of the events system and all the services have a reference to it. It holds a subscriber list for each event type so that when something occurs, the subscribed service is called.
A service can send an event but another service can intercept it only if it is subscribed to that event.

The **Service Manager** is another central object of this architecture. Is a convenience object that registers all the services and handles their lifetime. All services MUST pass through the Service Manager in order to assure proper lifetime.

The application architecture is based on the Core architecture. The EventManager is created in the main thread and is passed to and handled by the ServiceManager. All the services are instantiated by the ServiceManager through register() method.

Each Service inherits from the **ServiceBaseQueue** and the run() method is launched into another thread. All the local callback dispatching MUST be handled in this method. Please consult the Core project documentation.

The Services subscribe to events and react to event through callbacks. All the callbacks are run in the service thread, if other secondary threads are launched from one service they must be own and handled by the service.

Data can be exchanged through event payloads, but it is not reccomended! If the payload is small and scarce, then it is ok. For periodic telemetry data, this method must be avoided synce it can lead to the classic producer/consumer syncronization problems - if the producer produces messages faster than the consumer can consume, then the events queue start to fill and a delay between the event emission and event dispatching will lead to problems ! In this case, the DataManager must be used. A service will update a data field in the DataManager through a specific interface then it can emit a data ready event or not depending on the use case.

Startup settings and initial data are gathered by the Configuration Manager then stored into DataManager. 

---

<a id="build"></a>
## 3. Project Building Steps

### A. Project dependencies

- CMake ≥ 3.22
- C++17 compiler
- Lvgl >= 9.5
- libssl-dev
- can-utils
- Libcore (internal)
- Python 3 on host
- python "lz4" library
- python "pypng" library

Install command for the python libraries:
```python
pip3 install pypng lz4
```
Lvgl is added as a submodule. Update git submodules command:
```git
git submodule update --init --recursive
```

#### Host specific dependencies (Simulator)

- SDL2
- ALSA
- libssl-dev

#### Target specific dependencies (Yocto)

- DRM / GBM / EGL / GLESv2
- ALSA
- OPENSSL

Target builds require sourcing the Yocto SDK environment like for example:

```bash
. /opt/seco-sdk-kirkstone/environment-setup-cortexa53-crypto-poky-linux
```

The build script handles this automatically but the SDK_PATH variable MUST be valid. IMPORTANT: If missing, build will fail.

Use the generated SDK for the Yocto project depending of your configuration. DO NOT USE A SYSROOT IMAGE : you will not have the same files on target as the SDK and you will be obliged to install libraries on the target that are not needed at runtine thus polluting the OS or to have 2 separate images: one for development and one for the target creating confusion and deplyment problems in production !!

#### CORE Library

The project depends on the external core library. Set the path in the main **CMakeLists.txt** to the Core Library deploy folder.

Ex:
```
set(CORE_PATH "/home/rd2/__DEV__/Project_Core/deploy")
```

This path must contain:

CORE_PATH/host/libcore.a  or  CORE_PATH/target/libcore.a  depending on the build type.

### B. Building the application

This project builds an embedded application with two distinct execution environments:

- **Host**: SDL2-based simulator app
- **Target**: Yocto-based embedded Linux (DRM/KMS, EGL, GBM)

It also supports **standalone resource compilation** (images and audio) and integration of these resources into the main application executable.

A single script (`build.sh`) drives all operations:

- Host build (SDL simulator)
- Target build (Yocto cross-compilation)
- Resource generation (audio / images)
- Deployment to target
- Run on host

The build process is handled by CMake with Ninja and ccache (optional). The CMakeLists.txt is the build entry point.

Build the application using the build.sh script:

```
./build.sh [host|target] [debug|release|reldeb|min]
```

MODE:
- host : Native build using SDL2
- target : Cross-compilation using Yocto SDK

Build types:
- debug : Debug
- release :	Release
- reldeb : RelWithDebInfo (default)
- min : MinSizeRel

Options:
- run : run on the target;
- deploy : deploy and run on target - check the TARGET address in the script

For example, build in release mode for the target and deploy:

```
./build.sh target release deploy
```

or

```
./build.sh host run release
```

The delpoyment on the target is done via ssh. You can change the target address by modifying the TARGET variable in the build.sh script.

```bash
TARGET="root@192.168.32.33"
```
You can also change the Yocto SDK path in the build.sh:

```bash
SDK_PATH="/opt/seco-sdk-kirkstone/environment-setup-cortexa53-crypto-poky-linux"
```

The build directories for the main application are separated for the host and the target. Also resources build folders are also separated to maximize build times and ccache hits.

### C. Building resources

The **/resources/** folder contains its own CMakeLists.txt file that gathers all the resource building cmake scripts.

You can build images and sounds and copile them into the executable. 

Build resources script arguments: 
- img :	Build image resources only
- audio :	Build audio resources only
- fonts : Build fonts only
- vector : Build raster ilages from vector sources
- resall :	Build all resources

The script must be launched with the arguments

```
./build.sh [host|target] [img|audio|resall]
```

Ex.
```
./build.sh host resall
```

The target type is mandatory since there can be a host version using the host compilator and a target version which used the Yocto SDK !

### D. Building LVGL library

The project uses Lvgl library for the UI. The sources are placed into **/external/lib_lvgl/** folder along with the Lvgl specific CMake file and lvgl build scripts.

Lvgl uses the **lv_conf.h** header to handle build configuration and this file must be placed in the sources root folder.

The two build scripts are using the same arguments. 

Build for host:
```
./build_lvgl.sh [debug|rel|reldeb|min] [cpu sdlsw]"
```

Build for target:
```
./build_lvgl_target.sh [debug|rel|reldeb|min] [cpu]"
```

This will compile the LVGL library **liblvgl.a** placed into **/external/lib_lvgl/libs/target** or **/external/lib_lvgl/libs/host** depending on the build mode.

The library is build to use DRM with GPU support backend on the target and SDL on the host.

The library can be build to use GPU acceleration by default, but can be build with CPU only support for debugging renderering issues.

If the library is build with the **cpu** option then the GPU acceleration is disabled.
On the host there is also the option to disable SDL texture chache by passing the **sdlsw** option to the build.

### E. Running tests

Run unit tests (WIP) :
```
./build.sh unit
```

Run cppcheck static analysis :
```
./build.sh static
```
---

<a id="resources"></a>
## 4. Project Resources

The project resources are compiled into the executable for a fast loading and to reduce resource handling problems.

Also, the lvgl resource loading objects are compiling images and fonts at the runtime anyway so this can reduce overhead.

### A. Images (png)

To add a png image to the project to be used with lvgl UI, just place the image in the **/resources/images/** folder and then call the build script.

```
./build.sh target img
```

This will compile all the images into a static library **libimages.a** placed into **/external/lib_lvgl/libs/target** or **/external/lib_lvgl/libs/host** depending on the build mode.

Every time an image changes, the images resource must be compiled.

### B. Fonts

Fonts are compiled into .c files by using the lvgl online converter: https://lvgl.io/tools/fontconverter 

```
Ex: microsport_bold_32 
BPP: 4 bbp
fallback: lv_font_montserrat_32
Symbols: ABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890.:,;(*!?')
```

Place the *.c font files into the **/resources/fonts/** and then compile them with the command:

```
./build.sh target fonts
```

This will compile all the fonts into a static library **libfonts.a** placed into **/external/lib_lvgl/libs/target** or **/external/lib_lvgl/libs/host** depending on the build mode.

Use the lvglfont resources include method by calling LV_FONT_DECLARE (https://lvgl.io/docs/open/9.5/main-modules/fonts/built_in_fonts.html)

### C. SVG Images

SVG images can be used directly with Lvgl, but in this case we can rasterize a svg image directly into a *.c file.

Use the lvgl online converter: https://lvgl.io/tools/imageconverter to compile the svg into a  .c file.

Place the resulted file and the original (for reuse) in the **/resources/vector/** folder and then call the build script.

```
./build.sh target vector
```

This will compile all the rasterized vector images into a static library **libvector.a** like the png images and placed into **/external/lib_lvgl/libs/target** or **/external/lib_lvgl/libs/host** depending on the build mode.

Use the lvgl resources include method by calling LV_IMAGE_DECLARE (https://lvgl.io/docs/open/9.5/main-modules/images/using_images.html)

### D. Sounds

The short sounds are compiled and included into the executable for fast loading and reusability. There still exists the option to load sound files from disk in case of large files.

The .wav files are supported and they must be placed into the **/resources/sounds/** and then compile them with the command:

```
./build.sh target audio
```

All the sounds will be compiled into the **libaudiores.a** static library placed into **/external/lib_audiores/target** or **/external/lib_audiores/host** depending on the build mode.
There is also a header file generated and placed into **/external/lib_audiores/include** folder.

---

<a id="services_id"></a>
## 5. Services

### Service base

A service is an entity responsable of doing one thing or handling a specific resource. In the application a service is an object that inherits from  **Core::ServiceBaseQueue**.
**ServiceBaseQueue** provides a thread-isolated service model with a mailbox (internal queue) and optional event dispatching. Services communicate only via events.

Each service MUST have a unique id declared in the ServiceId enum **/include/ServiceTypes.hpp** header file. This file is a stub from the CORE Library and the first 3 ids are common: BASE = 0, ServiceManager = 1 and TestService = 2.

There is a service template provided in the **/templates/** folder.

There are a few rules to follow when implementing a service
- Subscription in `init()`
- Event handling in `dispatch()`
- Main loop in `run()`
- Must pass `staticId` to base class

Constraints:
- Do not subscribe in constructor
- Do not block in `dispatch()`
- Do not access other services directly

The run method can be overwritten like this (CAN_Service):

```
void CanService::run()
{
    setup();

    while(running())
    {
        canUpdate();
        
        dispatchLoop();
    }
}
```

The `dispatch()` method will be called in the `dispatchLoop()` and must be overwritten to handle the events.

Example Audio service dispatch()

```
void AudioService::dispatch(const Core::Event& event)
{
    switch(event.type())
    {
        case Core::EventType::INITIALIZED:
            LOG_DEBUG("AudioService::dispatch() Initialized");
        break;

        case Core::EventType::AudioPlayCmd:
            onAudioPlay(event);
        break;

        case Core::EventType::AudioStopCmd:
            onAudioStop();
        break;

        case Core::EventType::AudioPauseCmd:
            onAudioPause();
        break;

        case Core::EventType::AudioResumeCmd:
            onAudioResume();
        break;

        case Core::EventType::AudioVolumeCmd:
            onAudioVolume(event);
        break;
        
        default: 
            break;
    }
}
```

All the services are handled by the ServiceManager in `main.cpp` through the `registerService()` method:

```
serviceManager.registerService<UI::UIService>();
```

### Add a new service

To add a new service copy the **Service** template in the corresponding source folder and modify the class name and id.

Add the source files to the compilation in the CMakeLists.txt :

```
set(APP_SOURCES ..)
```

Register the service in the `main.cpp` like this:

```
serviceManager.registerService<Access::AccessService>();
```

---

<a id="data_manager"></a>
## 6. Data and Configuration Manager

### Data Manager

Data manager is an object that store data as thread safe variables. It is instantiated into the main thread. This data is accesses through specific interfaces depending on the services.

For example, the CAN service has 2 interfaces to acces DataManager : IDataReadCanService and IDataWriteCanService.

Services can have only read or only write interfaces, depending on use case.

As a rule, all the data exchange must pass through DataManager. Data can be exchanged through event payloads, but it is not reccomended! If the payload is small and scarce, then it is ok, depending on the use case. For periodic telemetry data like DLC or GC data, this method must be avoided since it can lead to the classic producer/consumer syncronization problems - if the producer produces messages faster than the consumer can consume, the events queue start to fill and a delay between the event emission and event dispatching will lead to desyncronizations !

The DataManager accesors are organized into **/src/data** folder.

### Suggested Workflows

A producer service can update a data field in the DataManager and the consumer may read it when needed. That variable may be updated many times in the DataManager like DLC data and the consumer service will read the latest update when needed - see the [UIService](#ui_service).

If there are periodic updates with a very low frequency, an event can be emitted once the data is available. Avoid updating data and emitting update events at the same time for periodic data unless there is a very specific use case.

### Configuration Manager

The Configuration Manager parses .ini files like app.ini and can.ini then updates specific fields in the DataManager, usually at application startup.
A service can just read the data when needed, usually when is initilized. The advantage is that the user can change the application configuration without recompiling the application.

The current requirement does not need on the fly updates. That is, if the app.ini is changed, the application must be restarted.

If a field is added in the .ini file, make sure that a fallback value exists with a warning message to facilitate debugging. If critical data is missing, error popups can be displayed.

---

<a id="ui_service"></a>
## 7. UI Service and Lvgl HAL

The UI Service responsability is the boundary separation between the application and the UI. Frameworks like Qt or Lvgl in this case may have their own event systems that must not be mixed with the application events. Every event or action from the UI must pass through the UIService and every screen update must be performed by the UIService by calling the right interface.

The UI Service is just like any other service except it also maintains the **LvglPlatform** object and handles the UI creation and instantiation.

The Lvgl library supports many rendering backends and in this case the application is using DRM on the Yocto target and SDL for host builds. Host builds permit to do quick simulations and tests like for example with the virtual CAN.

The **LvglPlatform** object is responsable with handling both backends, depending on the build type. It is initilizing Lvgl and is handling the inputs like mouse or touch.

### Screen refresh rate and syncronization

The `UIService::run()` method has two very important aspects:
- All the initializations are performed in the setup() before entering the main loop;
- The operations in the main loop are performed with a fixed rate (refresh rate);

Lvgl doesn't impose a limit on the screen updates and if the screen buffer is updated faster than screen draws, tearing effects may appear or application may crash when running on OpenGl or EGL. All the external screen updates are handled in the `updateFrame()` method like for example the DLC data.

Sometimes external updates are faster than the screen refresh rate. In this case do not send events, they will acumulate and create syncronization problems.
The data is simply updated in the DataManager and is read as needed during a periodic screen update.

The Ui Service is instantiating all the screen elements and tabs, is keeping the navigation history and is passing the events as actions from the UI to the application.

The UI elements are structured into folders. All te reusable components and widgets are stored in the **/ui/components** folder.The difference between UIComponents and UIWidgets is that UIComponents are generic and do not use custom styling.

All the UI styles are handled through the `StyleManager`and defined in `Palette.hpp` through a structure of type `ThemePalette`. To chenge the style you msut define another palette and update it at runtime.

---

<a id="can"></a>
## 8. CAN Service

The CAN service handles the CAN interface and the CAN protocol used for sending and receiving commands from the physical controllers.

All the CAN opearations are handled through the CanDriver and CanSocket objects and the communication protocol is handled through the CanMessage and CanCodecs objects.

The protocol is not hardcoded. It is configurable through the `can.ini` file as long as the CAN message keeps the same structure e.g. id,sender_id,function,parameter,data.

### CAN Simulator

In order to facilitate tests, a simulator is provided in the **/simulators** folder. The simulator is using the virtual CAN interface and for that the `can_interface` in the [CAN] section must be set to `vcan0` or another virtual can interface.

The simulator has an associated can_scenarios.ini file that can run multiple scenarios. There are **sequences** and specific scenarios like **dlc** or **gc**.
The sequences will pass through all the steps defined in the sequence and then repeat.

A step is defined like this
```
; message, payload, interval
step1 = dlc_left_response_x,0 0 0 0 1, 0.1
```
and that means that the simulator will send a CAN `dlc_left_response_x` message with `1` as numeric payload then will wait 0.1 seconds (100ms) and pass to the next step.

A specific scenario like **DLC** will always have 2 values that will increment from a min value to a max value with specified increments in a specific time frame.
For example:
```
; min, max, increment, time interval
left = 0, 200, 1, 0.001
```
means that the simulator will send dlc left z axis message every 0.001 seconds (1ms) with values from 0 to 200 with 1 increments. That means that it will send every millisecond 0, 1, 2 ... 200 values.  
The `alternate` option if true will alternate the increments first for the left and then for right. If `false`the two values will be incremented at the same time.

To launch a sequence, like [SEQUENCE1] section use:
```
python3 simulators/can_simulator.py sequence1
```

To launch a scenario like [DLC] section use:
```
python3 simulators/can_simulator.py dlc
```

The simulator will wait in this case for the start aquision command which is the `start dlc aquisition` and then wil begin sending messages accordingly.

### Useful refrences

Install can-utils:
```
sudo apt install can-utils
```

https://netmodule-linux.readthedocs.io/en/latest/howto/can.html

Enable CAN interface
```
ip link set can0 type can bitrate 1000000
ip link set can0 up
```

Verify CAN interface
```
ip link show can0
```

Dispaly CAN interface parameters
```
ip -details link show can0
```

Shut down CAN interface
```
ip link set can0 down
```

Test examples
```
cansend can0 1A5#A1504536    #0x1A5,P,E
cansend can0 1A5#A1505344    #0x1A5,P,S
cansend can0 1A2#A1504132    #0x1A2,P,A 
cansend can0 1A2#A150493A    #0x1A2,P,I
```

Open candump in another console to check received messages
```
candump can0
```
---

<a id="backlight"></a>
## 9. Backlight Service

The Backlight Service is handling screen backlight and needs some configuration for the backlight paths defined in the app.ini [BACKLIGHT] section.

### Useful refrences

Here are some useful commands to find the available backlight filenames.

List backlight sys folder:
```
ls /sys/class/backlight/
```
should see something like this:
```
edp_pwm_backlight                     regulators:lvds_led_driver_backlight
```

List backlight properties file names:
```
ls /sys/class/backlight/edp_pwm_backlight
```

and should see something like these sysfs files:

```
actual_brightness  brightness         device             power              subsystem          type
bl_power           consumers          max_brightness     scale              suppliers          uevent
```

---

<a id="audio"></a>
## 10. Audio Service

The Audio Service is just like any other service except is the only one that handles a worker thread for audio playback. The playback can be paused, stopped or resumed.it also handles volume levels.

This service needs startup configuration defined in the `app.ini` in the [AUDIO] section.

The service is using the Linux ALSA library for sound playback. Sounds can be played as a file or as a resource.

### Useful refrences

Some useful Linux commands are listed below.

List sound cards:
```
aplay -l
cat /proc/asound/cards
```
List mixer controls (per card):
```
amixer -c 0 scontrols
```

Find backlight device:
```
ls /sys/class/backlight/
```

---

<a id="auth"></a>
## 11. Access Service

The Access Service is handling authentications and access throught `openssl` library. 

If a service needs authentication then it sends an `AccessRequest` event to the AccessService.The Access service will respond by sending a `PasswordRequest` event to the UI Service to display the password keyboard and once the password is entered the UI Service will respond to the AccessService with a `PasswordEntered` event that holds the password string that the user entered.

At this point, the Access service loads all the user stored hashes from the DataManager and performs the following operation:

```
if (hash_with_salt(input_from_lvgl, salt) == stored_hash) -> SUCCESS

```

The Access service will respond with an `AccessResponse` event that holds the `user level` string or `failure`.

At this stage one service should send only one access request at a time.

### Hash generation

The [USERS] section contains password hashes and salt for each access level. For example:

```
admin = bccd1149f87dd734707059cdb003bb7ccccde5083933fee76d134f291e273764:10d269348ad38924fbea9680bc583e78 ; admin
```

contains the hash and salt for the password "admin" for user level "admin" (same password as the user level in this case).

To generate password hashes a python script `generate_user_sha256.py` is avalable in the **/scripts** folder.

Execute:
```
python3 scripts/generate_user_sha256.py
```
and enter the user level and then the desired password and a hash and salt will be generated.Copy and paste these into the app.ini [USERS] for the corresponding user level.

---

<a id="logger"></a>
## 12. Logger and debug

The Core library contains a utility logger that is used to log the application. Enable logging to a file by setting in `main.cpp`

```
#define LOG_IN_PRODUCTION 1 
```

If is 0 the log messages will be set to DEBUG level to console.

Since the application is launched on the target as a service, read the console log messages in real time:
```
journalctl -u app

```
Also check the service status with:
```
systemctl status app
```
Restart service with:
```
systemctl restart app
```
The Logger provides two main usage styles:

**Compile-time macros** (efficient, filtered by LOG_COMPILED_LEVEL)
```
 LOG_INFO("Application started");
```
That means that the log messages below the specified level will not be present in the final executable. Preffer this method for embedded.

**Runtime logging API** (less efficient )

```
Utils::Logger::info("Application started");
```

---

<a id="rules"></a>
## 13. Norms and coding rules

The project is based on the ISO 90003 / NF 62304 norm.

Here is a small compliance checklist for C++17, specifically for RAII and threading.

### 1. Resource Management (RAII)

ISO 90003 demands "control of resources" and "prevention of defects." RAII is the primary idiom for this:

- Eliminate Raw new/delete: Use std::unique_ptr or std::shared_ptr. Under ISO 90003, a raw delete is a manual process prone to human error.
- Use Scoped Locks: Never call .lock() and .unlock() manually. Use std::lock_guard or std::unique_lock.
- In C++20, prefer std::scoped_lock for multiple mutexes to prevent deadlocks via the deadlock avoidance algorithm.
- Apply [[nodiscard]] to Factory Functions: If a function returns a resource (like a pointer or a handle), mark it [[nodiscard]]. This ensures the caller doesn't accidentally leak the resource by ignoring the return value.
- DO NOT THROW exceptions, rely on tests and log messages instead.

### 2. Standard Containers & Data Safety

ISO 90003 emphasizes "product integrity." Using containers correctly ensures data is not corrupted or accessed out of bounds:
- DO NOT USE dynamyc containers like std::vector, std::map, std::queue, etc. Special containers with fixed size are available in the CORE library.
- Use std::string_view for Read-Only Strings: (C++17) This avoids unnecessary allocations and copies, reducing the "memory footprint" (an ISO 9001 efficiency metric).
- Consistent Initialization: Use braced initialization {} to prevent "narrowing conversions" .
- Thread-Safe Data Access: Use std::atomic for basic types or `AData` type object for thread safety available in the CORE library.

### 3. Multithreading & Concurrency

ISO 90003 requires "traceability of execution." Some strict multithreading rules are needed:
- Use std::atomic for Shared Flags: Never use volatile for thread synchronization - is just a compiler instruction.
- Prefer std::async for Simple Tasks.
- Always Join or Detach: Ensure every std::thread has a clear path to .join() or .detach(). A crashing program due to an unjoined thread is a "major non-conformance". Upgrade Path: In C++20, switch to std::jthread.
- Use Scoped Locks: Never call .lock() and .unlock() manually. Use std::lock_guard or std::unique_lock.

### 4. Lvgl

Use lvgl async calls to update lvgl widgets from another thread than the lvgl own thread. This can be difficult to identify at times so if unsure, always use lv_async().

### 4. Data Interfaces

The DataManager accessors are named `IDataReadServiceName` for read access and `IDataWriteServiceName` for write accesss.
