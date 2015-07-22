#!/usr/bin/env bash
#cd `dirname $(readlink -f $0)`

#echo "check the size of works directory ...."
#size=$(du -m -s works | awk '{print $1}')
#if [[ $size -ge 50 ]]; then
#  echo "the total size( $size M) of the works is beyoung the limit( 50 M) "
#  echo "you can "
#  echo " 1. clean the tmp file during the build process or "
#  echo " 2. remove libarys and file those is no need any more or"
#  echo " 3. rework you code"
#  exit
#fi

#echo "archive the works director"
#rm -rf works.tar.gz
#tar czf works.tar.gz works/
#zipsize=$(du -m -s works.tar.gz | awk '{print $1}')
#if [[ $zipsize -ge 10 ]]; then
#  echo "the archive ($zipsize M) of the works is beyoung the limit( 10 M) "
#  echo "you can "
#  echo " 1. clean the tmp file during the build process or "
#  echo " 2. remove libarys and file those is no need any more or"
#  echo " 3. rework you code"
#  exit
#fi

#echo "you can use works.tar.gz for submit"
#echo ""

rm -rf  *.txt *.csv

echo "prepare for test.."
killall game >/dev/null 2>&1
killall gameserver >/dev/null 2>&1
#rm  -rf ../run_area/*
#cp -r server ../run_area
#cp -rf works.tar.gz ../run_area
#pushd . >/dev/null
#cd ../run_area
#rm -rf works
#tar zxf works.tar.gz  

echo "start server"
#pushd . >/dev/null
cd server
for i in 1 2 3 4 5 6 7 8
do 
  export "PLAYER"$i"_IP"=127.0.0.$i
  export "PLAYER"$i"_PORT"=600$i
  export "PLAYER"$i"_ID"=$i$i$i$i
done
export 
#nohup ./gameserver -gip 127.0.0.1 -seq replay -d 1 -m 10000 -b 50 -t 2000 -h 500  0</dev/null 1>/dev/null 2>/dev/null  & 
./gameserver -gip 127.0.0.1 -seq replay -r 30 -d 0 -m 4000 -b 20 -t 2000 -h 600 -i 500 0</dev/null 1>/dev/null 2>/dev/null  & 
#nohup ./gameserver -gip 127.0.0.1 -seq replay -d 1 -m 4000 -b 20 -t 2000 -h 600  0</dev/null 1>/dev/null 2>/dev/null  & 
#popd >/dev/null

echo "start your program"
#pushd . >/dev/null

#cd works/target


####################
echo "start playmates"
./game                    127.0.0.1 6000 127.0.0.1 6001 1111 0</dev/null 1>/dev/null 2>/dev/null &
./game_v1                  127.0.0.1 6000 127.0.0.2 6002 2222 0</dev/null 1>/dev/null 2>/dev/null &
./game_v1		          127.0.0.1 6000 127.0.0.3 6003 3333 0</dev/null 1>/dev/null 2>/dev/null &
./game_v2      127.0.0.1 6000 127.0.0.4 6004 4444 0</dev/null 1>/dev/null 2>/dev/null &
./game_v2                   127.0.0.1 6000 127.0.0.5 6005 5555 0</dev/null 1>/dev/null 2>/dev/null &
./check  127.0.0.1 6000 127.0.0.6 6006 6666 0</dev/null 1>/dev/null 2>/dev/null &
./check                  127.0.0.1 6000 127.0.0.7 6007 7777 0</dev/null 1>/dev/null 2>/dev/null &
./check                  127.0.0.1 6000 127.0.0.8 6008 8888 0</dev/null 1>/dev/null 2>/dev/null &

#./game  127.0.0.1 6000 127.0.0.1 6001 1111 0</dev/null 1>/dev/null 2>/dev/null &
#./game  127.0.0.1 6000 127.0.0.2 6002 2222 0</dev/null 1>/dev/null 2>/dev/null &
#./game	127.0.0.1 6000 127.0.0.3 6003 3333 0</dev/null 1>/dev/null 2>/dev/null &
#./game  127.0.0.1 6000 127.0.0.4 6004 4444 0</dev/null 1>/dev/null 2>/dev/null &
#./game  127.0.0.1 6000 127.0.0.5 6005 5555 0</dev/null 1>/dev/null 2>/dev/null &
#./game  127.0.0.1 6000 127.0.0.6 6006 6666 0</dev/null 1>/dev/null 2>/dev/null &
#./game  127.0.0.1 6000 127.0.0.7 6007 7777 0</dev/null 1>/dev/null 2>/dev/null &
#./game  127.0.0.1 6000 127.0.0.8 6008 8888 0</dev/null 1>/dev/null 2>/dev/null &


#####################
#for i in 1 2 3 4 5 6 7 8
##for i in 1 2 3 4 5 6 7
#do
#  nohup ./game 127.0.0.1 6000 127.0.0.$i 600$i $i$i$i$i  0</dev/null 1>/dev/null 2>/dev/null &
##./game 127.0.0.1 6000 127.0.0.1 6001
#done
##popd >/dev/null

#i=8
#  nohup ./game 127.0.0.1 6000 127.0.0.$i 600$i $i$i$i$i 0</dev/null 1>/dev/null 2>/dev/null &

#popd >/dev/null

