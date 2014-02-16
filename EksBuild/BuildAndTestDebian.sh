#! /bin/bash

qbs detect-toolchains
qbs config --list profiles
qbs $EKS_BUILD_VARIANT

