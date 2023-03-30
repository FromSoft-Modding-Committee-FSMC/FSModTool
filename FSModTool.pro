QT += 3danimation 3dcore 3dinput 3dlogic 3drender  core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += 

# Use the GitHub Actions run ID
RUNID = $$(GITHUB_RUN_NUMBER)

!isEmpty(RUNID) {
    DEFINES += "RUNID=\\\"$$RUNID\\\""
}
isEmpty(RUNID) {
    DEFINES += "RUNID=\\\"DEVELOPMENT\\\""
}

# Set C++ to latest standard
CONFIG += c++latest

RC_ICONS = KFModTool.ico

DEFINES += QT_DEPRECATED_WARNINGS

# Inherit system CFLAGS/CXXFLAGS into qmake
QMAKE_CFLAGS += $$(CFLAGS) -isystem $$[QT_INSTALL_HEADERS]
QMAKE_CXXFLAGS += $$(CXXFLAGS) -isystem $$[QT_INSTALL_HEADERS]

# Supress warnings for Qt stuff
QMAKE_CFLAGS += -isystem "$$[QT_INSTALL_HEADERS]/QtWidgets" \
    -isystem "$$[QT_INSTALL_HEADERS]/QtXml" \
    -isystem "$$[QT_INSTALL_HEADERS]/QtGui" \
    -isystem "$$[QT_INSTALL_HEADERS]/QtCore"

QMAKE_CXXFLAGS += -isystem "$$[QT_INSTALL_HEADERS]/QtWidgets" \
    -isystem "$$[QT_INSTALL_HEADERS]/QtXml" \
    -isystem "$$[QT_INSTALL_HEADERS]/QtGui" \
    -isystem "$$[QT_INSTALL_HEADERS]/QtCore"

HEADERS += \
    formats/kf2/mo.h \
    libimagequant/blur.h \
    libimagequant/kmeans.h \
    libimagequant/libimagequant.h \
    libimagequant/mediancut.h \
    libimagequant/mempool.h \
    libimagequant/nearest.h \
    libimagequant/pam.h \
    libimagequant/remap.h \
    aboutdialog.h \
    core/icons.h \
    core/kfmtcore.h \
    core/kfmterror.h \
    core/kfmtfile.h \
    core/prettynames.h \
    datahandlers/gameexe.h \
    datahandlers/kfmtdatahandler.h \
    datahandlers/map.h \
    datahandlers/model.h \
    datahandlers/soundbank.h \
    datahandlers/texturedb.h \
    datahandlers/tileseticons.h \
    editors/kf2/kf2_exeeditor.h \
    editors/kfmteditor.h \
    editors/mapeditwidget.h \
    editors/modelviewerwidget.h \
    editors/simpletableeditor.h \
    editors/texturedbviewer.h \
    editors/subwidgets/mapviewer.h \
    editors/subwidgets/mapviewer3d.h \
    editors/subwidgets/modelglview.h \
    formats/ps1/seq.h \
    formats/ps1/tim.h \
    formats/ps1/tmd.h \
    formats/ps1/vab.h \
    mainwindow.h \
    models/kf1/kf1_levelcurvetablemodel.h \
    models/kf2/kf2_armourparamstablemodel.h \
    models/kf2/kf2_levelcurvetablemodel.h \
    models/kf2/kf2_magicparamstablemodel.h \
    models/kf2/kf2_objectclassestablemodel.h \
    models/kf2/kf2_soundeffectparamstablemodel.h \
    models/kf2/kf2_weaponparamstablemodel.h \
    models/entityclasslistmodel.h \
    models/entityclasstablemodel.h \
    models/entityinstancelistmodel.h \
    models/entityinstancetablemodel.h \
    models/entitystatetablemodel.h \
    models/filelistmodel.h \
    models/modelanimationlistmodel.h \
    models/modelobjecttablemodel.h \
    models/objectinstancelistmodel.h \
    models/objectinstancetablemodel.h \
    models/shoplistmodel.h \
    models/shoptablemodel.h \
    models/stringtablemodels.h \
    models/texturelistmodel.h \
    models/tilecontentslistmodel.h \
    models/vfxinstancetablemodel.h \
    types/kf1/levelcurveentry.h \
    types/kf2/armourparams.h \
    types/kf2/entity.h \
    types/kf2/levelcurveentry.h \
    types/kf2/magic.h \
    types/kf2/object.h \
    types/kf2/sfx.h \
    types/kf2/text.h \
    types/kf2/tile.h \
    types/kf2/vfx.h \
    types/kf2/weaponparams.h \
    types/ps1/fixedp.h \
    types/ps1/libgte.h \
    utilities.h
    
SOURCES += \
    libimagequant/blur.c \
    libimagequant/kmeans.c \
    libimagequant/libimagequant.c \
    libimagequant/mediancut.c \
    libimagequant/mempool.c \
    libimagequant/nearest.c \
    libimagequant/pam.c \
    libimagequant/remap.c \
    aboutdialog.cpp \
    core/icons.cpp \
    core/kfmtcore.cpp \
    core/kfmterror.cpp \
    core/kfmtfile.cpp \
    core/prettynames.cpp \
    datahandlers/gameexe.cpp \
    datahandlers/map.cpp \
    datahandlers/model.cpp \
    datahandlers/soundbank.cpp \
    datahandlers/texturedb.cpp \
    datahandlers/tileseticons.cpp \
    editors/kf2/kf2_exeeditor.cpp \
    editors/mapeditwidget.cpp \
    editors/modelviewerwidget.cpp \
    editors/texturedbviewer.cpp \
    editors/subwidgets/mapviewer.cpp \
    editors/subwidgets/mapviewer3d.cpp \
    editors/subwidgets/modelglview.cpp \
    formats/ps1/tmd.cpp \
    main.cpp \
    mainwindow.cpp \
    models/kf1/kf1_levelcurvetablemodel.cpp \
    models/kf2/kf2_armourparamstablemodel.cpp \
    models/kf2/kf2_levelcurvetablemodel.cpp \
    models/kf2/kf2_magicparamstablemodel.cpp \
    models/kf2/kf2_objectclassestablemodel.cpp \
    models/kf2/kf2_soundeffectparamstablemodel.cpp \
    models/kf2/kf2_weaponparamstablemodel.cpp \
    models/entityclasslistmodel.cpp \
    models/entityclasstablemodel.cpp \
    models/entityinstancelistmodel.cpp \
    models/entityinstancetablemodel.cpp \
    models/entitystatetablemodel.cpp \
    models/filelistmodel.cpp \
    models/modelanimationlistmodel.cpp \
    models/modelobjecttablemodel.cpp \
    models/objectinstancelistmodel.cpp \
    models/objectinstancetablemodel.cpp \
    models/shoplistmodel.cpp \
    models/shoptablemodel.cpp \
    models/stringtablemodels.cpp \
    models/texturelistmodel.cpp \
    models/tilecontentslistmodel.cpp \
    models/vfxinstancetablemodel.cpp \
    types/kf2/entity.cpp \
    types/kf2/magic.cpp \
    types/kf2/object.cpp \
    types/kf2/sfx.cpp \
    types/kf2/text.cpp \
    types/kf2/weaponparams.cpp \
    utilities.cpp

FORMS += \
    aboutdialog.ui \
    editors/mapeditwidget.ui \
    editors/modelviewerwidget.ui \
    editors/simpletableeditor.ui \
    editors/texturedbviewer.ui \
    editors/kf2/kf2_exeeditor.ui \
    mainwindow.ui
    
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
	resources.qrc

DISTFILES += \
    litCommon.frag \
    litMime.vert \
    litStatic.vert \
    unlitSimple.frag \
    unlitSimple.vert
