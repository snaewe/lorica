#!/bin/bash -p

echo "Uninstalling Lorica"

# Check if sctipt is run with administator rights
if [ $EUID -ne 0 ]; then
   echo "Script is not running with administrator rights"
   exit 1
fi

# Check if lorica is installed
if [ ! -e "/Library/Receipts/Install Lorica.pkg" ]; then
    echo "Lorica is not installed"
    exit 2
fi

# Stop deamon
deamon="/Library/LaunchDaemons/com.42tools.lorica.plist"
if [ -e $deamon ]; then
    launchctl unload $deamon 
fi

# Remove files installed by Lorica
BOM_FILES="$(lsbom -s -f /Library/Receipts/Install\ Lorica.pkg/Contents/Archive.bom)"
BOM_FILES="$BOM_FILES ./private/var/run/lorica.pid ./private/var/db/lorica/lorica.ior ./private/var/db/lorica/ifr.ior ./private/var/db/lorica/ifr.cache"

BOM_DIRS="$(lsbom -s -d /Library/Receipts/Install\ Lorica.pkg/Contents/Archive.bom)"
BOM_DIRS="$BOM_DIRS ./private/var/db/lorica"

for file in $BOM_FILES; do
    file=${file:1}

    if [ -e $file ] && [ -f $file ]; then
        echo "Removing file $file"
        rm $file
    fi
done

finished="unset"
until [ "$finished" == "true" ]; do
    finished="true"

    for dir in $BOM_DIRS; do
        dir=${dir:1}

        if [ -z $dir ]; then
            continue
        fi

        if [ -e $dir ] && [ -d $dir ]; then
            if [ ! "$(ls -A $dir)" ]; then
                echo "Removing dir $dir"
                rmdir $dir
                finished="false"
            fi
        fi
    done
done

# Remove Installer receipt
rm -rf "/Library/Receipts/Install Lorica.pkg"
exit 0
