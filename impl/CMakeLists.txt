qt_add_library(disboardImpl STATIC)

add_subdirectory(assets)

set_target_properties(disboardImpl PROPERTIES AUTOMOC ON)

add_subdirectory(controller)

target_link_libraries(disboardImpl PRIVATE Qt6::Quick libcontrollerplugin)

qt_add_qml_module(disboardImpl
        URI disboard.impl
        VERSION 1.0
        RESOURCE_PREFIX /imports

        QML_FILES
        qml/Square.qml
        qml/BasePiece.qml

        qml/HighlightRect.qml
        qml/HoverRect.qml
        qml/HintRect.qml
        qml/CaptureHintRect.qml

        qml/Piece.qml
        qml/PhantomPiece.qml

        qml/PromotionWindow.qml
        qml/DragArea.qml
        qml/BoardBackground.qml

        qml/BoardImpl.qml

        js/BoardView.mjs
        )
