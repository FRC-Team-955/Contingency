./gradlew clion
rm compile_commands.json
echo 'SET( CMAKE_EXPORT_COMPILE_COMMANDS 1 )' >> CMakeLists.txt
mkdir fakebuild
pushd fakebuild
cmake .. 
cp compile_commands.json ../
popd
rm -rf fakebuild
rm CMakeLists.txt
