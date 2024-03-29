# =================================================================================================
# Copyright (C) 2018 GRAPE Contributors Copyright (C) 2023-2024 HEPHAESTUS Contributors
# =================================================================================================

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${HEPHAESTUS_VERSION})
set(CPACK_PACKAGE_VENDOR "Filippo Brizzi")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_VENDOR})
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

set(CPACK_PACKAGE_DIRECTORY ${PROJECT_BINARY_DIR}/package)

set(CPACK_SET_DESTDIR ON)
set(CPACK_INSTALL_PREFIX "/opt/${PROJECT_NAME}")

include(CPack)
