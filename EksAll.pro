TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = EksCore/QtProject/EksCore.pro \
          Eks3D/QtProject/Eks3D.pro \
          EksScript/EksScript.pro \
          EksDebug

# EksReflex\test\EksReflexTest.pro
# EksDebugger \
# EksReflex \



!minimal_eks {
  SUBDIRS += EksGui/QtProject/EksGui.pro
}
