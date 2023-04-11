qt_add_library(libcontroller STATIC)

set_target_properties(libcontroller PROPERTIES AUTOMOC ON)

corrosion_import_crate(MANIFEST_PATH rust/Cargo.toml)
corrosion_add_cxxbridge(
        librustdisboard
        CRATE disboard-rust
        MANIFEST_PATH rust
        FILES lib.rs
)

target_link_libraries(libcontroller PRIVATE Qt6::Quick librustdisboard)

qt_add_qml_module(libcontroller
        URI disboard.impl.controller
        VERSION 1.0
        RESOURCE_PREFIX /imports
        SOURCES
        square.h
        piece.cpp
        piece.h
        move.cpp
        move.h
        disboard.cpp
        disboard.h
        movelistmodel.cpp
        movelistmodel.h
        controller.cpp
        controller.h
        )
