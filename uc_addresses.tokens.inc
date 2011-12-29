<?php
/**
 * @file
 * Copy of functionality in token.module to allow to replace tokens
 * of uc_addresses two levels deep.
 */

/**
 * Implements hook_token_info().
 *
 * @return array
 */
function uc_addresses_token_info($type = 'all') {
  $types['uc_addresses'] = array(
    'name' => t('Ubercart Addresses'),
    'description' => t('Tokens related to individual addresses'),
    'needs-data' => 'uc_addresses',
  );

  $tokens = array();
  // Ubercart Addresses tokens
  if ($type == 'uc_addresses' || $type == 'all') {
    $tokens['uc_addresses'] = array();
    $address = UcAddressesAddress::newAddress();
    $fields = uc_addresses_get_address_field_handler_instances($address, 'token');
    foreach ($fields as $fieldname => $handler) {
      $tokens['uc_addresses'] = array_merge($tokens['uc_addresses'], $handler->getTokenInfo());
      /*
      // Check if handler supports multiple output formats
      $formats = $handler->getOutputFormats();
      if (count($formats) > 0) {
        foreach ($formats as $format => $label) {
          $tokens['uc_addresses'][$fieldname . ':' . $format] = array(
            'name' => $handler->getFieldTitle(),
            'description' => $label,
          );
        }
      }
      else {
        // Only a single format is supported
        $tokens['uc_addresses'][$fieldname] = array(
          'name' => $handler->getFieldTitle(),
          'description' => t('!name, formatted', array('!name' => $handler->getFieldTitle())),
        );
      }
      $tokens['uc_addresses'][$fieldname . '-raw'] = array(
        'name' => $handler->getFieldTitle(),
        'description' => t('!name, raw', array('!name' => $handler->getFieldTitle())),
      );
      */
    }
  }

  // Ubercart order tokens
  if ($type == 'order' || $type == 'all') {
    $tokens['order']['uc-addresses-shipping-address'] = array(
      'name' => t('The order shipping address, formatted with the Ubercart Addresses module.'),
    );
    $tokens['order']['uc-addresses-billing-address'] = array(
      'name' => t('The order billing address, formatted with the Ubercart Addresses module.'),
    );
  }

  // User tokens
  $tokens['user']['default-shipping-address'] = array(
    'name' => t('Default shipping address'),
    'description' => t('The default shipping address, formatted with the Ubercart Addresses module.'),
    'type' => 'uc_addresses',
  );
  $tokens['user']['default-billing-address'] = array(
    'name' => t('Default billing address'),
    'description' => t('The default billing address, formatted with the Ubercart Addresses module.'),
    'type' => 'uc_addresses',
  );
  /*
  if ($type == 'user' || $type == 'all') {
    $info = uc_addresses_token_info('uc_addresses');
    $address_tokens = $info['tokens'];
    foreach ($address_tokens['uc_addresses'] as $token => $description) {
      if (strpos($token, 'uc_addresses_default_') === FALSE) {
        $user_token = str_replace('uc_addresses', 'uc_addresses-shipping', $token);
        $tokens[t('Ubercart Addresses') . ' - ' . t('Default shipping address')][$user_token] = t('Default shipping address') . ': ' . $description;
        $user_token = str_replace('uc_addresses', 'uc_addresses-billing', $token);
        $tokens[t('Ubercart Addresses') . ' - ' . t('Default billing address')][$user_token] = t('Default billing address') . ': ' . $description;
      }
      $tokens[t('Ubercart Addresses') . ' - ' . t('Default shipping address')]['uc-addresses-shipping-address'] = t('The default shipping address, formatted with the Ubercart Addresses module.');
      $tokens[t('Ubercart Addresses') . ' - ' . t('Default billing address')]['uc-addresses-billing-address'] = t('The default billing address, formatted with the Ubercart Addresses module.');
    }
  }
  */

  return array(
    'types' => $types,
    'tokens' => $tokens,
  );
}

/**
 * Implements hook_tokens().
 *
 * @param string $type
 * @param array $tokens
 * @param array $data
 * @param array $options
 *
 * @return array
 *
 * @todo IMPLEMENT!
 */
function uc_addresses_tokens($type, $tokens, $data = array(), $options = array()) {
  $replacements = array();
  switch ($type) {
    case 'uc_addresses':
      $address = $data['uc_addresses'];
      if ($address instanceof UcAddressesAddress) {
        foreach ($tokens as $name => $original) {
          $handler = NULL;
          $fieldname = $name;
          $output_format = NULL;
          // Check if this token represents a existing address field
          if (UcAddressesSchemaAddress::fieldExists($name)) {
            $handler = uc_addresses_get_address_field_handler($address, $name, 'token');
          }
          else {
            // Check if this token represents a particular output format of an existing address field
            $token_pieces = explode(':', $name);
            if (count($token_pieces) == 2) {
              $fieldname = $token_pieces[0];
              $output_format = $token_pieces[1];
              if (UcAddressesSchemaAddress::fieldExists($fieldname)) {
                // Check if this field supports the specified output format.
                $handler = uc_addresses_get_address_field_handler($address, $fieldname, 'token');
              }
            }
          }
          if ($handler instanceof UcAddressesFieldHandler) {
            // Use the handler to output the right value.
            $replacements[$original] = $handler->outputValue($address->getField($fieldname), $output_format);
          }
        }
      }
      break;
  }
  return $replacements;
}

/**
 * Replace tokens using uc_addresses tokens
 *
 * @param string $format
 * @param UcAddressesAddress $address
 * @return string
 *
 * @todo REMOVE
 */
function uc_addresses_token_replace($text, UcAddressesAddress $address) {
  $type = 'uc_addresses';
  $object = $address;

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
