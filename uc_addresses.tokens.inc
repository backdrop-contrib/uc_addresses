<?php
/**
 * @file
 * Copy of functionality in token.module to allow to replace tokens
 * of uc_addresses two levels deep.
 */

/**
 * Replace tokens using uc_addresses tokens
 *
 * @param string $format
 * @param UcAddressesAddress $address
 * @return string
 */
function uc_addresses_token_replace($text, UcAddressesAddress $address) {
  $type = 'uc_addresses';
  $object = $address;
  $options = array();

  token_include();

  $id = _token_get_id($type, $object);
  if ($id && isset($tokens[$type][$id])) {
    $tmp_tokens = $tokens[$type][$id];
  }
  else {
    $tmp_tokens = module_invoke_all('token_values', $type, $object, $options);
    $tokens[$type][$id] = $tmp_tokens;
  }

  // Special-case global tokens, as we always want to be able to process
  // those substitutions.
  if (!isset($tokens['global']['default'])) {
    $tokens['global']['default'] = module_invoke_all('token_values', 'global');
  }

  $all = array_merge($tokens['global']['default'], $tokens[$type][$id]);

  $result = new stdClass();
  $result->tokens = array_keys($all);
  $result->values = array_values($all);

  $tokens = token_prepare_tokens($result->tokens);
  return str_replace($tokens, $result->values, $text);
}
