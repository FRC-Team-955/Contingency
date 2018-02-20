ROBOT_IP=roborio-955-frc.local
ROBOT_USER=lvuser
./gradlew build
ssh $ROBOT_USER@$ROBOT_IP rm -v frcUserProgram
scp build/exe/frcUserProgram/frcUserProgram $ROBOT_USER@$ROBOT_IP:/home/lvuser/
ssh $ROBOT_USER@$ROBOT_IP /usr/local/frc/bin/frcKillRobot.sh -t
