# boost_beast_client_server
boost::beast client and server sample
## build
```shell
cd server/
mkdir server_build
cd server_build/
cmake ..
make
cp sample_server ../../
cd ../..
cd client/
mkdir client_build
cd client_build/
cmake ..
make
cp sample_client ../../
cd ../..
```
## run
```shell
cp server.json $HOME
./server_sample
./server_client
```
