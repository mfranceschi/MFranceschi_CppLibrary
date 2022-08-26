include(CheckSymbolExists)
include(CheckCXXSymbolExists)

list(APPEND CMAKE_REQUIRED_DEFINITIONS -D__STDC_WANT_LIB_EXT1__=1)

# Note: we don't actually use MF_HAS_strerror,
# but I am using it as a safeguard to ensure that check_symbol_exists behaves as expected.
check_symbol_exists(strerror "string.h" MF_HAS_strerror)

check_symbol_exists(strerrordesc_np "string.h" MF_HAS_strerrordesc_np)

check_symbol_exists(strerror_r "string.h" MF_HAS_strerror_repeatable)

check_symbol_exists(strerror_s "string.h" "errno.h" MF_HAS_strerror_secure)

check_symbol_exists(strerror_l "string.h" "errno.h" "locale.h" MF_HAS_strerror_localized)

message("MF_HAS_strerror_localized=${MF_HAS_strerror_localized}")
