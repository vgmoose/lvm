%with udevdir %{_udevdir}
%enableif %{enable_cmirror} cmirrord
%enableif %{enable_udev} udev_sync
%enableif %{enable_profiling} profiling
%global enable_lvmetad %(if echo %{services} | grep -q lvmetad; then echo 1; else echo 0; fi)
%enableif %{enable_lvmetad} lvmetad

%build
%configure \
  --with-default-dm-run-dir=%{_default_dm_run_dir} \
  --with-default-run-dir=%{_default_run_dir} \
  --with-default-pid-dir=%{_default_pid_dir} \
  --with-default-locking-dir=%{_default_locking_dir} \
  --with-usrlibdir=%{_libdir} \
  --enable-lvm1_fallback \
  --enable-fsadm \
  --with-pool=internal \
  --with-user= \
  --with-group= \
  --with-device-uid=0 \
  --with-device-gid=6 \
  --with-device-mode=0660 \
  --enable-pkgconfig \
  --enable-applib \
  --enable-cmdlib \
  --enable-dmeventd \
  %{configure_flags}

make %{?_smp_mflags}
%{?extra_build_commands}

%install
make install DESTDIR=$RPM_BUILD_ROOT
make install_system_dirs DESTDIR=$RPM_BUILD_ROOT
make install_initscripts DESTDIR=$RPM_BUILD_ROOT
%if %{enable_systemd}
make install_systemd_units DESTDIR=$RPM_BUILD_ROOT
make install_tmpfiles_configuration DESTDIR=$RPM_BUILD_ROOT
%endif

# when building an src.rpm from freestanding specfiles
test -e %{_sourcedir}/source.inc || cp source.inc build.inc packages.inc macros.inc %{_sourcedir}

%check
%{?check_commands}

%clean
pwd
test -n rpm-no-clean || rm -rf $RPM_BUILD_ROOT
