#! /bin/bash

qbs setup-toolchains --detect
qbs setup-qt /usr/bin/qmake qt
qbs config profiles.qt.baseProfile clang
qbs profile:qt $EKS_BUILD_VARIANT


