#!/bin/sh

module="cdriver"
device="luludriver"
lib_mod_dir="/lib/modules/`uname -r`"

cp ../$module.ko $lib_mod_dir/$module
cp ../modules.order $lib_mod_dir
depmod -a
modprobe $module

rm -f /dev/$device

major=$(awk '$2=="'$device'" {print $1}' /proc/devices)
mknod /dev/$device c $major 0

#
# test read / write azerty
#
echo "------write to device------"
cat  text.txt > /dev/$device
echo "------end write-------"

echo "------read fm device------"
result=$(cat /dev/$device)
echo "------end read------"
echo $result

if [ $result = "azerty" ]; 
then
    echo "test r/w : OK !!!!!"
else
    echo "test r/w : !!!!! NOK"
fi

#
# increase buffer size with IOCTL to 50 (fm 10)
# test final buffer_size
#

buffer_size=$(./test_ioctl /dev/$device 50)
if [ $buffer_size = 50 ]; then
    echo "buffer size OK!!!!"
else
    echo "buffer size = $buffer_size !!!! NOK"
fi

cat text2.txt > /dev/$device
result=$(cat /dev/$device)
#echo $result

in_text2=$(cat text2.txt)
if [ $result = $in_text2 ]; then
    echo "test r/w : OK !!!!!"
else
     echo "test r/w : !!!!! NOK"
fi


modprobe -r $module
#rmmod $module
