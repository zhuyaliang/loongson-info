Name:          loongson-info
Version:       1.0.1
Release:       1%{?dist}
Summary:       Loongson (4000/5000) cpu info tool
License:       GPLv3+ 
URL:           http://gitlab.gen.cn/software/loongson-info

Source0:       %url/archive/%{version}/%{name}-%{version}.tar.gz

BuildRequires: gcc
BuildRequires: git
BuildRequires: gtk3-devel
BuildRequires: meson
BuildRequires: polkit-devel
BuildRequires: glib2-devel
BuildRequires: libgtop2-devel
BuildRequires: systemd
BuildRequires: systemd-devel
BuildRequires: gobject-introspection-devel
BuildRequires: gtk-doc
BuildRequires: gettext-devel
BuildRequires: pkgconfig(dbus-1)

Requires: dmidecode
%description
Loongson(4000/5000) cpu info tool

%package libs
Summary: Client-side library to talk to loongson-info
Requires: %{name} = %{version}-%{release}

%description libs
The loongson-info-libs package contains a library that can
be used by applications that want to interact with the loongson-info
daemon.

%package devel
Summary: Development files for loongson-info-libs
Requires: %{name}-libs = %{version}-%{release}

%description devel
The loongson-info-devel package contains headers and other
files needed to build applications that use loongson-info-libs.


%prep
%autosetup -S git 

%build
%meson
%meson_build

%install
%meson_install
mkdir -p $RPM_BUILD_ROOT%{_datadir}/loongson-info/interfaces/

%find_lang %{name}

%ldconfig_scriptlets libs

%post
%systemd_post loongson-info-daemon.service

%preun
%systemd_preun loongson-info-daemon.service

%postun
%systemd_postun loongson-info-daemon.service


%files -f %{name}.lang
%license COPYING
%doc README.md
%{_bindir}/loongson-info
%{_datadir}/applications/loongson-info.desktop
%{_datadir}/metainfo/loongson-info.appdata.xml
%{_datadir}/loongson-info/icons/*.png
%{_datadir}/icons/hicolor/*/apps/loongson*.png
%{_sysconfdir}/dbus-1/system.d/cn.loongson.info.conf
%{_libexecdir}/loongson-info/loongson-infod
%dir %{_datadir}/loongson-info/
%dir %{_datadir}/loongson-info/interfaces/
%{_datadir}/dbus-1/interfaces/cn.loongson.info.xml
%{_datadir}/dbus-1/system-services/cn.loongson.info.service
%{_unitdir}/loongson-info-daemon.service

%files libs
%{_libdir}/libloongson-info.so.*

%files devel
%{_includedir}/loongson-info-1.0
%{_libdir}/libloongson-info.so
%{_libdir}/pkgconfig/loongson-info.pc
%dir %{_datadir}/gtk-doc/html/libinfo
%{_datadir}/gtk-doc/html/libinfo/*


%changelog
* Mon Aug 08 2022 zhuyaliang <zhuyaliang@loongson.cn> - 1.0.1-1
- initial build for loongson

