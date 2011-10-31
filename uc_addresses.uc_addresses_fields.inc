<?php
/**
 * @file
 * In this file fields and field handlers for Ubercart Addresses are registered.
 */

// ---------------------------------------------------------------------------
// UC_ADDRESSES HOOKS
// ---------------------------------------------------------------------------

/**
 * Implementation of hook_uc_addresses_field_handlers().
 *
 * Register form fields and their definitions.
 *
 * @return array
 */
function uc_addresses_uc_addresses_field_handlers() {
  $path = drupal_get_path('module', 'uc_addresses') .'/handlers';

  $info = array();

  // Base class
  $info['UcAddressesFieldHandler'] = array(
    'hidden' => TRUE,
    'handler' => array(
      'class' => 'UcAddressesFieldHandler',
      'file' => 'UcAddressesFieldHandler.class.php',
      'path' => $path,
    ),
  );

  // Ubercart
  $file = 'ubercart.handlers.inc';
  $info['UcAddressesUcFieldHandler'] = array(
    'hidden' => TRUE,
    'handler' => array(
      'parent' => 'UcAddressesFieldHandler',
      'class' => 'UcAddressesUcFieldHandler',
      'file' => $file,
      'path' => $path,
    ),
  );
  $info['UcAddressesUcTextFieldHandler'] = array(
    'handler' => array(
      'parent' => 'UcAddressesUcFieldHandler',
      'class' => 'UcAddressesUcTextFieldHandler',
      'file' => $file,
      'path' => $path,
    ),
  );
  $info['UcAddressesUcZoneFieldHandler'] = array(
    'handler' => array(
      'parent' => 'UcAddressesUcFieldHandler',
      'class' => 'UcAddressesUcZoneFieldHandler',
      'file' => $file,
      'path' => $path,
    ),
  );
  $info['UcAddressesUcCountryFieldHandler'] = array(
    'handler' => array(
      'parent' => 'UcAddressesUcFieldHandler',
      'class' => 'UcAddressesUcCountryFieldHandler',
      'file' => $file,
      'path' => $path,
    ),
  );

  // Ubercart Addresses
  $file = 'uc_addresses.handlers.inc';
  $info['UcAddressesAddressNameFieldHandler'] = array(
    'handler' => array(
      'parent' => 'UcAddressesFieldHandler',
      'class' => 'UcAddressesAddressNameFieldHandler',
      'file' => $file,
      'path' => $path,
    ),
  );
  $info['UcAddressesDefaultAddressFieldHandler'] = array(
    'handler' => array(
      'parent' => 'UcAddressesFieldHandler',
      'class' => 'UcAddressesDefaultAddressFieldHandler',
      'file' => $file,
      'path' => $path,
    ),
  );
  $info['UcAddressesDateFieldHandler'] = array(
    'handler' => array(
      'parent' => 'UcAddressesFieldHandler',
      'class' => 'UcAddressesDateFieldHandler',
      'file' => $file,
      'path' => $path,
    ),
  );
  $info['UcAddressesHiddenFieldHandler'] = array(
    'handler' => array(
      'parent' => 'UcAddressesFieldHandler',
      'class' => 'UcAddressesHiddenFieldHandler',
      'file' => $file,
      'path' => $path,
    ),
  );

  return $info;
}

/**
 * Implementation of hook_uc_addresses_fields().
 *
 * Register all address fields used by Ubercart Addresses.
 *
 * @return array
 */
function uc_addresses_uc_addresses_fields() {
  $data = array();

  // Display settings for Ubercart Core Fields (except phone)
  $ubercart_display_settings = array(
    'default' => TRUE,
    'address_view' => FALSE,
    'checkout_review' => FALSE,
    'order_view' => FALSE,
  );

  // Ubercart Core Fields
  $data['first_name'] = array(
    'handler' => 'UcAddressesUcTextFieldHandler',
    'display_settings' => $ubercart_display_settings,
  );
  $data['last_name'] = array(
    'handler' => 'UcAddressesUcTextFieldHandler',
    'display_settings' => $ubercart_display_settings,
  );
  $data['phone'] = array(
    'handler' => 'UcAddressesUcTextFieldHandler',
  );
  $data['company'] = array(
    'handler' => 'UcAddressesUcTextFieldHandler',
    'display_settings' => $ubercart_display_settings,
  );
  $data['street1'] = array(
    'handler' => 'UcAddressesUcTextFieldHandler',
    'display_settings' => $ubercart_display_settings,
  );
  $data['street2'] = array(
    'handler' => 'UcAddressesUcTextFieldHandler',
    'display_settings' => $ubercart_display_settings,
  );
  $data['city'] = array(
    'handler' => 'UcAddressesUcTextFieldHandler',
    'display_settings' => $ubercart_display_settings,
  );
  $data['country'] = array(
    'handler' => 'UcAddressesUcCountryFieldHandler',
    'display_settings' => $ubercart_display_settings,
  );
  $data['zone'] = array(
    'handler' => 'UcAddressesUcZoneFieldHandler',
    'display_settings' => $ubercart_display_settings,
  );
  $data['postal_code'] = array(
    'handler' => 'UcAddressesUcTextFieldHandler',
    'display_settings' => $ubercart_display_settings,
  );

  // Ubercart Addresses fields
  $data['address_name'] = array(
    'title' => t('Name'),
    'handler' => 'UcAddressesAddressNameFieldHandler',
    'display_settings' => array(
      'default' => TRUE,
      'checkout_form' => FALSE,
      'checkout_review' => FALSE,
      'order_form' => FALSE,
      'order_view' => FALSE,
    ),
    'compare' => FALSE,
  );
  $data['default_shipping'] = array(
    'title' => t('Default shipping address'),
    'handler' => 'UcAddressesDefaultAddressFieldHandler',
    'display_settings' => array(
      'default' => TRUE,
      'address_view' => FALSE,
      'checkout_form' => FALSE,
      'checkout_review' => FALSE,
      'order_form' => FALSE,
      'order_view' => FALSE,
    ),
    'compare' => FALSE,
    'suffix' => t('Use as my default shipping address'),
    'default_type' => 'shipping',
  );
  $data['default_billing'] = array(
    'title' => t('Default billing address'),
    'handler' => 'UcAddressesDefaultAddressFieldHandler',
    'display_settings' => array(
      'default' => TRUE,
      'address_view' => FALSE,
      'checkout_form' => FALSE,
      'checkout_review' => FALSE,
      'order_form' => FALSE,
      'order_view' => FALSE,
    ),
    'compare' => FALSE,
    'suffix' => t('Use as my default billing address'),
    'default_type' => 'billing',
  );
  // Non editable fields
  $data['aid'] = array(
    'title' => t('Address ID'),
    'handler' => 'UcAddressesHiddenFieldHandler',
    'display_settings' => array(
      'default' => FALSE,
    ),
    'compare' => FALSE,
  );
  $data['uid'] = array(
    'title' => t('User ID'),
    'handler' => 'UcAddressesHiddenFieldHandler',
    'display_settings' => array(
      'default' => FALSE,
    ),
    'compare' => FALSE,
  );
  $data['created'] = array(
    'title' => t('Created'),
    'handler' => 'UcAddressesDateFieldHandler',
    'display_settings' => array(
      'default' => FALSE,
    ),
    'compare' => FALSE,
  );
  $data['modified'] = array(
    'title' => t('Modified'),
    'handler' => 'UcAddressesDateFieldHandler',
    'display_settings' => array(
      'default' => FALSE,
    ),
    'compare' => FALSE,
  );

  return $data;
}

/**
 * Implementation of hook_uc_addresses_select_addresses().
 *
 * @param int $uid
 *   The user ID to select addresses for
 * @param string $context
 *   The context in which the addresses are used:
 *   - checkout_form
 *   - order_form
 * @param string $type
 *   The type of address to select addresses for (shipping or billing)
 *
 * @return array
 *   An array of UcAddressesAddress objects.
 *   Or an array of address arrays.
 * @todo Add setting to say if addresses may come from previous orders or not.
 */
function uc_addresses_uc_addresses_select_addresses($uid, $context, $type) {
  static $addresses = array();

  if (count($addresses) > 0) {
    return $addresses;
  }

  // Address book selecting
  $uc_addresses = UcAddressesAddressBook::get($uid)->getAddresses();
  if (count($uc_addresses) > 0) {
    foreach ($uc_addresses as $address) {
      if ($address->isNew()) {
        // Skip unsaved addresses
        continue;
      }
      if (!UcAddressesPermissions::canViewAddress($address->getUserId(), $address)) {
        // Skip addresses that the currently logged in user may not view.
        continue;
      }
      $addresses[] = $address;
    }
  }

  // If there no addresses in the address book, try to select addresses from
  // previous orders.
  if (count($addresses) < 1) {
    $addresses = uc_get_addresses($uid, $type);
    foreach ($addresses as $index => $address) {
      // Set source module manually this time.
      $addresses[$index]['module'] = 'uc_order';
    }
  }
  return $addresses;
}
