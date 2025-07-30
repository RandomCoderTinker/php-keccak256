dnl config.m4 for keccak extension

PHP_ARG_ENABLE(keccak, whether to enable keccak extension,
[ --enable-keccak   Enable keccak extension])

if test "$PHP_KECCAK" != "no"; then
  PHP_NEW_EXTENSION(keccak, keccak256.c, $ext_shared)
fi
