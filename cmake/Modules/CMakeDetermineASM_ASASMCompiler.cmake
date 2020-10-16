# support for asasm (RISC OS GCC SDK)
set(ASM_DIALECT "_ASASM")
# CMAKE_ASM${ASM_DIALECT}_COMPILER should get set in riscos.cmake
include(CMakeDetermineASMCompiler)
set(ASM_DIALECT)
