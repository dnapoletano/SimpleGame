add_library(
        shaders
        OBJECT
        textured.metal
        skybox.metal
)

set_target_properties(shaders PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(
        shaders
        PRIVATE
        device
        constant
        threadgroup
        vertex
        fragment
        sampler
)

target_include_directories(
        shaders
        PRIVATE
        /Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/System/Library/Frameworks/MetalKit.framework/Headers
)