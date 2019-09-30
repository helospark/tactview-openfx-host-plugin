g++ -DDEBUG=true -w *.cpp -ldl -g
g++ -w -shared -fPIC -Wl,-soname,libopenfxplugin.so -o libopenfxplugin.so *.cpp -ldl -g
cp libopenfxplugin.so ../src/main/resources/.
