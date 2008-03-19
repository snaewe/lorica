#!/bin/bash

#  
#  Release script for Lorica.
#  Copyright (C) 2008 OMC Denmark ApS
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, 
#  MA 02111-1307 USA

prepare ()
{
    make distclean
    ./bootstrap --enable-tao-build
}

if [ $# -eq 0 ]; then
    action=SOURCE
else
    action="$1"
fi

product=lorica

if [ "$action" = "windows" ]; then
    scp -i $HOME/.ssh/no_passphrase_id_rsa $HOME/work/vmware-shared-folder/"$product"-install.exe colding@42tools.com:
    exit 0
fi

prepare

case "$action" in
    Mac%20OS%20X%20Tiger)
	make dist-dmg
	;;
    Mac%20OS%20X%20Leopard)
	make dist-dmg
	;;
    Red%20Hat%20Enterprise%20Linux%204)
	make dist-rpm
	;;
    Fedora%20Core%204)
	make dist-rpm
	;;
    Fedora%20Core%205)
	make dist-rpm
	;;
    Fedora%20Core%206)
	make dist-rpm
	;;
    Fedora%207)
	make dist-rpm
	;;
    Fedora%208)
	make dist-rpm
	;;
    Rawhide)
	make dist-rpm
	;;
    OpenSUSE%2010.2)
	make dist-rpm
	echo -n "Now cd to $(HOME)/suse_build/ and execute build as root in another terminal. I'll wait right here... "
	read answer
	;;
    OpenSUSE%2010.3)
	make dist-rpm
	echo -n "Now cd to $(HOME)/suse_build/ and execute build as root in another terminal. I'll wait right here... "
	read answer
	;;
    Gentoo)
	make dist-ebuild
	;;
    Ubuntu*)
	codename=`lsb_release -sc`
	case "$codename" in
	    "edgy")
		codename="Edgy"
		;;
	    "feisty")
		codename="Feisty"
		;;
	    "gutsy")
		codename="Gutsy"
		;;
	    *)
		echo "Unsupported Ubuntu release"
		exit 1
		;;
	esac
	make dist-deb-bin
	;;
    tarball)
	make
	make dist
	;;
    SOURCE)
	make
	make dist
	;;
    *)
	echo "Error - Unknown action"
	exit 1
	;;
esac


if [ $# -ge 2 ]; then
    exit 0
fi

case "$action" in
    Mac%20OS%20X%20Tiger)
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r packagemaker/Lorica.dmg colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Mac\ OS\ X\ Tiger/'
	;;
    Mac%20OS%20X%20Leopard)
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r packagemaker/Lorica.dmg colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Mac\ OS\ X\ Leopard/'
	;;
    Red%20Hat%20Enterprise%20Linux%204)
	cd $HOME/rpmbuild
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Red\ Hat\ Enterprise\ Linux\ 4/'
	;;
    Fedora%20Core%204)
	cd $HOME/rpmbuild
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Fedora\ Core\ 4/'
	;;
    Fedora%20Core%205)
	cd $HOME/rpmbuild
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Fedora\ Core\ 5/'
	;;
    Fedora%20Core%206)
	cd $HOME/rpmbuild
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Fedora\ Core\ 6/'
	;;
    Fedora%207)
	cd $HOME/rpmbuild
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Fedora\ 7/'
	;;
    Fedora%208)
	cd $HOME/rpmbuild
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Fedora\ 8/'
	;;
    Rawhide)
	cd $HOME/rpmbuild
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Rawhide'
	;;
    OpenSUSE%2010.2)
	cd /var/tmp/build-root/usr/src/packages
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/OpenSUSE\ 10.2/'
	;;
    OpenSUSE%2010.3)
	cd /var/tmp/build-root/usr/src/packages
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/OpenSUSE\ 10.3/'
	;;
    Gentoo)
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r ebuild/brutus-keyring-*.ebuild ebuild/ChangeLog ebuild/metadata.xml colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Gentoo/'
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r *.tar.gz colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Gentoo/'
	;;
    Ubuntu*)
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r deb/*.deb colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Ubuntu\ '"$codename/"
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r *.tar.gz colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Ubuntu\ '"$codename/"
	;;
    tarball)
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r *.tar.gz colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/SOURCES/'
	;;
    *)
	echo "Error - Unknown action"
	exit 1
	;;
esac

exit 0
