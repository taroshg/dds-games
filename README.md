# Fast DDS Games

### Latest update



### Instructions to build:
make build folder and cd into it
 ```
 mkdir build
 cd build
```
build all the files!
```
cmake ..
cmake --build .

```

### Instructions to build src:
create a idl file then run
```
~/FastDDS/src/fastddsgen/scripts/fastddsgen HelloWorld.idl
```

### Instructions to run
in one terminal
```
cd build 
.\DDSHelloWorldPublisher
```
in another terminal
```
cd build 
.\
```

### Mac OS build cmd
```
cmake .. -DCMAKE_INSTALL_PREFIX=~/Fast-DDS/install -DBUILD_SHARED_LIBS=ON
cmake --build .
```
