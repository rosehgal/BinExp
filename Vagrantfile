Vagrant.configure("2") do |config|
    config.vm.box = "ubuntu/xenial64"
    
    config.vm.provision "shell", inline: <<-SHELL
    dpkg --add-architecture i386
    cp /etc/apt/sources.list /etc/apt/sources.list.old
    sed -i -e 's/archive\.ubuntu\.com/mirror\.0x\.sg/g' /etc/apt/sources.list
    apt-get update
    apt-get install -y wget libc6:i386 libncurses5:i386 libstdc++6:i386 gdb python python-pip libssl-dev gcc git binutils socat apt-transport-https ca-certificates libc6-dev-i386 python-capstone libffi-dev
    pip install --upgrade pip
    pip install ropgadget
    pip install pwntools
    pip install ipython
    pip install ropper

    wget -q -O- https://github.com/hugsy/gef/raw/master/gef.sh | sh

    git clone https://github.com/niklasb/libc-database.git /home/ubuntu/libc-database
    cd /home/ubuntu/libc-database
    /home/ubuntu/libc-database/add /lib/i386-linux-gnu/libc.so.6
    /home/ubuntu/libc-database/add /lib/x86_64-linux-gnu/libc.so.6

    apt-get update
    SHELL
end 
