#!/bin/bash

#  
#  Release script for Lorica.
#  Copyright (C) 2007-2009 OMC Denmark ApS.
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Affero General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#  GNU Affero General Public License for more details.
#
#  You should have received a copy of the GNU Affero General Public License
#  along with this program. If not, see <http://www.gnu.org/licenses/>.

prepare ()
{
    make distclean
    ./bootstrap reconf --enable-tao-build --enable-lorica-debug
}

if [ $# -eq 0 ]; then
    action=SOURCE
else
    action="$1"
fi

product=lorica

if [ "$action" = "windows" ]; then
    scp -i $HOME/.ssh/no_passphrase_id_rsa "$HOME/Public/$product"-install.exe colding@42tools.com:
    echo ""
    echo "Download location:"
    echo "   <http://www.42tools.com/sites/default/files/downloads/dist/lorica/Windows/lorica-install.exe>"
    echo ""
    exit 0
fi

prepare

case "$action" in
    Mac%20OS%20X*)
	make dist-dmg || exit 1
	;;
    Red%20Hat%20Enterprise%20Linux%204)
	make dist-rpm || exit 1
	;;
    CentOS*)
	make dist-rpm || exit 1
	;;
    Fedora*)
	make dist-rpm || exit 1
	;;
    Rawhide)
	make dist-rpm || exit 1
	;;
    OpenSUSE*)
	make dist-rpm || exit 1
	echo -n "Now cd to $(HOME)/suse_build/ and execute build as root in another terminal. I'll wait right here... "
	read answer
	;;
    Gentoo)
	make dist-ebuild || exit 1
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
	    "hardy")
		codename="Hardy"
		;;
            "intrepid")
                codename="Intrepid"
                ;;
            "jaunty")
                codename="Jaunty"
                ;;
	    *)
		echo "Unsupported Ubuntu release"
		exit 1
		;;
	esac
	make dist-deb-bin || exit 1
	;;
    tarball)
	make || exit 1
	make dist || exit 1
	;;
    SOURCE)
	make || exit 1
	make dist || exit 1
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
    Mac%20OS%20X*)
	scp -i $HOME/.ssh/no_passphrase_id_rsa apple/Lorica.build/Lorica.dmg colding@42tools.com:
	;;
    Red%20Hat%20Enterprise%20Linux%204)
	cd $HOME/rpmbuild || exit 1
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Red\ Hat\ Enterprise\ Linux\ 4/'
	;;
    Fedora%20Core%204)
	cd $HOME/rpmbuild || exit 1
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Fedora\ Core\ 4/'
	;;
    Fedora%20Core%205)
	cd $HOME/rpmbuild || exit 1
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Fedora\ Core\ 5/'
	;;
    Fedora%20Core%206)
	cd $HOME/rpmbuild || exit 1
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Fedora\ Core\ 6/'
	;;
    Fedora%207)
	cd $HOME/rpmbuild || exit 1
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Fedora\ 7/'
	;;
    Fedora%208)
	cd $HOME/rpmbuild || exit 1
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Fedora\ 8/'
	;;
    Rawhide)
	cd $HOME/rpmbuild || exit 1
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES SPECS RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Rawhide'
	;;
    OpenSUSE%2010.2)
	cd /var/tmp/build-root/usr/src/packages || exit 1
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/OpenSUSE\ 10.2/'
	;;
    OpenSUSE%2010.3)
	cd /var/tmp/build-root/usr/src/packages || exit 1
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r SOURCES RPMS SRPMS colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/OpenSUSE\ 10.3/'
	;;
    Gentoo)
	scp -i $HOME/.ssh/no_passphrase_id_rsa -r ebuild/lorica-*.ebuild ebuild/ChangeLog ebuild/metadata.xml colding@42tools.com:'/var/www/omc/htdocs/sites/default/files/downloads/dist/'"$product"'/Gentoo/'
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
