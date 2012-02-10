<?php
/**
 * @file
 * Hook implementations for token module.
 */

/**
 * Implements hook_token_info().
 *
 * @return array
 */
function uc_addresses_token_info() {
  $types['uc_addresses'] = array(
    'name' => t('Ubercart Addresses'),
    'description' => t('Tokens related to individual addresses'),
    'needs-data' => 'uc_addresses',
  );

  $tokens = array();
  // Ubercart Addresses tokens
  $tokens['uc_addresses'] = array();
  $address = UcAddressesAddress::newAddress();
  $fields = uc_addresses_get_address_field_handler_instances($address, 'token');
  foreach ($fields as $fieldname => $handler) {
    $tokens['uc_addresses'] = array_merge($tokens['uc_addresses'], $handler->getTokenInfo());
  }

  // Ubercart order tokens
  $tokens['uc_order']['uc-addresses-shipping-address'] = array(
    'name' => t('Shipping address'),
    'description' => t('The order shipping address, formatted with the Ubercart Addresses module.'),
  );
  $tokens['uc_order']['uc-addresses-billing-address'] = array(
    'name' => t('Billing address'),
    'description' => t('The order billing address, formatted with the Ubercart Addresses module.'),
  );

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
            if (count($token_pieces) >= 2) {
              $fieldname = $token_pieces[0];
              array_shift($token_pieces);
              $output_format = implode(':', $token_pieces);
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
    case 'uc_order':
      if (empty($data['uc_order'])) {
        // No order object, can happen. RulesTokenEvaluator sometimes sends empty data.
        // Found this out after a long debug session in the automated tests.
        return array();
      }
      $order = $data['uc_order'];
      foreach ($tokens as $name => $original) {
        switch ($name) {
          case 'uc-addresses-shipping-address':
            if (isset($order->uc_addresses['shipping']) && $order->uc_addresses['shipping'] instanceof UcAddressesAddress) {
              $replacements[$original] = uc_addresses_format_address($order->uc_addresses['shipping'], 'token');
            }
            break;
          case 'uc-addresses-billing-address':
            if (isset($order->uc_addresses['billing']) && $order->uc_addresses['billing'] instanceof UcAddressesAddress) {
              $replacements[$original] = uc_addresses_format_address($order->uc_addresses['billing'], 'token');
            }
            break;
        }
      }
      break;
    case 'user':
      $user = $data['user'];
      $user_tokens = array(
        'billing' => 'default-billing-address',
        'shipping' => 'default-shipping-address',
      );
      foreach ($user_tokens as $address_type => $token) {
        $address_tokens = token_find_with_prefix($tokens, $token);
        if (!isset($tokens[$token]) && !$address_tokens) {
          continue;
        }
        $address = UcAddressesAddressBook::get($user)->getDefaultAddress($address_type);
        if (!$address) {
          continue;
        }

        if (isset($tokens[$token])) {
          $replacements[$tokens[$token]] = uc_addresses_format_address($address, 'token');
        }
        if ($address_tokens) {
          $replacements += token_generate('uc_addresses', $address_tokens, array('uc_addresses' => $address), $options);
        }
      }
      break;
  }
  return $replacements;
}
