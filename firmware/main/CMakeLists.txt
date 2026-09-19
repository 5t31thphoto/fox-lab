idf_component_register(
    SRCS
        "main.cpp"
        "fox_face.cpp"
        "fox_audio.cpp"
        "echobase/M5EchoBase.cpp"
        "echobase/es8311.cpp"
    INCLUDE_DIRS "." "echobase"
)
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-error=unused-function
    -Wno-error=unused-variable
    -Wno-error=unused-but-set-variable
    -Wno-error=sign-compare
    -Wno-error=type-limits
    -Wno-error=missing-field-initializers
)
