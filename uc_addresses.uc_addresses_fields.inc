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
  $data['zone'] = array(
    'handler' => 'UcAddressesUcZoneFieldHandler',
    'display_settings' => $ubercart_display_settings,
  );
  $data['postal_code'] = array(
    'handler' => 'UcAddressesUcTextFieldHandler',
    'display_settings' => $ubercart_display_settings,
  );
  $data['country'] = array(
    'handler' => 'UcAddressesUcCountryFieldHandler',
    'display_settings' => $ubercart_display_settings,
  );

  // Ubercart Addresses fields
  $data['address_name'] = array(
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
    'handler' => 'UcAddressesDefaultAddressFieldHandler',
    'display_settings' => array(
      'default' => TRUE,
      'address_view' => FALSE,
      'checkout_form' => FALSE,
      'checkout_review' => FALSE,
      'order_form' => FALSE,
      'order_view' => FALSE,
    ),
    'strings' => array(
      'title' => t('Default shipping address'),
      'suffix' => t('Use as my default shipping address'),
      'type' => 'shipping', // String is used internally only, so no translation is needed here
    ),
    'compare' => FALSE,
  );
  $data['default_billing'] = array(
    'handler' => 'UcAddressesDefaultAddressFieldHandler',
    'display_settings' => array(
      'default' => TRUE,
      'address_view' => FALSE,
      'checkout_form' => FALSE,
      'checkout_review' => FALSE,
      'order_form' => FALSE,
      'order_view' => FALSE,
    ),
    'strings' => array(
      'title' => t('Default billing address'),
      'suffix' => t('Use as my default billing address'),
      'type' => 'billing', // String is used internally only, so no translation is needed here
    ),
    'compare' => FALSE,
  );
  // Non editable fields
  $data['aid'] = array(
    'handler' => 'UcAddressesHiddenFieldHandler',
    'display_settings' => array(
      'default' => FALSE,
    ),
    'strings' => array(
      'title' => t('Address ID'),
    ),
    'compare' => FALSE,
  );
  $data['uid'] = array(
    'handler' => 'UcAddressesHiddenFieldHandler',
    'display_settings' => array(
      'default' => FALSE,
    ),
    'strings' => array(
      'title' => t('User ID'),
    ),
    'compare' => FALSE,
  );
  $data['created'] = array(
    'handler' => 'UcAddressesDateFieldHandler',
    'display_settings' => array(
      'default' => FALSE,
    ),
    'strings' => array(
      'title' => t('Created'),
    ),
    'compare' => FALSE,
  );
  $data['modified'] = array(
    'handler' => 'UcAddressesDateFieldHandler',
    'display_settings' => array(
      'default' => FALSE,
    ),
    'strings' => array(
      'title' => t('Modified'),
    ),
    'compare' => FALSE,
  );

  return $data;
}
