TEMPLATE = subdirs

CONFIG += ordered minimal_eks

SUBDIRS = EksCore/QtProject/EksCore.pro \
          Eks3D/QtProject/Eks3D.pro \
          EksScript/EksScript.pro \
          EksDebug \
          EksDebugger \
          EksReflex \
          EksReflex\test\EksReflexTest.pro


!minimal_eks {
  SUBDIRS += EksGui/QtProject/EksGui.pro
}
