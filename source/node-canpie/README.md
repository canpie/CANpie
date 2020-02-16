# Node.js for QCanSocket

This project provides the Node.js bindings for the QCanSocket class. The Node.js runtime can
be found here: [https://nodejs.org/de/](https://nodejs.org/de/).

An overview of the Node.js versions is available here:
[https://nodejs.org/de/about/releases/](https://nodejs.org/de/about/releases/).
It is recommended to use **Node.js 10**.

The following articles are helpful to get more into touch with N-API:

- [N-API and getting started with writing C addons for Node.js](https://hackernoon.com/n-api-and-getting-started-with-writing-c-addons-for-node-js-cf061b3eae75)
- [Node.js Addon Examples](https://github.com/nodejs/abi-stable-node-addon-examples)
- [Node.js Documentation](https://nodejs.org/api/n-api.html)

## Build prerequisites

- [Node.js 10](https://nodejs.org/de/download/)
- [python 3.x](https://www.python.org/downloads/)
- C/C++ compiler
- [Qt](https://www.qt.io/)

Additional remarks for use under Windows:

- [Windows Build Tools](https://www.npmjs.com/package/windows-build-tools) are required to compile the *.cpp sources, see here [node-gyp documentation](https://github.com/nodejs/node-gyp)
- Qt MSVC 2017 toolchain with libs whose version corresponds to installed version of [Windows Build Tools](https://www.npmjs.com/package/windows-build-tools). This Qt MSVC toolchain is provided e.g. by [Qt offline installer](http://download.qt.io/official_releases/qt/5.9/5.9.8/qt-opensource-windows-x86-5.9.8.exe)

Check the versions of required tools after installation:

```shell
> node --version
v10.15.3

> npm --version
6.4.1

> python --version
Python 3.7.3
```

Make sure the `bin` folder of Qt is set in Path variable, simply check it by following instruction:

```shell
> qmake --version
QMake version 3.1
Using Qt version 5.9.8 in D:/devtools/Qt/Qt5.9.8/5.9.8/msvc2017_64/lib
```

## Usage prerequisites

- [CANpie Server](https://github.com/canpie/CANpie) and the including can-* tools

## Building the Node.js module

Execute the shell script init-project.sh, which will do the required project setup.

```shell
> ./init-project.sh
```

For Windows use:m

```shell
> ./init-project.bat
```

### Doing the setup manually

If you don't execute the script file you have to:

- setup symbolic links for some QCan class files (refer to the script file)
- run the Meta-Object compiler (moc) to generare a *_moc.cpp file
- install the node-addon-api

In advance of the compilation process the Node-Addon-API must be installed:

```shell
> npm install node-addon-api
```

This command will create a directory `node_modules` inside the `source/nodejs` directory.

## Build the module

Command for building

```shell
> npm run build
```

## Examples

This node provides a ./canpie.js file, with some definitions that make usage of CANpie easier.
In the root folder there are also *example.js files which provides some simple examples of CANpie usage.

### Send a CAN message

The ./cp-send_example.js provides an example of simple, strait forward implementation to send a CAN message:

```javascript
const CANpie = require('./canpie');

function sendCanMessage() {
    const cpSocket = new CANpie.Socket();
    const cpFrame  = new CANpie.Frame();
    const buffer   = new ArrayBuffer(8);
    const canData  = new Uint8Array(buffer);

    if (cpSocket.connect(CANpie.CAN_CHANNEL_1, 100) === true)
    {
        console.log("Send a CBFF CAN message with Identifier 127h, DLC 5 and data [11 22 03 04 05] ...");

        canData.set([0x11,0x22,3,4,5,6,7,8], 0);
        cpFrame.setFrameFormat(CANpie.MSG_FORMAT_CBFF);
        cpFrame.setIdentifier(0x127);
        cpFrame.setDlc(5);
        cpFrame.setData(canData);
        cpSocket.write(cpFrame);
        cpSocket.disconnect();

    } else {
        console.log("The connection to the CANpie server could not be established.");
    }
}

sendCanMessage();
```

Make sure that the CANpie Server and if necessary a tool to display the CAN messages, how the can-dump runs. Then you can trigger the sending of a CAN message from the Node.js with the following instruction:

```shell
> node cp-send_example.js
Send a CBFF CAN message with Identifier 127h, DLC 5 and data [11 22 03 04 05] ...
```

### Receive a CAN message

tbd ...

## Pending clarification

Currently, the `canpie` package can be created under MacOS and Windows. The tests or even simple test scripts in the root directory can then use `canpie` with require('../canpie'); or require('./canpie');. However, this is not the procedure you choose to create a new Node.js package based on `canpie`.

Typically, you create a new package in an empty directory with `npm init` and install the necessary packages with `npm install <packagename>`. The `<packetname>` can be relative path to an existing package such as `canpie`.

If you choose exactly these method, then creating the `canpie` in a new package will fail, at least under Windows.

The workaround is to copy the contents of the previously created `canpie` into the `node_modules` directory of the new package and use it with `require('canpie');` in a Node.js file.

By default there are three ways to add a packages:

1. from a npm repository (there may be private or custom npm repositories)
1. from an URL tha downloads a tarball, that is a .tar.gz file. For example  GitHub or GitLab repositories.
1. from a local file system using a URL like this file:../../path/to/dir

So it is obvious that like here a strategy or a red thread is needed to distribute your own Node.js packages and use them among each other.

**Further information on this topic**

- https://docs.npmjs.com/cli/pack.html
- https://docs.npmjs.com/cli/publish
- https://docs.npmjs.com/creating-and-publishing-private-packages
- https://zellwk.com/blog/publish-to-npm/
- https://hackernoon.com/publish-your-own-npm-package-946b19df577e
