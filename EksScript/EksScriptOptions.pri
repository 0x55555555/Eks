win32 {
  debug:LIBS += -L$$ROOT/EksScript/v8/debuglib/
  release:LIBS += -L$$ROOT/EksScript/v8/releaselib/

  LIBS += -lv8
}
