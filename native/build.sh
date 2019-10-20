#-DDEBUG_LOGGING=true
rm -r target || true
mkdir target
g++ -DDEBUG=true -w *.cpp -ldl -g
g++ -w -shared -fPIC -Wl,-soname,libopenfxplugin.so -o libopenfxplugin.so *.cpp -ldl -g
