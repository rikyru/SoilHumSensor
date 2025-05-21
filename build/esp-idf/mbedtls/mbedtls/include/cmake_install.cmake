# Install script for directory: C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/SoilHumSensor")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Compiler/Espressif/tools/riscv32-esp-elf/esp-13.2.0_20240530/riscv32-esp-elf/bin/riscv32-esp-elf-objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mbedtls" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/aes.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/aria.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/asn1.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/asn1write.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/base64.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/bignum.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/block_cipher.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/build_info.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/camellia.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ccm.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/chacha20.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/chachapoly.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/check_config.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/cipher.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/cmac.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/compat-2.x.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_legacy_crypto.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_legacy_from_psa.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_psa_from_legacy.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_psa_superset_legacy.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_ssl.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_adjust_x509.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/config_psa.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/constant_time.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ctr_drbg.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/debug.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/des.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/dhm.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecdh.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecdsa.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecjpake.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ecp.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/entropy.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/error.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/gcm.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/hkdf.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/hmac_drbg.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/lms.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/mbedtls_config.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/md.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/md5.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/memory_buffer_alloc.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/net_sockets.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/nist_kw.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/oid.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pem.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pk.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs12.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs5.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/pkcs7.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform_time.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/platform_util.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/poly1305.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/private_access.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/psa_util.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ripemd160.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/rsa.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha1.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha256.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha3.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/sha512.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_cache.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_ciphersuites.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_cookie.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/ssl_ticket.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/threading.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/timing.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/version.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_crl.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_crt.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/mbedtls/x509_csr.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/psa" TYPE FILE PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ FILES
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/build_info.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_adjust_auto_enabled.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_adjust_config_key_pair_types.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_adjust_config_synonyms.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_builtin_composites.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_builtin_key_derivation.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_builtin_primitives.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_compat.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_config.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_common.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_composites.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_key_derivation.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_driver_contexts_primitives.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_extra.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_legacy.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_platform.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_se_driver.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_sizes.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_struct.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_types.h"
    "C:/Compiler/Espressif/frameworks/esp-idf-v5.3.1/v5.3.1/esp-idf/components/mbedtls/mbedtls/include/psa/crypto_values.h"
    )
endif()

