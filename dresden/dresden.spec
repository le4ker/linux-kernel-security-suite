%{!?dist: %define dist .ai6}
%define debug_package %{nil}

Name:		dresden
Version:	1.0
Release:	1%{?dist}

Summary:	Kernel module for blocking and logging kernel module insertion attempts
Group:		System Environment/Kernel
License:	GPLv2+
URL:		http://github.com/CERN-CERT/dresden
Vendor:		CERN, http://cern.ch/linux
BuildRoot: 	%{_tmppath}/%{name}-%{version}-buildroot
BuildRequires:	sed, redhat-rpm-config
BuildRequires:	%kernel_module_package_buildreqs
ExclusiveArch:	i686 x86_64

Source0:	%{name}/%{name}-%{version}.tar.gz
Source1:	%{name}.files
Source2:	%{name}.conf
Source3:	%{name}.modules

# Uncomment to build "debug" packages
#kernel_module_package -f %{SOURCE1} default debug

# Build only for standard kernel variant(s)
%kernel_module_package -f %{SOURCE1} default

%description
%{name} is a kernel module that will log emergency messages for every attempt of inserting or removing a kernel module.
Also, it will block the insertion of every kernel module, by replacing the init functio of the (wannabe) incoming module
with a function crafted for failure. After deploying dresden, you will not be able to remove it. It will delete itself from
the list of the loaded kernel modules.


%prep
%setup -q
set -- *
mkdir source
mv "$@" source/
mkdir obj

%build
for flavor in %flavors_to_build ; do
	rm -rf obj/$flavor
	cp -r source obj/$flavor

	# update symvers file if existing
	symvers=source/Module.symvers-%{_target_cpu}
	if [ -e $symvers ]; then
		cp $symvers obj/$flavor/Module.symvers
	fi

	make -C %{kernel_source $flavor} M=$PWD/obj/$flavor/src
done


%install
rm -rf $RPM_BUILD_ROOT
export INSTALL_MOD_PATH=$RPM_BUILD_ROOT
export INSTALL_MOD_DIR=extra/%{name}
for flavor in %flavors_to_build ; do 
	 make -C %{kernel_source $flavor} modules_install \
	 M=$PWD/obj/$flavor/src
	# Cleanup unnecessary kernel-generated module dependency files.
	find $INSTALL_MOD_PATH/lib/modules -iname 'modules.*' -exec rm {} \;
done

install -m644 -D %{SOURCE2} $RPM_BUILD_ROOT/etc/depmod.d/%{name}.conf

#Load at boot time

mkdir -p ${RPM_BUILD_ROOT}/etc/sysconfig/modules/
install -m0755 %{SOURCE3} ${RPM_BUILD_ROOT}/etc/sysconfig/modules/

%post

${RPM_BUILD_ROOT}/etc/sysconfig/modules/%{name}.modules

%clean
rm -rf $RPM_BUILD_ROOT

%changelog
* Thu Sep 26 2012 Antonio Perez Perez <antonio.perez.perez@cern.ch> - 1.0-1
- Added support for loading the module as the last one in the system

* Thu Sep 26 2012 Antonio Perez Perez <antonio.perez.perez@cern.ch> - 1.0
- Initial packaging
